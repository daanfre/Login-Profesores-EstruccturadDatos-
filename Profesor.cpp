/***********************************************************************
 * Module:  Profesor.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Profesor
 ***********************************************************************/

#include "Profesor.h"

Profesor::Profesor(const std::string& ced, const std::string& nom,
                   const std::string& ape, const std::string& depto, bool estaActivo)
   : cedula(ced), nombres(nom), apellidos(ape), departamento(depto), activo(estaActivo)
{
}

const std::string& Profesor::getCedula() const       { return cedula; }
const std::string& Profesor::getNombres() const      { return nombres; }
const std::string& Profesor::getApellidos() const    { return apellidos; }
const std::string& Profesor::getDepartamento() const { return departamento; }
bool Profesor::estaActivo() const                    { return activo; }

std::string Profesor::getNombreCompleto() const
{
   return nombres + " " + apellidos;
}

void Profesor::setNombres(const std::string& valor)      { nombres = valor; }
void Profesor::setApellidos(const std::string& valor)    { apellidos = valor; }
void Profesor::setDepartamento(const std::string& valor) { departamento = valor; }
void Profesor::setActivo(bool valor)                     { activo = valor; }

// Recorre los horarios con un puntero y devuelve el primero ACTIVO que se
// solapa con 'nuevo' (los horarios inactivos no generan conflicto).
const Horario* Profesor::buscarConflicto(const Horario& nuevo) const
{
   for (Nodo<Horario>* n = horarios.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      const Horario& h = n->getDato();
      if (h.estaActivo() && h.seSolapaCon(nuevo))
         return &h;
   }
   return nullptr;
}

bool Profesor::agregarHorario(const Horario& nuevo)
{
   if (nuevo.estaActivo() && buscarConflicto(nuevo) != nullptr)
      return false;

   // Contar cuántos horarios van ANTES del nuevo (orden por día y hora de inicio).
   // Al primer horario que va después, se detiene el recorrido.
   int posicion = 0;
   Nodo<Horario>* n = horarios.getCabeza();
   while (n != nullptr)
   {
      const Horario& h = n->getDato();
      bool nuevoVaAntes = nuevo.getDiaSemana() < h.getDiaSemana() ||
                          (nuevo.getDiaSemana() == h.getDiaSemana() && nuevo.getHoraInicio() < h.getHoraInicio());
      if (nuevoVaAntes)
         break;
      posicion++;
      n = n->getSiguiente();
   }
   horarios.insertarEnPosicion(posicion, nuevo);
   return true;
}

Horario* Profesor::buscarHorario(const std::string& codigo)
{
   for (Nodo<Horario>* n = horarios.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCodigoHorario() == codigo)
         return &n->getDato();
   return nullptr;
}

const Horario* Profesor::buscarHorario(const std::string& codigo) const
{
   for (Nodo<Horario>* n = horarios.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCodigoHorario() == codigo)
         return &n->getDato();
   return nullptr;
}

bool Profesor::eliminarHorario(const std::string& codigo)
{
   for (Nodo<Horario>* n = horarios.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      if (n->getDato().getCodigoHorario() == codigo)
      {
         horarios.eliminarNodo(n);     // desenlace O(1): el nodo conoce a sus vecinos
         return true;
      }
   }
   return false;
}

const ListaDoble<Horario>& Profesor::getHorarios() const
{
   return horarios;
}
