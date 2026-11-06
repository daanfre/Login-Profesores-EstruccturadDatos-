/***********************************************************************
 * Module:  GestorArchivos.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase GestorArchivos
 *
 * Persistencia en 3 archivos de texto UTF-8 separados por ';':
 *   profesores.txt  cedula;nombres;apellidos;departamento;activo(1|0)
 *   horarios.txt    cedula;codigo;codMateria;nomMateria;dia;HH:MM;HH:MM;activo(1|0)
 *   asistencias.txt cedula;codigo;AAAA-MM-DD;HH:MM:SS;HH:MM:SS o "-" (salida pendiente)
 *
 * En memoria los datos siguen viviendo en las listas doblemente enlazadas;
 * los archivos solo sirven para recuperarlos al reiniciar el programa.
 ***********************************************************************/

#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include "GestorProfesores.h"
#include "ControladorAsistencia.h"

class GestorArchivos
{
private:
   std::string carpeta;
   static const int MAX_CAMPOS = 8;

   std::string ruta(const std::string& archivo) const;
   // Escribe primero en "archivo.tmp" y luego lo renombra: si el programa
   // se corta a la mitad, el archivo original queda intacto.
   bool escribirSeguro(const std::string& archivo, const std::string& contenido, std::string& error) const;
   // Separa una línea por ';' y deja cada campo como un nodo de la lista 'campos'.
   // Devuelve la cantidad de campos o -1 si hay más de MAX_CAMPOS.
   static int dividir(const std::string& linea, ListaDoble<std::string>& campos);

public:
   explicit GestorArchivos(const std::string& carpeta = "datos");

   bool guardar(const GestorProfesores& gestor, const ControladorAsistencia& controlador,
                std::string& error) const;
   // Precondición: gestor y controlador vacíos. Devuelve la cantidad de líneas descartadas.
   int cargar(GestorProfesores& gestor, ControladorAsistencia& controlador, std::string& informe) const;
};

#endif
