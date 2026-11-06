/***********************************************************************
 * Module:  RegistroAsistencia.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase RegistroAsistencia
 *
 * Asistencia de un profesor a UNA sesión de clase.
 * Clave única: (cedulaProfesor, codigoHorario, fecha).
 * Se vincula al profesor y al horario por CLAVE, no por puntero, para
 * que nunca quede apuntando a memoria liberada.
 * La salida pendiente se representa con std::optional vacío (sin
 * valores ambiguos como -1).
 ***********************************************************************/

#ifndef REGISTROASISTENCIA_H
#define REGISTROASISTENCIA_H

#include <string>
#include <optional>
#include "Fecha.h"
#include "Hora.h"

class RegistroAsistencia
{
private:
   std::string cedulaProfesor;
   std::string codigoHorario;
   Fecha fecha;
   Hora horaEntrada;
   std::optional<Hora> horaSalida;

public:
   // El registro nace con la entrada (RN05)
   RegistroAsistencia(const std::string& cedulaProfesor, const std::string& codigoHorario,
                      const Fecha& fecha, const Hora& horaEntrada);

   // false si ya tenía salida o si la hora es anterior a la entrada (RN06, RN07)
   bool registrarSalida(const Hora& hora);

   bool tieneSalida() const;
   bool coincide(const std::string& cedula, const std::string& codigo, const Fecha& fecha) const;

   const std::string& getCedulaProfesor() const;
   const std::string& getCodigoHorario() const;
   const Fecha& getFecha() const;
   const Hora& getHoraEntrada() const;
   Hora getHoraSalida() const;          // precondición: tieneSalida()
};

#endif
