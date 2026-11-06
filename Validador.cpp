/***********************************************************************
 * Module:  Validador.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Validador
 ***********************************************************************/

#include "Validador.h"

// ------------------------------------------------------------------ Texto

std::string Validador::recortar(const std::string& texto)
{
   const char* espacios = " \t\r\n";
   std::size_t inicio = texto.find_first_not_of(espacios);
   if (inicio == std::string::npos)
      return "";
   std::size_t fin = texto.find_last_not_of(espacios);
   return texto.substr(inicio, fin - inicio + 1);
}

std::string Validador::normalizarEspacios(const std::string& texto)
{
   std::string limpio = recortar(texto);
   std::string resultado;
   bool espacioAnterior = false;
   for (char c : limpio)
   {
      bool esEspacio = (c == ' ' || c == '\t');
      if (esEspacio)
      {
         if (!espacioAnterior) resultado += ' ';
      }
      else
      {
         resultado += c;
      }
      espacioAnterior = esEspacio;
   }
   return resultado;
}

int Validador::contarCaracteres(const std::string& texto)
{
   // En UTF-8 los bytes de continuación tienen la forma 10xxxxxx
   int total = 0;
   for (unsigned char c : texto)
      if ((c & 0xC0) != 0x80)
         total++;
   return total;
}

// Verifica si un texto ya está en UTF-8 válido
// Se recorre con un puntero 'p' desde el primer byte hasta 'fin'
// (la posición que está justo después del último byte).
static bool esUtf8Valido(const std::string& t)
{
   const unsigned char* p = reinterpret_cast<const unsigned char*>(t.c_str());
   const unsigned char* fin = p + t.size();
   while (p < fin)
   {
      unsigned char c = *p;
      int extra;
      if (c < 0x80)              extra = 0;
      else if ((c >> 5) == 0x6)  extra = 1;
      else if ((c >> 4) == 0xE)  extra = 2;
      else if ((c >> 3) == 0x1E) extra = 3;
      else return false;
      if (p + extra >= fin)               // faltan bytes de continuación
         return false;
      for (int k = 1; k <= extra; k++)
         if ((*(p + k) & 0xC0) != 0x80)   // cada continuación debe ser 10xxxxxx
            return false;
      p += extra + 1;                     // saltar al siguiente carácter
   }
   return true;
}

std::string Validador::aUtf8(const std::string& texto, int paginaCodigos)
{
   // La consola de Windows en español suele usar la página 850 (o 1252).
   // Si el texto ya es UTF-8 (Linux, MSYS2/mintty, o chcp 65001) se deja igual.
   if (esUtf8Valido(texto))
      return texto;

   std::string resultado;
   for (unsigned char c : texto)
   {
      const char* reemplazo = nullptr;
      if (paginaCodigos == 850)
      {
         switch (c)
         {
         case 0xA0: reemplazo = "\xC3\xA1"; break;   // á
         case 0x82: reemplazo = "\xC3\xA9"; break;   // é
         case 0xA1: reemplazo = "\xC3\xAD"; break;   // í
         case 0xA2: reemplazo = "\xC3\xB3"; break;   // ó
         case 0xA3: reemplazo = "\xC3\xBA"; break;   // ú
         case 0x81: reemplazo = "\xC3\xBC"; break;   // ü
         case 0xA4: reemplazo = "\xC3\xB1"; break;   // ñ
         case 0xB5: reemplazo = "\xC3\x81"; break;   // Á
         case 0x90: reemplazo = "\xC3\x89"; break;   // É
         case 0xD6: reemplazo = "\xC3\x8D"; break;   // Í
         case 0xE0: reemplazo = "\xC3\x93"; break;   // Ó
         case 0xE9: reemplazo = "\xC3\x9A"; break;   // Ú
         case 0x9A: reemplazo = "\xC3\x9C"; break;   // Ü
         case 0xA5: reemplazo = "\xC3\x91"; break;   // Ñ
         }
      }
      else   // 1252 (o Latin-1)
      {
         if (c >= 0xC0)
         {
            // En Latin-1 los caracteres 0xC0..0xFF pasan a UTF-8 con dos bytes
            resultado += static_cast<char>(0xC3);
            resultado += static_cast<char>(c - 0x40);
            continue;
         }
      }

      if (reemplazo != nullptr)
         resultado += reemplazo;
      else if (c < 0x80)
         resultado += static_cast<char>(c);
      else
         resultado += '?';      // carácter no reconocido: hará fallar la validación
   }
   return resultado;
}

std::string Validador::aMayusculas(const std::string& texto)
{
   std::string r = texto;
   for (char& c : r)
      if (c >= 'a' && c <= 'z')
         c = static_cast<char>(c - 'a' + 'A');
   return r;
}

// --------------------------------------------------------- Cédula ecuatoriana

