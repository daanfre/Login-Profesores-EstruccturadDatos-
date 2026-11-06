/***********************************************************************
 * Module:  Menu.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Menu
 ***********************************************************************/

#include "Menu.h"
#include "Validador.h"
#include <iostream>

#ifdef _WIN32
#ifndef NOMINMAX                 // algunas versiones de g++ ya lo definen
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

using std::cout;
using std::string;

// Clave del modo administración (decisión del equipo, D6).
// Control académico: no es seguridad real, el valor está en el código.
static const string CLAVE_ADMIN = "espe2026";
static const int INTENTOS_CLAVE = 3;

Menu::Menu(GestorProfesores& g, ControladorAsistencia& c, GeneradorReportes& r, GestorArchivos& a)
   : gestor(g), controlador(c), reportes(r), archivos(a), finEntrada(false)
{
}

// ================================================================ Lectura

bool Menu::leerLinea(const string& mensaje, string& linea)
{
   if (finEntrada)
      return false;
   cout << mensaje;
   if (!std::getline(std::cin, linea))
   {
      finEntrada = true;              // Ctrl+Z (Windows) o Ctrl+D (Linux)
      cout << "\n";
      return false;
   }
#ifdef _WIN32
   linea = Validador::aUtf8(linea, static_cast<int>(GetConsoleCP()));
#else
   linea = Validador::aUtf8(linea, 65001);
#endif
   if (static_cast<int>(linea.size()) > Validador::LONGITUD_MAXIMA_ENTRADA)
   {
      cout << "  (!) Entrada demasiado larga (maximo " << Validador::LONGITUD_MAXIMA_ENTRADA
           << " caracteres). Se ignora.\n";
      linea.clear();
   }
   return true;
}

int Menu::leerOpcion(const string& mensaje, int minimo, int maximo)
{
   string linea;
   while (leerLinea(mensaje, linea))
   {
      int valor;
      if (!Validador::enteroDesdeTexto(linea, valor))
         cout << "  (!) Ingrese un numero.\n";
      else if (valor < minimo || valor > maximo)
         cout << "  (!) Opcion invalida. Debe estar entre " << minimo << " y " << maximo << ".\n";
      else
         return valor;
   }
   return -1;
}

bool Menu::leerHora(const string& mensaje, Hora& hora)
{
   string linea;
   while (leerLinea(mensaje, linea))
   {
      if (Hora::desdeTexto(Validador::recortar(linea), hora))
         return true;
      cout << "  (!) Formato invalido. Use HH:MM (00:00 a 23:59).\n";
   }
   return false;
}

bool Menu::leerCedula(string& cedula)
{
   string linea;
   while (leerLinea("Cedula (10 digitos, vacio para cancelar): ", linea))
   {
      cedula = Validador::recortar(linea);
      if (cedula.empty())
         return false;
      string error = Validador::errorCedula(cedula);
      if (error.empty())
         return true;
      cout << "  (!) " << error << ".\n";
   }
   return false;
}

bool Menu::confirmar(const string& mensaje)
{
   string linea;
   while (leerLinea(mensaje + " (S/N): ", linea))
   {
      string r = Validador::aMayusculas(Validador::recortar(linea));
      if (r == "S") return true;
      if (r == "N") return false;
      cout << "  (!) Responda S o N.\n";
   }
   return false;
}

void Menu::pausa()
{
   string linea;
   leerLinea("\nPresione Enter para continuar...", linea);
}

// =============================================================== Mensajes

string Menu::textoResultado(ResultadoAsistencia r)
{
   switch (r)
   {
   case ResultadoAsistencia::EXITO:                       return "Operacion registrada correctamente.";
   case ResultadoAsistencia::CEDULA_INVALIDA:             return "La cedula no es valida.";
   case ResultadoAsistencia::PROFESOR_NO_EXISTE:          return "No existe un profesor registrado con esa cedula.";
   case ResultadoAsistencia::PROFESOR_INACTIVO:           return "El profesor esta dado de baja.";
   case ResultadoAsistencia::HORARIO_NO_EXISTE:           return "El horario no existe o esta desactivado.";
   case ResultadoAsistencia::SIN_CLASES_HOY:              return "No tiene clases programadas hoy.";
   case ResultadoAsistencia::SIN_OPERACIONES_DISPONIBLES: return "No hay ninguna ventana de registro abierta en este momento.";
   case ResultadoAsistencia::FUERA_DE_VENTANA:            return "Fuera de la ventana de tiempo permitida (10 minutos).";
   case ResultadoAsistencia::ENTRADA_DUPLICADA:           return "La entrada de esta clase ya fue registrada hoy.";
   case ResultadoAsistencia::SALIDA_DUPLICADA:            return "La salida de esta clase ya fue registrada hoy.";
   case ResultadoAsistencia::SALIDA_SIN_ENTRADA:          return "No se puede registrar la salida: no hay entrada registrada.";
   }
   return "Resultado desconocido.";
}

