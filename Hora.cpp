/***********************************************************************
 * Module:  Hora.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Hora
 ***********************************************************************/

#include "Hora.h"
#include <ctime>
#include <stdexcept>

Hora::Hora() : segundosDelDia(0)
{
}

Hora::Hora(int hora, int minuto, int segundo)
{
   if (!esValida(hora, minuto, segundo))
      throw std::invalid_argument("Hora invalida");
   segundosDelDia = hora * 3600 + minuto * 60 + segundo;
}

bool Hora::esValida(int hora, int minuto, int segundo)
{
   return hora >= 0 && hora <= 23 &&
          minuto >= 0 && minuto <= 59 &&
          segundo >= 0 && segundo <= 59;
}

Hora Hora::ahora()
{
   std::time_t t = std::time(nullptr);
   std::tm* local = std::localtime(&t);
   return Hora(local->tm_hour, local->tm_min, local->tm_sec);
}

// Número de 0 a 99 a texto de 2 dígitos: 7 -> "07"
static std::string dosDigitos(int valor)
{
   return (valor < 10 ? "0" : "") + std::to_string(valor);
}

// Convierte un número de 1 o 2 dígitos. Devuelve -1 si no es válido.
static int leerNumero(const std::string& texto)
{
   if (texto.empty() || texto.size() > 2)
      return -1;
   int valor = 0;
   for (char c : texto)
   {
      if (c < '0' || c > '9')
         return -1;
      valor = valor * 10 + (c - '0');
   }
   return valor;
}

bool Hora::desdeTexto(const std::string& texto, Hora& resultado)
{
   // Formatos aceptados: H:MM, HH:MM, HH:MM:SS (los minutos y segundos con 2 dígitos)
   std::size_t p1 = texto.find(':');
   if (p1 == std::string::npos)
      return false;
   std::size_t p2 = texto.find(':', p1 + 1);

   std::string sh = texto.substr(0, p1);
   std::string sm = (p2 == std::string::npos) ? texto.substr(p1 + 1) : texto.substr(p1 + 1, p2 - p1 - 1);
   std::string ss = (p2 == std::string::npos) ? "00" : texto.substr(p2 + 1);

   if (sm.size() != 2 || ss.size() != 2)
      return false;

   int h = leerNumero(sh), m = leerNumero(sm), s = leerNumero(ss);
   if (h < 0 || m < 0 || s < 0 || !esValida(h, m, s))
      return false;

   resultado = Hora(h, m, s);
   return true;
}

int Hora::getHora() const    { return segundosDelDia / 3600; }
int Hora::getMinuto() const  { return (segundosDelDia % 3600) / 60; }
int Hora::getSegundo() const { return segundosDelDia % 60; }
int Hora::getSegundosDelDia() const { return segundosDelDia; }

bool Hora::sumarMinutos(int minutos, Hora& resultado) const
{
   int total = segundosDelDia + minutos * 60;
   if (total < 0 || total >= SEGUNDOS_POR_DIA)
      return false;               // cruzaría la medianoche
   resultado.segundosDelDia = total;
   return true;
}

std::string Hora::aTexto() const
{
   return dosDigitos(getHora()) + ":" + dosDigitos(getMinuto()) + ":" + dosDigitos(getSegundo());
}

std::string Hora::aTextoCorto() const
{
   return dosDigitos(getHora()) + ":" + dosDigitos(getMinuto());
}

bool Hora::operator==(const Hora& otra) const { return segundosDelDia == otra.segundosDelDia; }
bool Hora::operator!=(const Hora& otra) const { return segundosDelDia != otra.segundosDelDia; }
bool Hora::operator<(const Hora& otra) const  { return segundosDelDia < otra.segundosDelDia; }
bool Hora::operator<=(const Hora& otra) const { return segundosDelDia <= otra.segundosDelDia; }
bool Hora::operator>(const Hora& otra) const  { return segundosDelDia > otra.segundosDelDia; }
bool Hora::operator>=(const Hora& otra) const { return segundosDelDia >= otra.segundosDelDia; }
