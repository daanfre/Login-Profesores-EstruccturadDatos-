/***********************************************************************
 * Module:  GestorProfesores.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase GestorProfesores
 *
 * Administra la lista de profesores y sus horarios (registrar, buscar,
 * actualizar, asignar y desactivar horarios). Valida los datos con
 * Validador antes de modificar las listas.
 * Los métodos que modifican devuelven "" si todo salió bien o el
 * mensaje de error.
 ***********************************************************************/

#ifndef GESTORPROFESORES_H
#define GESTORPROFESORES_H

#include <string>
#include "ListaDoble.h"
#include "Profesor.h"

class GestorProfesores
{
private:
   ListaDoble<Profesor> profesores;

public:
   std::string registrar(const std::string& cedula, const std::string& nombres,
                         const std::string& apellidos, const std::string& departamento);
   std::string actualizar(const std::string& cedula, const std::string& nombres,
                          const std::string& apellidos, const std::string& departamento);

   Profesor* buscar(const std::string& cedula);
   const Profesor* buscar(const std::string& cedula) const;
   bool existe(const std::string& cedula) const;
   bool eliminar(const std::string& cedula);      // elimina físicamente (el controlador decide si se puede)

   bool existeCodigoHorario(const std::string& codigo) const;
   std::string asignarHorario(const std::string& cedula, const std::string& codigoHorario,
                              const std::string& codigoMateria, const std::string& nombreMateria,
                              int diaSemana, const Hora& inicio, const Hora& fin, bool activo = true);
   std::string desactivarHorario(const std::string& cedula, const std::string& codigoHorario);

   const ListaDoble<Profesor>& getProfesores() const;
};

#endif