void Menu::autoguardar()
{
   string error;
   if (!archivos.guardar(gestor, controlador, error))
      cout << "  (!) ATENCION: no se pudieron guardar los datos: " << error << "\n";
}

// ======================================================= Menú principal

void Menu::ejecutar()
{
   while (!finEntrada)
   {
      cout << "\n==============================================\n"
           << "   CONTROL DE ASISTENCIA DOCENTE\n"
           << "   " << Fecha::hoy().aTexto() << "  " << Horario::nombreDia(Fecha::hoy().diaSemana())
           << "  " << Hora::ahora().aTextoCorto() << "\n"
           << "==============================================\n"
           << " 1. Marcar asistencia (profesor)\n"
           << " 2. Administracion\n"
           << " 0. Salir\n";
      int op = leerOpcion("Opcion: ", 0, 2);
      if (op == 1) menuMarcarAsistencia();
      else if (op == 2) autenticarAdministrador();
      else break;                       // 0 o fin de entrada
   }
   autoguardar();
   cout << "Datos guardados. Hasta luego.\n";
}

// ===================================================== Marcar asistencia

void Menu::menuMarcarAsistencia()
{
   cout << "\n--- MARCAR ASISTENCIA ---\n";
   string cedula;
   if (!leerCedula(cedula))
      return;

   // La fecha y la hora SIEMPRE vienen del reloj del equipo (RN17)
   Fecha hoy = Fecha::hoy();
   Hora ahora = Hora::ahora();

   ListaDoble<OperacionDisponible> ops;
   ResultadoAsistencia r = controlador.operacionesDisponibles(cedula, hoy, ahora, ops);
   const Profesor* p = gestor.buscar(cedula);
   if (p != nullptr)
      cout << "Profesor: " << p->getNombreCompleto() << "   Hora actual: " << ahora.aTexto() << "\n";

   if (r != ResultadoAsistencia::EXITO)
   {
      cout << "  (X) " << textoResultado(r) << "\n";
      OperacionDisponible prox;
      if (controlador.proximaVentana(cedula, hoy, ahora, prox))
         cout << "      Proxima ventana: " << (prox.tipo == TipoOperacion::ENTRADA ? "ENTRADA" : "SALIDA")
              << " de " << prox.nombreMateria << " (" << prox.codigoHorario << ") de "
              << prox.inicioVentana.aTextoCorto() << " a " << prox.finVentana.aTextoCorto() << "\n";
      pausa();
      return;
   }

   // Mostrar las operaciones disponibles (máximo 2, RN10)
   cout << "Operaciones disponibles:\n";
   int i = 1;
   for (Nodo<OperacionDisponible>* n = ops.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const OperacionDisponible& o = n->getDato();
      cout << "  " << i << ". " << (o.tipo == TipoOperacion::ENTRADA ? "ENTRADA" : "SALIDA ")
           << " - " << o.nombreMateria << " (" << o.codigoHorario << ")  ventana "
           << o.inicioVentana.aTextoCorto() << "-" << o.finVentana.aTextoCorto() << "\n";
      i++;
   }

   int elegida;
   if (ops.getCantidad() == 1)
   {
      if (!confirmar("Confirmar la operacion 1"))
         return;
      elegida = 0;
   }
   else
   {
      int op = leerOpcion("Elija la operacion (0 = cancelar): ", 0, ops.getCantidad());
      if (op <= 0)
         return;
      elegida = op - 1;
   }

   const OperacionDisponible& o = ops.obtener(elegida);
   Hora horaReal = Hora::ahora();            // hora real en el momento de confirmar
   ResultadoAsistencia res = (o.tipo == TipoOperacion::ENTRADA)
      ? controlador.registrarEntrada(cedula, o.codigoHorario, hoy, horaReal)
      : controlador.registrarSalida(cedula, o.codigoHorario, hoy, horaReal);

   if (res == ResultadoAsistencia::EXITO)
   {
      cout << "  (OK) " << (o.tipo == TipoOperacion::ENTRADA ? "Entrada" : "Salida")
           << " registrada a las " << horaReal.aTexto() << " - " << o.nombreMateria << "\n";
      autoguardar();
   }
   else
   {
      cout << "  (X) " << textoResultado(res) << "\n";
   }
   pausa();
}

