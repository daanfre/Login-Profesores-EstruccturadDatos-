/***********************************************************************
 * Module:  Validador.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Validador
 *
 * Clase de utilidad (solo métodos estáticos) que centraliza las reglas
 * de validación. La usan GestorProfesores, ControladorAsistencia,
 * GestorArchivos y Menu, así las reglas no se duplican.
 *
 * Los textos se manejan en UTF-8. Letras con tilde aceptadas:
 * á é í ó ú ü ñ (mayúsculas y minúsculas).
 ***********************************************************************/

#ifndef VALIDADOR_H
#define VALIDADOR_H

#include <string>

class Validador
{
public:
   static const int LONGITUD_MAXIMA_ENTRADA = 200;

   // ---- Texto ----
   static std::string recortar(const std::string& texto);           // quita espacios al inicio y final
   static std::string normalizarEspacios(const std::string& texto); // recorta y deja un solo espacio entre palabras
   static int contarCaracteres(const std::string& textoUtf8);       // caracteres reales (no bytes)
   static std::string aUtf8(const std::string& texto, int paginaCodigos);  // convierte la entrada de consola de Windows
   static std::string aMayusculas(const std::string& textoAscii);

   // ---- Cédula ecuatoriana ----
   // Devuelve "" si es válida, o el motivo del error.
   static std::string errorCedula(const std::string& cedula);
   static bool cedulaEcuatoriana(const std::string& cedula);

   // ---- Nombres y textos ----
   static bool nombrePersona(const std::string& nombre);            // 2..60 caracteres: letras, espacio, ' y -
   static bool textoGeneral(const std::string& texto, int minimo, int maximo);  // + dígitos y . , ( )
   static bool codigo(const std::string& codigo);                   // 1..15: letras sin tilde, dígitos y -

   // ---- Números ----
   static bool diaSemana(int dia);                                  // 1..5 (lunes a viernes)
   static bool enteroDesdeTexto(const std::string& texto, int& resultado);   // solo dígitos (y signo -)

private:
   static bool esLetraConTilde(unsigned char primero, unsigned char segundo);
};

#endif
