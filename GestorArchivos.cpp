
#include "GestorArchivos.h"
#include "Validador.h"
#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;
GestorArchivos::GestorArchivos(const std::string& c) : carpeta(c)
{
}
std::string GestorArchivos::ruta(const std::string& archivo) const
{
   return (fs::path(carpeta) / archivo).string();
}
int GestorArchivos::dividir(const std::string& linea, ListaDoble<std::string>& campos)
{
   campos.vaciar();
   std::string actual;
   for (char c : linea)
   {
      if (c == ';')
      {
         if (campos.getCantidad() >= MAX_CAMPOS) return -1;
         campos.insertarFinal(actual);   // cada campo es un nodo nuevo
         actual.clear();
      }
      else if (c != '\r')         // tolera archivos con fin de línea de Windows
      {
         actual += c;
      }
   }
   if (campos.getCantidad() >= MAX_CAMPOS) return -1;
   campos.insertarFinal(actual);
   return campos.getCantidad();
}
bool GestorArchivos::escribirSeguro(const std::string& archivo, const std::string& contenido,
                                    std::string& error) const
{
   std::string destino = ruta(archivo);
   std::string temporal = destino + ".tmp";
   {
      std::ofstream out(temporal, std::ios::binary | std::ios::trunc);
      if (!out)
      {
         error = "No se pudo crear " + temporal;
         return false;
      }
      out << contenido;
      out.flush();
      if (!out)
      {
         error = "Error al escribir " + temporal;
         return false;
      }
   }
   std::error_code ec;
   fs::rename(temporal, destino, ec);          // reemplaza el archivo anterior
   if (ec)
   {
      // Respaldo por si el sistema no permite reemplazar al renombrar
      std::error_code ec2;
      fs::remove(destino, ec2);
      ec.clear();
      fs::rename(temporal, destino, ec);
   }
   if (ec)
   {
      error = "No se pudo reemplazar " + destino + ": " + ec.message();
      return false;
   }
   return true;
}
bool GestorArchivos::guardar(const GestorProfesores& gestor, const ControladorAsistencia& controlador,
                             std::string& error) const
{
   std::error_code ec;
   fs::create_directories(carpeta, ec);
   if (ec)
   {
      error = "No se pudo crear la carpeta " + carpeta;
      return false;
   }
   std::ostringstream prof, hor, asis;
   for (Nodo<Profesor>* np = gestor.getProfesores().getCabeza(); np != nullptr; np = np->getSiguiente())
   {
      const Profesor& p = np->getDato();
      prof << p.getCedula() << ';' << p.getNombres() << ';' << p.getApellidos() << ';'
           << p.getDepartamento() << ';' << (p.estaActivo() ? 1 : 0) << '\n';
      for (Nodo<Horario>* nh = p.getHorarios().getCabeza(); nh != nullptr; nh = nh->getSiguiente())
      {
         const Horario& h = nh->getDato();
         hor << p.getCedula() << ';' << h.getCodigoHorario() << ';' << h.getCodigoMateria() << ';'
             << h.getNombreMateria() << ';' << h.getDiaSemana() << ';'
             << h.getHoraInicio().aTextoCorto() << ';' << h.getHoraFin().aTextoCorto() << ';'
             << (h.estaActivo() ? 1 : 0) << '\n';
      }
   }
   for (Nodo<RegistroAsistencia>* nr = controlador.getRegistros().getCabeza(); nr != nullptr;
        nr = nr->getSiguiente())
   {
      const RegistroAsistencia& r = nr->getDato();
      asis << r.getCedulaProfesor() << ';' << r.getCodigoHorario() << ';' << r.getFecha().aTexto() << ';'
           << r.getHoraEntrada().aTexto() << ';' << (r.tieneSalida() ? r.getHoraSalida().aTexto() : "-") << '\n';
   }
   return escribirSeguro("profesores.txt", prof.str(), error) &&
          escribirSeguro("horarios.txt", hor.str(), error) &&
          escribirSeguro("asistencias.txt", asis.str(), error);
}
int GestorArchivos::cargar(GestorProfesores& gestor, ControladorAsistencia& controlador,
                           std::string& informe) const
{
   int descartadas = 0;
   int profesoresCargados = 0, horariosCargados = 0, asistenciasCargadas = 0;
   std::ostringstream detalle;
   ListaDoble<std::string> campos;     
   ListaDoble<std::string> inactivos;   
   {
      std::ifstream in(ruta("profesores.txt"));
      std::string linea;
      int num = 0;
      while (in && std::getline(in, linea))
      {
         num++;
         if (Validador::recortar(linea).empty()) continue;
         int n = dividir(linea, campos);
         if (n != 5 || (campos.obtener(4) != "0" && campos.obtener(4) != "1"))
         {
            descartadas++;
            detalle << "  profesores.txt linea " << num << ": formato invalido\n";
            continue;
         }
         std::string error = gestor.registrar(campos.obtener(0), campos.obtener(1), campos.obtener(2), campos.obtener(3));
         if (!error.empty())
         {
            descartadas++;
            detalle << "  profesores.txt linea " << num << ": " << error << "\n";
            continue;
         }
         if (campos.obtener(4) == "0") inactivos.insertarFinal(campos.obtener(0));
         profesoresCargados++;
      }
   }
   {
      std::ifstream in(ruta("horarios.txt"));
      std::string linea;
      int num = 0;
      while (in && std::getline(in, linea))
      {
         num++;
         if (Validador::recortar(linea).empty()) continue;
         int n = dividir(linea, campos);
         int dia = 0;
         Hora inicio, fin;
         if (n != 8 || !Validador::enteroDesdeTexto(campos.obtener(4), dia) ||
             !Hora::desdeTexto(campos.obtener(5), inicio) || !Hora::desdeTexto(campos.obtener(6), fin) ||
             (campos.obtener(7) != "0" && campos.obtener(7) != "1"))
         {
            descartadas++;
            detalle << "  horarios.txt linea " << num << ": formato invalido\n";
            continue;
         }
         std::string error = gestor.asignarHorario(campos.obtener(0), campos.obtener(1), campos.obtener(2), campos.obtener(3),
                                                   dia, inicio, fin, campos.obtener(7) == "1");
         if (!error.empty())
         {
            descartadas++;
            detalle << "  horarios.txt linea " << num << ": " << error << "\n";
            continue;
         }
         horariosCargados++;
      }
   }
   {
      std::ifstream in(ruta("asistencias.txt"));
      std::string linea;
      int num = 0;
      while (in && std::getline(in, linea))
      {
         num++;
         if (Validador::recortar(linea).empty()) continue;
         int n = dividir(linea, campos);
         Fecha fecha;
         Hora entrada, salida;
         bool pendiente = (n == 5 && campos.obtener(4) == "-");
         if (n != 5 || !Fecha::desdeTexto(campos.obtener(2), fecha) || !Hora::desdeTexto(campos.obtener(3), entrada) ||
             (!pendiente && !Hora::desdeTexto(campos.obtener(4), salida)))
         {
            descartadas++;
            detalle << "  asistencias.txt linea " << num << ": formato invalido\n";
            continue;
         }
         RegistroAsistencia reg(campos.obtener(0), campos.obtener(1), fecha, entrada);
         if ((!pendiente && !reg.registrarSalida(salida)) || !controlador.agregarRegistroCargado(reg))
         {
            descartadas++;
            detalle << "  asistencias.txt linea " << num << ": referencia inexistente, duplicado o inconsistente\n";
            continue;
         }
         asistenciasCargadas++;
      }
   }
   for (Nodo<std::string>* n = inactivos.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      Profesor* p = gestor.buscar(n->getDato());
      if (p != nullptr) p->setActivo(false);
   }
   std::ostringstream res;
   res << "Datos cargados: " << profesoresCargados << " profesor(es), " << horariosCargados
       << " horario(s), " << asistenciasCargadas << " asistencia(s).";
   if (descartadas > 0)
      res << "\nATENCION: " << descartadas << " linea(s) invalida(s) omitida(s):\n" << detalle.str();
   informe = res.str();
   return descartadas;
}