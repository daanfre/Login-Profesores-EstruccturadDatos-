/***********************************************************************
 * Module:  Fecha.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Fecha
 ***********************************************************************/

#include "Fecha.h"
#include <ctime>
#include <stdexcept>

// Convierte un número a texto con al menos 'ancho' dígitos, rellenando
// con ceros a la izquierda: (7, 2) -> "07", (2026, 4) -> "2026".
static std::string conCeros(int valor, int ancho)
{
   std::string texto = std::to_string(valor);
   while ((int)texto.size() < ancho)
      texto = "0" + texto;
   return texto;
}

// Desfase de cada mes que usa el algoritmo de Sakamoto (enero = 0 ... diciembre = 4).
static int desfaseMes(int mes)
{
   switch (mes)
   {
   case 1:  return 0;
   case 2:  return 3;
   case 3:  return 2;
   case 4:  return 5;
   case 5:  return 0;
   case 6:  return 3;
   case 7:  return 5;
   case 8:  return 1;
   case 9:  return 4;
   case 10: return 6;
   case 11: return 2;
   default: return 4;   // diciembre
   }
}

Fecha::Fecha() : anio(2000), mes(1), dia(1)
{
}

Fecha::Fecha(int a, int m, int d)
{
   if (!esValida(a, m, d))
      throw std::invalid_argument("Fecha invalida");
   anio = a;
   mes = m;
   dia = d;
}

bool Fecha::esBisiesto(int a)
{
   return (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
}

int Fecha::diaDelMes(int a, int m)
{
   switch (m)
   {
   case 2:
      return esBisiesto(a) ? 29 : 28;
   case 4: case 6: case 9: case 11:
      return 30;
   default:
      return 31;
   }
}

bool Fecha::esValida(int a, int m, int d)
{
   if (a < ANIO_MINIMO || a > ANIO_MAXIMO) return false;
   if (m < 1 || m > 12) return false;
   return d >= 1 && d <= diaDelMes(a, m);
}

Fecha Fecha::hoy()
{
   std::time_t t = std::time(nullptr);
   std::tm* local = std::localtime(&t);
   return Fecha(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
}

bool Fecha::desdeTexto(const std::string& texto, Fecha& resultado)
{
   // Formato estricto: AAAA-MM-DD (10 caracteres)
   if (texto.size() != 10)
      return false;
   // Se recorre carácter por carácter llevando la posición:
   // en las posiciones 4 y 7 debe ir '-', en las demás un dígito.
   int posicion = 0;
   for (char c : texto)
   {
      if (posicion == 4 || posicion == 7)
      {
         if (c != '-') return false;
      }
      else if (c < '0' || c > '9')
      {
         return false;
      }
      posicion++;
   }
   int a = std::stoi(texto.substr(0, 4));
   int m = std::stoi(texto.substr(5, 2));
   int d = std::stoi(texto.substr(8, 2));
   if (!esValida(a, m, d))
      return false;
   resultado = Fecha(a, m, d);
   return true;
}

int Fecha::getAnio() const { return anio; }
int Fecha::getMes() const  { return mes; }
int Fecha::getDia() const  { return dia; }

int Fecha::diaSemana() const
{
   // Algoritmo de Sakamoto: devuelve 0 = domingo ... 6 = sábado
   int a = anio;
   if (mes < 3) a -= 1;               // enero y febrero cuentan como meses del año anterior
   int d = (a + a / 4 - a / 100 + a / 400 + desfaseMes(mes) + dia) % 7;
   return (d == 0) ? 7 : d;          // convertir a 1 = lunes ... 7 = domingo
}

std::string Fecha::aTexto() const
{
   return conCeros(anio, 4) + "-" + conCeros(mes, 2) + "-" + conCeros(dia, 2);
}

bool Fecha::operator==(const Fecha& o) const
{
   return anio == o.anio && mes == o.mes && dia == o.dia;
}

bool Fecha::operator!=(const Fecha& o) const
{
   return !(*this == o);
}

bool Fecha::operator<(const Fecha& o) const
{
   if (anio != o.anio) return anio < o.anio;
   if (mes != o.mes) return mes < o.mes;
   return dia < o.dia;
}
