/***********************************************************************
 * Module:  ControladorAsistencia.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase ControladorAsistencia
 ***********************************************************************/

#include "ControladorAsistencia.h"
#include "Validador.h"

ControladorAsistencia::ControladorAsistencia(GestorProfesores& g) : gestor(g)
{
}

// Los registros se insertan en orden cronológico, por eso se busca desde
// la cola: los más recientes (los de hoy) se encuentran primero.
RegistroAsistencia* ControladorAsistencia::buscarRegistro(const std::string& cedula,
                                                          const std::string& codigo, const Fecha& fecha)
{
   // Recorrido hacia atrás: desde la cola, avanzando con getAnterior()
   for (Nodo<RegistroAsistencia>* n = registros.getCola(); n != nullptr; n = n->getAnterior())
      if (n->getDato().coincide(cedula, codigo, fecha))
         return &n->getDato();
   return nullptr;
}

const RegistroAsistencia* ControladorAsistencia::buscarRegistro(const std::string& cedula,
                                                                const std::string& codigo,
                                                                const Fecha& fecha) const
{
   for (Nodo<RegistroAsistencia>* n = registros.getCola(); n != nullptr; n = n->getAnterior())
      if (n->getDato().coincide(cedula, codigo, fecha))
         return &n->getDato();
   return nullptr;
}

ResultadoAsistencia ControladorAsistencia::validarProfesor(const std::string& cedula,
                                                           const Profesor*& profesor) const
{
   if (!Validador::cedulaEcuatoriana(cedula))
      return ResultadoAsistencia::CEDULA_INVALIDA;
   profesor = gestor.buscar(cedula);
   if (profesor == nullptr)
      return ResultadoAsistencia::PROFESOR_NO_EXISTE;
   if (!profesor->estaActivo())
      return ResultadoAsistencia::PROFESOR_INACTIVO;
   return ResultadoAsistencia::EXITO;
}

ResultadoAsistencia ControladorAsistencia::operacionesDisponibles(const std::string& cedula,
                                                                  const Fecha& fecha, const Hora& hora,
                                                                  ListaDoble<OperacionDisponible>& operaciones) const
{
   operaciones.vaciar();
   const Profesor* p = nullptr;
   ResultadoAsistencia r = validarProfesor(cedula, p);
   if (r != ResultadoAsistencia::EXITO)
      return r;

   int dia = fecha.diaSemana();
   int clasesHoy = 0;

   for (Nodo<Horario>* n = p->getHorarios().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const Horario& h = n->getDato();
      if (!h.estaActivo() || h.getDiaSemana() != dia)
         continue;                                   // no es una clase de hoy
      clasesHoy++;
      const RegistroAsistencia* reg = buscarRegistro(cedula, h.getCodigoHorario(), fecha);

      if (h.permiteEntrada(dia, hora) && reg == nullptr)
         operaciones.insertarFinal({ TipoOperacion::ENTRADA, h.getCodigoHorario(), h.getCodigoMateria(),
                                     h.getNombreMateria(), h.getHoraInicio(), h.finVentanaEntrada() });

      if (h.permiteSalida(dia, hora) && reg != nullptr && !reg->tieneSalida())
         operaciones.insertarFinal({ TipoOperacion::SALIDA, h.getCodigoHorario(), h.getCodigoMateria(),
                                     h.getNombreMateria(), h.getHoraFin(), h.finVentanaSalida() });
   }

   if (clasesHoy == 0)
      return ResultadoAsistencia::SIN_CLASES_HOY;
   if (operaciones.estaVacia())
      return ResultadoAsistencia::SIN_OPERACIONES_DISPONIBLES;
   return ResultadoAsistencia::EXITO;
}

bool ControladorAsistencia::proximaVentana(const std::string& cedula, const Fecha& fecha,
                                           const Hora& hora, OperacionDisponible& proxima) const
{
   const Profesor* p = gestor.buscar(cedula);
   if (p == nullptr || !p->estaActivo())
      return false;

   int dia = fecha.diaSemana();
   bool hay = false;
   for (Nodo<Horario>* n = p->getHorarios().getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const Horario& h = n->getDato();
      if (!h.estaActivo() || h.getDiaSemana() != dia)
         continue;
      const RegistroAsistencia* reg = buscarRegistro(cedula, h.getCodigoHorario(), fecha);
      // Entrada pendiente que todavía no abre
      if (reg == nullptr && hora < h.getHoraInicio() && (!hay || h.getHoraInicio() < proxima.inicioVentana))
      {
         proxima = { TipoOperacion::ENTRADA, h.getCodigoHorario(), h.getCodigoMateria(),
                     h.getNombreMateria(), h.getHoraInicio(), h.finVentanaEntrada() };
         hay = true;
      }
      // Salida pendiente que todavía no abre
      if (reg != nullptr && !reg->tieneSalida() && hora < h.getHoraFin() &&
          (!hay || h.getHoraFin() < proxima.inicioVentana))
      {
         proxima = { TipoOperacion::SALIDA, h.getCodigoHorario(), h.getCodigoMateria(),
                     h.getNombreMateria(), h.getHoraFin(), h.finVentanaSalida() };
         hay = true;
      }
   }
   return hay;
}

