/***********************************************************************
 * Module:  Horario.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Horario
 *
 * Una clase programada semanal de un profesor. Conoce sus propias
 * ventanas de entrada y salida (principio del experto en información).
 ***********************************************************************/

#ifndef HORARIO_H
#define HORARIO_H

#include <string>
#include "Hora.h"

class Horario
{
public:
   static const int VENTANA_MINUTOS = 10;        // RN01, RN02
   static const int DURACION_MINIMA_MIN = 30;    // RN12 (decisión del equipo)

private:
   std::string codigoHorario;
   std::string codigoMateria;
   std::string nombreMateria;
   int diaSemana;               // 1 = lunes ... 5 = viernes (solo dias laborables)
   Hora horaInicio;
   Hora horaFin;
   bool activo;

public:
   // Lanza std::invalid_argument si no cumple validar(...)
   Horario(const std::string& codigoHorario, const std::string& codigoMateria,
           const std::string& nombreMateria, int diaSemana,
           const Hora& horaInicio, const Hora& horaFin, bool activo = true);

   // Reglas de tiempo de un horario. Devuelve "" si es válido o el motivo del error.
   static std::string validar(int diaSemana, const Hora& inicio, const Hora& fin);
   static std::string nombreDia(int diaSemana);

   const std::string& getCodigoHorario() const;
   const std::string& getCodigoMateria() const;
   const std::string& getNombreMateria() const;
   int getDiaSemana() const;
   const Hora& getHoraInicio() const;
   const Hora& getHoraFin() const;
   bool estaActivo() const;
   void setActivo(bool activo);

   int duracionMinutos() const;
   Hora finVentanaEntrada() const;     // inicio + 10 min
   Hora finVentanaSalida() const;      // fin + 10 min

   bool seSolapaCon(const Horario& otro) const;
   bool permiteEntrada(int dia, const Hora& hora) const;
   bool permiteSalida(int dia, const Hora& hora) const;
};

#endif
