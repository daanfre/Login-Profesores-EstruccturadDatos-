/***********************************************************************
 * Module:  RegistroAsistencia.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase RegistroAsistencia
 ***********************************************************************/

#include "RegistroAsistencia.h"

RegistroAsistencia::RegistroAsistencia(const std::string& cedula, const std::string& codigo,
                                       const Fecha& f, const Hora& entrada)
   : cedulaProfesor(cedula), codigoHorario(codigo), fecha(f), horaEntrada(entrada), horaSalida()
{
}

bool RegistroAsistencia::registrarSalida(const Hora& hora)
{
   if (horaSalida.has_value())     // salida duplicada
      return false;
   if (hora < horaEntrada)         // inconsistencia cronológica
      return false;
   horaSalida = hora;
   return true;
}

bool RegistroAsistencia::tieneSalida() const
{
   return horaSalida.has_value();
}

bool RegistroAsistencia::coincide(const std::string& cedula, const std::string& codigo,
                                  const Fecha& f) const
{
   return cedulaProfesor == cedula && codigoHorario == codigo && fecha == f;
}

const std::string& RegistroAsistencia::getCedulaProfesor() const { return cedulaProfesor; }
const std::string& RegistroAsistencia::getCodigoHorario() const  { return codigoHorario; }
const Fecha& RegistroAsistencia::getFecha() const                { return fecha; }
const Hora& RegistroAsistencia::getHoraEntrada() const           { return horaEntrada; }

Hora RegistroAsistencia::getHoraSalida() const
{
   return horaSalida.value();     // lanza std::bad_optional_access si no hay salida
}