// =========================================================== Administración

// Pide la clave (máximo 3 intentos). Si es correcta, abre el menú de administración.
void Menu::autenticarAdministrador()
{
   string clave;
   for (int intento = 1; intento <= INTENTOS_CLAVE; intento++)
   {
      if (!leerLinea("Clave de administrador: ", clave))
         return;
      if (clave == CLAVE_ADMIN)
      {
         menuAdministracion();
         return;
      }
      cout << "  (!) Clave incorrecta (" << intento << "/" << INTENTOS_CLAVE << ").\n";
   }
   cout << "  (X) Demasiados intentos. Volviendo al menu principal.\n";
}

void Menu::menuAdministracion()
{
   while (!finEntrada)
   {
      cout << "\n--- ADMINISTRACION ---\n"
           << " 1. Profesores\n"
           << " 2. Horarios\n"
           << " 3. Reportes y consultas\n"
           << " 4. Guardar ahora\n"
           << " 5. Modo demostracion (fecha y hora simuladas, NO se guarda)\n"
           << " 0. Volver\n";
      int op = leerOpcion("Opcion: ", 0, 5);
      if (op == 1) menuProfesores();
      else if (op == 2) menuHorarios();
      else if (op == 3) menuReportes();
      else if (op == 4)
      {
         string error;
         if (archivos.guardar(gestor, controlador, error)) cout << "  (OK) Datos guardados.\n";
         else cout << "  (X) " << error << "\n";
      }
      else if (op == 5)
      {
         // Permite probar las ventanas (07:00, 07:10, 07:11, 09:05...) sin esperar
         // a esas horas. Trabaja sobre una COPIA del historial (copia profunda de
         // ListaDoble): al salir, las asistencias simuladas se descartan y el
         // historial real no se modifica (RN17).
         ControladorAsistencia demo = controlador;
         cout << "\n--- MODO DEMOSTRACION (nada de esto se guarda) ---\n";
         string cedula, textoFecha;
         while (leerCedula(cedula))
         {
            Fecha fecha;
            if (!leerLinea("Fecha simulada (AAAA-MM-DD): ", textoFecha)) return;
            if (!Fecha::desdeTexto(Validador::recortar(textoFecha), fecha))
            {
               cout << "  (!) Fecha invalida.\n";
               continue;
            }
            Hora hora;
            if (!leerHora("Hora simulada (HH:MM o HH:MM:SS): ", hora)) return;

            ListaDoble<OperacionDisponible> ops;
            ResultadoAsistencia r = demo.operacionesDisponibles(cedula, fecha, hora, ops);
            cout << "  " << fecha.aTexto() << " (" << Horario::nombreDia(fecha.diaSemana()) << ") "
                 << hora.aTexto() << "\n";
            if (r != ResultadoAsistencia::EXITO)
            {
               cout << "  (X) " << textoResultado(r) << "\n";
               continue;
            }
            int i = 1;
            for (Nodo<OperacionDisponible>* n = ops.getCabeza(); n != nullptr; n = n->getSiguiente())
            {
               const OperacionDisponible& disponible = n->getDato();
               cout << "  " << i << ". " << (disponible.tipo == TipoOperacion::ENTRADA ? "ENTRADA" : "SALIDA ")
                    << " - " << disponible.nombreMateria << " (" << disponible.codigoHorario << ")  ventana "
                    << disponible.inicioVentana.aTextoCorto() << "-" << disponible.finVentana.aTextoCorto() << "\n";
               i++;
            }
            int elegida = leerOpcion("Elija la operacion (0 = ninguna): ", 0, ops.getCantidad());
            if (elegida <= 0) continue;
            const OperacionDisponible& o = ops.obtener(elegida - 1);
            ResultadoAsistencia res = (o.tipo == TipoOperacion::ENTRADA)
               ? demo.registrarEntrada(cedula, o.codigoHorario, fecha, hora)
               : demo.registrarSalida(cedula, o.codigoHorario, fecha, hora);
            cout << "  " << (res == ResultadoAsistencia::EXITO ? "(OK) " : "(X) ") << textoResultado(res) << "\n";
         }
         cout << "Fin del modo demostracion: las asistencias simuladas se descartaron.\n";
      }
      else return;
   }
}

