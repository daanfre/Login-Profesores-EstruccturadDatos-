/***********************************************************************
 * Module:  Fecha.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Fecha
 *
 * Fecha del calendario (años 2000 a 2100) con validación de meses y
 * años bisiestos. diaSemana() devuelve 1 = lunes ... 7 = domingo.
 ***********************************************************************/

#ifndef FECHA_H
#define FECHA_H

#include <string>

class Fecha
{
private:
   int anio;
   int mes;
   int dia;

public:
   static const int ANIO_MINIMO = 2000;
   static const int ANIO_MAXIMO = 2100;

   Fecha();                              // 2000-01-01
   Fecha(int anio, int mes, int dia);    // lanza std::invalid_argument si no es válida

   static bool esBisiesto(int anio);
   static int diaDelMes(int anio, int mes);
   static bool esValida(int anio, int mes, int dia);
   static Fecha hoy();                                           // fecha del equipo
   static bool desdeTexto(const std::string& texto, Fecha& resultado);   // "AAAA-MM-DD"

   int getAnio() const;
   int getMes() const;
   int getDia() const;

   int diaSemana() const;                // 1 = lunes ... 7 = domingo
   std::string aTexto() const;           // "AAAA-MM-DD"

   bool operator==(const Fecha& otra) const;
   bool operator!=(const Fecha& otra) const;
   bool operator<(const Fecha& otra) const;
};

#endif