std::string Validador::errorCedula(const std::string& cedula)
{
   if (cedula.empty())
      return "La cedula es obligatoria";
   if (cedula.size() != 10)
      return "La cedula debe tener 10 digitos numericos";
   for (char c : cedula)
      if (c < '0' || c > '9')
         return "La cedula debe tener 10 digitos numericos";

   // 'd' apunta al primer dígito; *(d + k) es el dígito k (contando desde 0)
   const char* d = cedula.c_str();

   int provincia = (*d - '0') * 10 + (*(d + 1) - '0');
   if (!((provincia >= 1 && provincia <= 24) || provincia == 30))
      return "Codigo de provincia invalido (01-24 o 30)";

   if (*(d + 2) - '0' >= 6)
      return "La cedula no corresponde a una persona natural (tercer digito < 6)";

   // Módulo 10: coeficientes 2,1,2,1,2,1,2,1,2 sobre los 9 primeros dígitos.
   // El puntero 'p' avanza dígito por dígito; 'coeficiente' alterna 2 y 1.
   int suma = 0;
   int coeficiente = 2;
   for (const char* p = d; p < d + 9; p++)
   {
      int producto = (*p - '0') * coeficiente;
      if (producto > 9) producto -= 9;
      suma += producto;
      coeficiente = (coeficiente == 2) ? 1 : 2;
   }
   int verificador = (10 - suma % 10) % 10;
   if (verificador != *(d + 9) - '0')        // el décimo dígito
      return "Digito verificador incorrecto";

   return "";
}

bool Validador::cedulaEcuatoriana(const std::string& cedula)
{
   return errorCedula(cedula).empty();
}

// ------------------------------------------------------ Nombres y textos

bool Validador::esLetraConTilde(unsigned char a, unsigned char b)
{
   if (a != 0xC3) return false;
   switch (b)
   {
   case 0xA1: case 0xA9: case 0xAD: case 0xB3: case 0xBA: case 0xBC: case 0xB1:   // á é í ó ú ü ñ
   case 0x81: case 0x89: case 0x8D: case 0x93: case 0x9A: case 0x9C: case 0x91:   // Á É Í Ó Ú Ü Ñ
      return true;
   default:
      return false;
   }
}

// Recorre el texto y verifica cada carácter. 'extras' son los símbolos
// ASCII permitidos además de las letras.
static bool soloPermitidos(const std::string& t, const std::string& extras, bool& tieneLetra,
                           bool (*tilde)(unsigned char, unsigned char))
{
   tieneLetra = false;
   const unsigned char* p = reinterpret_cast<const unsigned char*>(t.c_str());
   const unsigned char* fin = p + t.size();
   for (; p < fin; p++)
   {
      unsigned char c = *p;
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
      {
         tieneLetra = true;
      }
      else if (c >= 0x80)
      {
         // Letra con tilde: ocupa 2 bytes (este y el siguiente)
         if (p + 1 >= fin || !tilde(c, *(p + 1)))
            return false;
         tieneLetra = true;
         p++;                      // saltar el segundo byte
      }
      else if (extras.find(static_cast<char>(c)) == std::string::npos)
      {
         return false;
      }
   }
   return true;
}

bool Validador::nombrePersona(const std::string& nombre)
{
   int n = contarCaracteres(nombre);
   if (n < 2 || n > 60)
      return false;
   char primero = nombre.front();          // no puede empezar con apóstrofo, guion o espacio
   if (primero == '\'' || primero == '-' || primero == ' ')
      return false;
   bool tieneLetra;
   return soloPermitidos(nombre, " '-", tieneLetra, esLetraConTilde) && tieneLetra;
}

bool Validador::textoGeneral(const std::string& texto, int minimo, int maximo)
{
   int n = contarCaracteres(texto);
   if (n < minimo || n > maximo)
      return false;
   bool tieneLetra;
   return soloPermitidos(texto, " '-.,()0123456789", tieneLetra, esLetraConTilde) && tieneLetra;
}

bool Validador::codigo(const std::string& c)
{
   if (c.empty() || c.size() > 15)
      return false;
   for (char x : c)
   {
      bool ok = (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '-';
      if (!ok) return false;
   }
   return true;
}

// ------------------------------------------------------------- Números

bool Validador::diaSemana(int dia)
{
   return dia >= 1 && dia <= 5;          // clases solo de lunes (1) a viernes (5)
}

bool Validador::enteroDesdeTexto(const std::string& texto, int& resultado)
{
   std::string t = recortar(texto);
   if (t.empty() || t.size() > 9)          // máximo 9 caracteres: evita desbordamiento
      return false;
   const char* p = t.c_str();
   const char* fin = p + t.size();
   bool negativo = false;
   if (*p == '-')
   {
      negativo = true;
      p++;                                  // el signo no es un dígito
      if (p == fin) return false;           // solo "-"
   }
   int valor = 0;
   for (; p < fin; p++)
   {
      if (*p < '0' || *p > '9')
         return false;
      valor = valor * 10 + (*p - '0');
   }
   resultado = negativo ? -valor : valor;
   return true;
}