void Menu::menuProfesores()
{
   while (!finEntrada)
   {
      cout << "\n--- PROFESORES ---\n"
           << " 1. Registrar profesor\n"
           << " 2. Listar profesores\n"
           << " 3. Buscar profesor por cedula\n"
           << " 4. Actualizar datos\n"
           << " 5. Eliminar / dar de baja\n"
           << " 6. Reactivar profesor dado de baja\n"
           << " 0. Volver\n";
      int op = leerOpcion("Opcion: ", 0, 6);
      if (op == 1) registrarProfesor();
      else if (op == 2) { reportes.listarProfesores(cout); pausa(); }
      else if (op == 3)
      {
         string ced;
         if (leerCedula(ced)) { reportes.horarioDeProfesor(ced, cout); pausa(); }
      }
      else if (op == 4) actualizarProfesor();
      else if (op == 5) eliminarProfesor();
      else if (op == 6) reactivarProfesor();
      else return;
   }
}

void Menu::registrarProfesor()
{
   cout << "\n--- REGISTRAR PROFESOR ---\n";
   string ced, nom, ape, dep;
   if (!leerCedula(ced)) return;
   if (gestor.existe(ced)) { cout << "  (X) Ya existe un profesor con esa cedula.\n"; return; }
   if (!leerLinea("Nombres: ", nom) || !leerLinea("Apellidos: ", ape) || !leerLinea("Departamento: ", dep))
      return;
   string error = gestor.registrar(ced, nom, ape, dep);
   if (error.empty()) { cout << "  (OK) Profesor registrado.\n"; autoguardar(); }
   else cout << "  (X) " << error << ".\n";
}

void Menu::actualizarProfesor()
{
   cout << "\n--- ACTUALIZAR PROFESOR ---\n";
   string ced;
   if (!leerCedula(ced)) return;
   const Profesor* p = gestor.buscar(ced);
   if (p == nullptr) { cout << "  (X) Profesor no encontrado.\n"; return; }

   cout << "Deje vacio un campo para conservar el valor actual.\n";
   string nom, ape, dep;
   if (!leerLinea("Nombres (actual: " + p->getNombres() + "): ", nom) ||
       !leerLinea("Apellidos (actual: " + p->getApellidos() + "): ", ape) ||
       !leerLinea("Departamento (actual: " + p->getDepartamento() + "): ", dep))
      return;
   if (Validador::recortar(nom).empty()) nom = p->getNombres();
   if (Validador::recortar(ape).empty()) ape = p->getApellidos();
   if (Validador::recortar(dep).empty()) dep = p->getDepartamento();

   string error = gestor.actualizar(ced, nom, ape, dep);
   if (error.empty()) { cout << "  (OK) Datos actualizados.\n"; autoguardar(); }
   else cout << "  (X) " << error << ". No se modifico nada.\n";
}

void Menu::eliminarProfesor()
{
   cout << "\n--- ELIMINAR / DAR DE BAJA ---\n";
   string ced;
   if (!leerCedula(ced)) return;
   const Profesor* p = gestor.buscar(ced);
   if (p == nullptr) { cout << "  (X) Profesor no encontrado.\n"; return; }

   cout << "Profesor: " << p->getNombreCompleto() << "\n";
   if (controlador.tieneRegistros(ced))
      cout << "Tiene asistencias registradas: se DARA DE BAJA (no se borra su historial).\n";
   else
      cout << "No tiene asistencias: se ELIMINARA junto con sus horarios.\n";
   if (!confirmar("Desea continuar"))
   {
      cout << "  Operacion cancelada.\n";
      return;
   }
   cout << "  " << controlador.eliminarProfesor(ced) << "\n";
   autoguardar();
}

void Menu::reactivarProfesor()
{
   string ced;
   if (!leerCedula(ced)) return;
   Profesor* p = gestor.buscar(ced);
   if (p == nullptr) { cout << "  (X) Profesor no encontrado.\n"; return; }
   if (p->estaActivo()) { cout << "  El profesor ya esta activo.\n"; return; }
   p->setActivo(true);
   cout << "  (OK) Profesor reactivado.\n";
   autoguardar();
}

