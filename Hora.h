/***********************************************************************
 * Module:  Hora.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Hora
 *
 * Hora del día guardada como segundos desde la medianoche (0..86399).
 * Así comparar dos horas es comparar dos enteros y la ventana de
 * 10 minutos se calcula con precisión de segundos (07:10:00 válido,
 * 07:10:01 inválido).
 ***********************************************************************/

#ifndef HORA_H
#define HORA_H

#include <string>

class Hora
{
private:
   int segundosDelDia;

public:
   static const int SEGUNDOS_POR_DIA = 86400;

   Hora();                                // 00:00:00
   Hora(int hora, int minuto, int segundo = 0);   // lanza std::invalid_argument si no es válida

   static bool esValida(int hora, int minuto, int segundo);
   static Hora ahora();                   // hora del reloj del equipo
   static bool desdeTexto(const std::string& texto, Hora& resultado);   // "HH:MM" o "HH:MM:SS"

   int getHora() const;
   int getMinuto() const;
   int getSegundo() const;
   int getSegundosDelDia() const;

   // Devuelve true y deja el resultado en 'resultado' si no se pasa de 23:59:59
   bool sumarMinutos(int minutos, Hora& resultado) const;

   std::string aTexto() const;            // "HH:MM:SS"
   std::string aTextoCorto() const;       // "HH:MM"

   bool operator==(const Hora& otra) const;
   bool operator!=(const Hora& otra) const;
   bool operator<(const Hora& otra) const;
   bool operator<=(const Hora& otra) const;
   bool operator>(const Hora& otra) const;
   bool operator>=(const Hora& otra) const;
};

#endif
