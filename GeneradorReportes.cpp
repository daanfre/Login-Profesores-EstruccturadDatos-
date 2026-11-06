/***********************************************************************
 * Module:  GeneradorReportes.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase GeneradorReportes
 ***********************************************************************/

#include "GeneradorReportes.h"
#include "Validador.h"
#include <iomanip>

GeneradorReportes::GeneradorReportes(const GestorProfesores& g, const ControladorAsistencia& c)
   : gestor(g), controlador(c)
{
}

const Horario* GeneradorReportes::horarioDe(const RegistroAsistencia& r) const
{
   const Profesor* p = gestor.buscar(r.getCedulaProfesor());
   return (p == nullptr) ? nullptr : p->buscarHorario(r.getCodigoHorario());
}

bool GeneradorReportes::esIncompleto(const RegistroAsistencia& r, const Fecha& hoy, const Hora& ahora) const
{
   if (r.tieneSalida())
      return false;
   if (r.getFecha() < hoy)
      return true;                                  // un día anterior sin salida
   if (r.getFecha() == hoy)
   {
      const Horario* h = horarioDe(r);
      return h != nullptr && ahora > h->finVentanaSalida();   // la ventana ya se cerró
   }
   return false;
}

std::string GeneradorReportes::estadoRegistro(const RegistroAsistencia& r, const Fecha& hoy,
                                              const Hora& ahora) const
{
   if (r.tieneSalida()) return "COMPLETO";
   if (esIncompleto(r, hoy, ahora)) return "INCOMPLETO";
   return "EN CURSO";
}

void GeneradorReportes::encabezadoRegistros(std::ostream& out) const
{
   out << std::left << std::setw(12) << "Fecha" << std::setw(12) << "Cedula"
       << std::setw(10) << "Horario" << std::setw(10) << "Materia"
       << std::setw(10) << "Entrada" << std::setw(10) << "Salida" << "Estado\n";
   out << std::string(76, '-') << "\n";
}

void GeneradorReportes::filaRegistro(std::ostream& out, const RegistroAsistencia& r,
                                     const Fecha& hoy, const Hora& ahora) const
{
   const Horario* h = horarioDe(r);
   out << std::left << std::setw(12) << r.getFecha().aTexto()
       << std::setw(12) << r.getCedulaProfesor()
       << std::setw(10) << r.getCodigoHorario()
       << std::setw(10) << (h ? h->getCodigoMateria() : "?")
       << std::setw(10) << r.getHoraEntrada().aTexto()
       << std::setw(10) << (r.tieneSalida() ? r.getHoraSalida().aTexto() : "--:--:--")
       << estadoRegistro(r, hoy, ahora) << "\n";
}

void GeneradorReportes::listarProfesores(std::ostream& out) const
{
   const ListaDoble<Profesor>& lista = gestor.getProfesores();
   if (lista.estaVacia())
   {
      out << "No hay profesores registrados.\n";
      return;
   }
   out << std::left << std::setw(12) << "Cedula" << std::setw(40) << "Nombre"
       << std::setw(9) << "Horarios" << "Estado\n" << std::string(70, '-') << "\n";
   for (Nodo<Profesor>* n = lista.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const Profesor& p = n->getDato();
      out << std::left << std::setw(12) << p.getCedula() << std::setw(40) << p.getNombreCompleto()
          << std::setw(9) << p.getHorarios().getCantidad() << (p.estaActivo() ? "Activo" : "Baja") << "\n";
   }
   out << "Total: " << lista.getCantidad() << " profesor(es)\n";
}

void GeneradorReportes::horarioDeProfesor(const std::string& cedula, std::ostream& out) const
{
   const Profesor* p = gestor.buscar(cedula);
   if (p == nullptr)
   {
      out << "Profesor no encontrado.\n";
      return;
   }
   out << "Profesor: " << p->getNombreCompleto() << " (" << p->getDepartamento() << ")"
       << (p->estaActivo() ? "" : " (DADO DE BAJA)") << "\n";
   if (p->getHorarios().estaVacia())
   {
      out << "No tiene horarios asignados.\n";
      return;
   }
   out << std::left << std::setw(10) << "Codigo" << std::setw(11) << "Dia" << std::setw(14) << "Hora"
       << std::setw(10) << "Materia" << std::setw(30) << "Nombre" << "Estado\n"
       << std::string(82, '-') << "\n";
   for (Nodo<Horario>* n = p->getHorarios().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const Horario& h = n->getDato();
      out << std::left << std::setw(10) << h.getCodigoHorario()
          << std::setw(11) << Horario::nombreDia(h.getDiaSemana())
          << std::setw(14) << (h.getHoraInicio().aTextoCorto() + "-" + h.getHoraFin().aTextoCorto())
          << std::setw(10) << h.getCodigoMateria()
          << std::setw(30) << h.getNombreMateria()
          << (h.estaActivo() ? "Activo" : "Inactivo") << "\n";
   }
}