void Menu::menuHorarios()
{
   while (!finEntrada)
   {
      cout << "\n--- HORARIOS ---\n"
           << " 1. Asignar horario a un profesor\n"
           << " 2. Ver horarios de un profesor\n"
           << " 3. Desactivar horario\n"
           << " 0. Volver\n";
      int op = leerOpcion("Opcion: ", 0, 3);
      if (op == 1) asignarHorario();
      else if (op == 2)
      {
         string ced;
         if (leerCedula(ced)) { reportes.horarioDeProfesor(ced, cout); pausa(); }
      }
      else if (op == 3) desactivarHorario();
      else return;
   }
}

void Menu::asignarHorario()
{
   cout << "\n--- ASIGNAR HORARIO ---\n";
   string ced;
   if (!leerCedula(ced)) return;
   const Profesor* p = gestor.buscar(ced);
   if (p == nullptr) { cout << "  (X) Profesor no encontrado.\n"; return; }
   if (!p->estaActivo()) { cout << "  (X) El profesor esta dado de baja.\n"; return; }
   cout << "Profesor: " << p->getNombreCompleto() << "\n";

   string codHor, codMat, nomMat;
   if (!leerLinea("Codigo de horario (ej. H-001): ", codHor) ||
       !leerLinea("Codigo de materia (ej. MAT101): ", codMat) ||
       !leerLinea("Nombre de materia: ", nomMat))
      return;
   cout << "Dia: 1=Lunes 2=Martes 3=Miercoles 4=Jueves 5=Viernes\n";
   int dia = leerOpcion("Dia: ", 1, 5);
   if (dia < 0) return;
   Hora inicio, fin;
   if (!leerHora("Hora de inicio (HH:MM): ", inicio) || !leerHora("Hora de fin (HH:MM): ", fin))
      return;

   string error = gestor.asignarHorario(ced, codHor, codMat, nomMat, dia, inicio, fin);
   if (error.empty()) { cout << "  (OK) Horario asignado.\n"; autoguardar(); }
   else cout << "  (X) " << error << ".\n";
}

void Menu::desactivarHorario()
{
   cout << "\n--- DESACTIVAR HORARIO ---\n";
   string ced, cod;
   if (!leerCedula(ced)) return;
   reportes.horarioDeProfesor(ced, cout);
   if (!gestor.existe(ced)) return;
   if (!leerLinea("Codigo del horario a desactivar: ", cod)) return;
   if (!confirmar("El horario dejara de aceptar asistencias. Continuar")) return;
   string error = gestor.desactivarHorario(ced, cod);
   if (error.empty()) { cout << "  (OK) Horario desactivado (su historial se conserva).\n"; autoguardar(); }
   else cout << "  (X) " << error << ".\n";
}

void Menu::menuReportes()
{
   while (!finEntrada)
   {
      cout << "\n--- REPORTES Y CONSULTAS ---\n"
           << " 1. Historial por profesor (cedula)\n"
           << " 2. Asistencias por fecha\n"
           << " 3. Asistencias por materia\n"
           << " 4. Registros incompletos\n"
           << " 5. Resumen por profesor\n"
           << " 0. Volver\n";
      int op = leerOpcion("Opcion: ", 0, 5);
      if (op <= 0) return;

      Fecha hoy = Fecha::hoy();
      Hora ahora = Hora::ahora();
      if (op == 1)
      {
         string ced;
         if (leerCedula(ced)) reportes.porProfesor(ced, hoy, ahora, cout);
      }
      else if (op == 2)
      {
         string txt;
         Fecha f;
         if (!leerLinea("Fecha (AAAA-MM-DD, vacio = hoy): ", txt)) return;
         txt = Validador::recortar(txt);
         if (txt.empty()) f = hoy;
         else if (!Fecha::desdeTexto(txt, f)) { cout << "  (!) Fecha invalida.\n"; continue; }
         reportes.porFecha(f, hoy, ahora, cout);
      }
      else if (op == 3)
      {
         string cod;
         if (!leerLinea("Codigo de materia: ", cod)) return;
         reportes.porMateria(cod, hoy, ahora, cout);
      }
      else if (op == 4) reportes.incompletos(hoy, ahora, cout);
      else if (op == 5) reportes.resumenPorProfesor(hoy, ahora, cout);
      pausa();
   }
}