ResultadoAsistencia ControladorAsistencia::registrarEntrada(const std::string& cedula,
                                                            const std::string& codigoHorario,
                                                            const Fecha& fecha, const Hora& hora)
{
   const Profesor* p = nullptr;
   ResultadoAsistencia r = validarProfesor(cedula, p);
   if (r != ResultadoAsistencia::EXITO)
      return r;

   const Horario* h = p->buscarHorario(codigoHorario);
   if (h == nullptr || !h->estaActivo())
      return ResultadoAsistencia::HORARIO_NO_EXISTE;

   if (buscarRegistro(cedula, codigoHorario, fecha) != nullptr)
      return ResultadoAsistencia::ENTRADA_DUPLICADA;

   if (!h->permiteEntrada(fecha.diaSemana(), hora))
      return ResultadoAsistencia::FUERA_DE_VENTANA;

   registros.insertarFinal(RegistroAsistencia(cedula, codigoHorario, fecha, hora));
   return ResultadoAsistencia::EXITO;
}

ResultadoAsistencia ControladorAsistencia::registrarSalida(const std::string& cedula,
                                                           const std::string& codigoHorario,
                                                           const Fecha& fecha, const Hora& hora)
{
   const Profesor* p = nullptr;
   ResultadoAsistencia r = validarProfesor(cedula, p);
   if (r != ResultadoAsistencia::EXITO)
      return r;

   const Horario* h = p->buscarHorario(codigoHorario);
   if (h == nullptr || !h->estaActivo())
      return ResultadoAsistencia::HORARIO_NO_EXISTE;

   RegistroAsistencia* reg = buscarRegistro(cedula, codigoHorario, fecha);
   if (reg == nullptr)
      return ResultadoAsistencia::SALIDA_SIN_ENTRADA;
   if (reg->tieneSalida())
      return ResultadoAsistencia::SALIDA_DUPLICADA;

   if (!h->permiteSalida(fecha.diaSemana(), hora))
      return ResultadoAsistencia::FUERA_DE_VENTANA;

   if (!reg->registrarSalida(hora))       // protección adicional (RN07)
      return ResultadoAsistencia::FUERA_DE_VENTANA;
   return ResultadoAsistencia::EXITO;
}

bool ControladorAsistencia::tieneRegistros(const std::string& cedula) const
{
   for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCedulaProfesor() == cedula)
         return true;
   return false;
}

bool ControladorAsistencia::horarioTieneRegistros(const std::string& codigoHorario) const
{
   for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCodigoHorario() == codigoHorario)
         return true;
   return false;
}

std::string ControladorAsistencia::eliminarProfesor(const std::string& cedula)
{
   Profesor* p = gestor.buscar(cedula);
   if (p == nullptr)
      return "Profesor no encontrado";

   if (tieneRegistros(cedula))
   {
      if (!p->estaActivo())
         return "El profesor ya estaba dado de baja";
      p->setActivo(false);
      return "El profesor tiene asistencias registradas: se dio de BAJA (su historial se conserva)";
   }
   gestor.eliminar(cedula);
   return "Profesor eliminado (no tenia asistencias registradas)";
}

bool ControladorAsistencia::agregarRegistroCargado(const RegistroAsistencia& reg)
{
   const Profesor* p = gestor.buscar(reg.getCedulaProfesor());
   if (p == nullptr)
      return false;                                  // profesor inexistente
   if (p->buscarHorario(reg.getCodigoHorario()) == nullptr)
      return false;                                  // horario inexistente
   if (buscarRegistro(reg.getCedulaProfesor(), reg.getCodigoHorario(), reg.getFecha()) != nullptr)
      return false;                                  // duplicado
   if (reg.tieneSalida() && reg.getHoraSalida() < reg.getHoraEntrada())
      return false;                                  // inconsistente
   registros.insertarFinal(reg);
   return true;
}

const ListaDoble<RegistroAsistencia>& ControladorAsistencia::getRegistros() const
{
   return registros;
}
