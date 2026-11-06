/***********************************************************************
 * Module:  Profesor.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Profesor
 *
 * Composición: el profesor es dueño de su lista de horarios.
 * Los horarios se guardan ordenados por día y hora de inicio.
 ***********************************************************************/

#ifndef PROFESOR_H
#define PROFESOR_H

#include <string>
#include "ListaDoble.h"
#include "Horario.h"

class Profesor
{
private:
   std::string cedula;
   std::string nombres;
   std::string apellidos;
   std::string departamento;
   bool activo;
   ListaDoble<Horario> horarios;

public:
   Profesor(const std::string& cedula, const std::string& nombres,
            const std::string& apellidos, const std::string& departamento,
            bool activo = true);

   const std::string& getCedula() const;
   const std::string& getNombres() const;
   const std::string& getApellidos() const;
   const std::string& getDepartamento() const;
   std::string getNombreCompleto() const;
   bool estaActivo() const;

   // La cédula NO tiene setter: es el identificador del profesor.
   void setNombres(const std::string& nombres);
   void setApellidos(const std::string& apellidos);
   void setDepartamento(const std::string& departamento);
   void setActivo(bool activo);

   // Inserta en orden (día, hora de inicio). Devuelve false si se solapa
   // con un horario activo (los horarios inactivos no generan conflicto).
   bool agregarHorario(const Horario& horario);
   Horario* buscarHorario(const std::string& codigo);
   const Horario* buscarHorario(const std::string& codigo) const;
   const Horario* buscarConflicto(const Horario& horario) const;
   bool eliminarHorario(const std::string& codigo);
   const ListaDoble<Horario>& getHorarios() const;
};

#endif