void GeneradorReportes::porProfesor(const std::string& cedula, const Fecha& hoy, const Hora& ahora,
                                    std::ostream& out) const
{
   const Profesor* p = gestor.buscar(cedula);
   if (p == nullptr)
   {
      out << "Profesor no encontrado.\n";
      return;
   }
   out << "Historial de " << p->getNombreCompleto() << "\n";
   int total = 0;
   encabezadoRegistros(out);
   for (Nodo<RegistroAsistencia>* n = controlador.getRegistros().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const RegistroAsistencia& r = n->getDato();
      if (r.getCedulaProfesor() == cedula) { filaRegistro(out, r, hoy, ahora); total++; }
   }
   out << "Registros: " << total << "\n";
}

void GeneradorReportes::porFecha(const Fecha& fecha, const Fecha& hoy, const Hora& ahora,
                                 std::ostream& out) const
{
   out << "Asistencias del " << fecha.aTexto() << "\n";
   int total = 0;
   encabezadoRegistros(out);
   for (Nodo<RegistroAsistencia>* n = controlador.getRegistros().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const RegistroAsistencia& r = n->getDato();
      if (r.getFecha() == fecha) { filaRegistro(out, r, hoy, ahora); total++; }
   }
   out << "Registros: " << total << "\n";
}

void GeneradorReportes::porMateria(const std::string& codigoMateria, const Fecha& hoy, const Hora& ahora,
                                   std::ostream& out) const
{
   std::string cod = Validador::aMayusculas(Validador::recortar(codigoMateria));
   out << "Asistencias de la materia " << cod << "\n";
   int total = 0;
   encabezadoRegistros(out);
   for (Nodo<RegistroAsistencia>* n = controlador.getRegistros().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const RegistroAsistencia& r = n->getDato();
      const Horario* h = horarioDe(r);
      if (h != nullptr && h->getCodigoMateria() == cod) { filaRegistro(out, r, hoy, ahora); total++; }
   }
   out << "Registros: " << total << "\n";
}

void GeneradorReportes::incompletos(const Fecha& hoy, const Hora& ahora, std::ostream& out) const
{
   out << "Registros INCOMPLETOS (sin salida y con la ventana de salida cerrada)\n";
   int total = 0;
   encabezadoRegistros(out);
   for (Nodo<RegistroAsistencia>* n = controlador.getRegistros().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const RegistroAsistencia& r = n->getDato();
      if (esIncompleto(r, hoy, ahora)) { filaRegistro(out, r, hoy, ahora); total++; }
   }
   out << "Registros: " << total << "\n";
}

void GeneradorReportes::resumenPorProfesor(const Fecha& hoy, const Hora& ahora, std::ostream& out) const
{
   out << std::left << std::setw(12) << "Cedula" << std::setw(36) << "Nombre"
       << std::setw(8) << "Total" << std::setw(10) << "Completos" << std::setw(10) << "En curso"
       << "Incompletos\n" << std::string(87, '-') << "\n";
   // Recorrido anidado: por cada profesor se recorren todos los registros.
   for (Nodo<Profesor>* np = gestor.getProfesores().getCabeza(); np != nullptr; np = np->getSiguiente())
   {
      const Profesor& p = np->getDato();
      int total = 0, completos = 0, enCurso = 0, incompletosN = 0;
      for (Nodo<RegistroAsistencia>* nr = controlador.getRegistros().getCabeza(); nr != nullptr;
           nr = nr->getSiguiente())
      {
         const RegistroAsistencia& r = nr->getDato();
         if (r.getCedulaProfesor() != p.getCedula())
            continue;
         total++;
         if (r.tieneSalida()) completos++;
         else if (esIncompleto(r, hoy, ahora)) incompletosN++;
         else enCurso++;
      }
      out << std::left << std::setw(12) << p.getCedula() << std::setw(36) << p.getNombreCompleto()
          << std::setw(8) << total << std::setw(10) << completos << std::setw(10) << enCurso
          << incompletosN << "\n";
   }
}
