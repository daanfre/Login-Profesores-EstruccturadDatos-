/***********************************************************************
 * Module:  main.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Punto de entrada del Sistema de Control de Asistencia Docente
 *
 * Compilar (MSYS2 UCRT64, desde la carpeta del proyecto):
 *   g++ -std=c++17 -Wall -Wextra -I. *.cpp -o asistencia.exe
 ***********************************************************************/

#include <iostream>
#include "GestorProfesores.h"
#include "ControladorAsistencia.h"
#include "GeneradorReportes.h"
#include "GestorArchivos.h"
#include "Menu.h"

#ifdef _WIN32
#ifndef NOMINMAX                 // algunas versiones de g++ ya lo definen
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
   SetConsoleOutputCP(CP_UTF8);     // mostrar tildes y ñ guardadas en UTF-8
#endif

   // Todos los objetos viven en main: las referencias entre ellos son
   // válidas mientras el programa se ejecuta.
   GestorProfesores gestor;
   ControladorAsistencia controlador(gestor);
   GeneradorReportes reportes(gestor, controlador);
   GestorArchivos archivos("datos");

   std::string informe;
   archivos.cargar(gestor, controlador, informe);
   std::cout << informe << "\n";

   Menu menu(gestor, controlador, reportes, archivos);
   menu.ejecutar();
   return 0;
}
