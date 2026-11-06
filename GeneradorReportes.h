/***********************************************************************
 * Module:  GeneradorReportes.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase GeneradorReportes
 *
 * Solo LEE la información (referencias const) y la presenta en tablas.
 * Escribe en un std::ostream para poder probar los reportes.
 ***********************************************************************/

#ifndef GENERADORREPORTES_H
#define GENERADORREPORTES_H

#include <iostream>
#include <string>
#include "GestorProfesores.h"
#include "ControladorAsistencia.h"

class GeneradorReportes
{
private:
   const GestorProfesores& gestor;
   const ControladorAsistencia& controlador;

   void encabezadoRegistros(std::ostream& salida) const;
   void filaRegistro(std::ostream& salida, const RegistroAsistencia& r,
                     const Fecha& hoy, const Hora& ahora) const;
   // "COMPLETO", "EN CURSO" (aún puede marcar salida) o "INCOMPLETO" (RN13)
   std::string estadoRegistro(const RegistroAsistencia& r, const Fecha& hoy, const Hora& ahora) const;
   const Horario* horarioDe(const RegistroAsistencia& r) const;

public:
   GeneradorReportes(const GestorProfesores& gestor, const ControladorAsistencia& controlador);

   void listarProfesores(std::ostream& salida) const;
   void horarioDeProfesor(const std::string& cedula, std::ostream& salida) const;
   void porProfesor(const std::string& cedula, const Fecha& hoy, const Hora& ahora, std::ostream& salida) const;
   void porFecha(const Fecha& fecha, const Fecha& hoy, const Hora& ahora, std::ostream& salida) const;
   void porMateria(const std::string& codigoMateria, const Fecha& hoy, const Hora& ahora, std::ostream& salida) const;
   void incompletos(const Fecha& hoy, const Hora& ahora, std::ostream& salida) const;
   void resumenPorProfesor(const Fecha& hoy, const Hora& ahora, std::ostream& salida) const;

   bool esIncompleto(const RegistroAsistencia& r, const Fecha& hoy, const Hora& ahora) const;
};

#endif
