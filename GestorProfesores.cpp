/***********************************************************************
 * Module:  GestorProfesores.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase GestorProfesores
 ***********************************************************************/

#include "GestorProfesores.h"
#include "Validador.h"

// Valida nombres, apellidos y departamento ya normalizados
static std::string validarDatosPersonales(const std::string& nombres, const std::string& apellidos,
                                          const std::string& departamento)
{
   if (nombres.empty())
      return "Los nombres son obligatorios";
   if (!Validador::nombrePersona(nombres))
      return "Nombres invalidos: 2 a 60 caracteres, solo letras (con tildes y n), espacios, ' y -";
   if (apellidos.empty())
      return "Los apellidos son obligatorios";
   if (!Validador::nombrePersona(apellidos))
      return "Apellidos invalidos: 2 a 60 caracteres, solo letras (con tildes y n), espacios, ' y -";
   if (!Validador::textoGeneral(departamento, 2, 60))
      return "Departamento invalido: 2 a 60 caracteres (letras, numeros, espacios, . , - ( ))";
   return "";
}

std::string GestorProfesores::registrar(const std::string& cedula, const std::string& nombres,
                                        const std::string& apellidos, const std::string& departamento)
{
   std::string ced = Validador::recortar(cedula);
   std::string error = Validador::errorCedula(ced);
   if (!error.empty())
      return error;
   if (existe(ced))
      return "Ya existe un profesor con esa cedula";

   std::string nom = Validador::normalizarEspacios(nombres);
   std::string ape = Validador::normalizarEspacios(apellidos);
   std::string dep = Validador::normalizarEspacios(departamento);
   error = validarDatosPersonales(nom, ape, dep);
   if (!error.empty())
      return error;

   profesores.insertarFinal(Profesor(ced, nom, ape, dep));
   return "";
}

std::string GestorProfesores::actualizar(const std::string& cedula, const std::string& nombres,
                                         const std::string& apellidos, const std::string& departamento)
{
   Profesor* p = buscar(Validador::recortar(cedula));
   if (p == nullptr)
      return "Profesor no encontrado";

   std::string nom = Validador::normalizarEspacios(nombres);
   std::string ape = Validador::normalizarEspacios(apellidos);
   std::string dep = Validador::normalizarEspacios(departamento);
   std::string error = validarDatosPersonales(nom, ape, dep);
   if (!error.empty())
      return error;           // no se cambia nada si algún dato es inválido

   p->setNombres(nom);
   p->setApellidos(ape);
   p->setDepartamento(dep);
   return "";
}

// Búsqueda lineal recorriendo los nodos con un puntero. O(n)
Profesor* GestorProfesores::buscar(const std::string& cedula)
{
   for (Nodo<Profesor>* n = profesores.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCedula() == cedula)
         return &n->getDato();
   return nullptr;
}

const Profesor* GestorProfesores::buscar(const std::string& cedula) const
{
   for (Nodo<Profesor>* n = profesores.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().getCedula() == cedula)
         return &n->getDato();
   return nullptr;
}

bool GestorProfesores::existe(const std::string& cedula) const
{
   return buscar(cedula) != nullptr;
}

bool GestorProfesores::eliminar(const std::string& cedula)
{
   for (Nodo<Profesor>* n = profesores.getCabeza(); n != nullptr; n = n->getSiguiente())
   {
      if (n->getDato().getCedula() == cedula)
      {
         profesores.eliminarNodo(n);   // libera el nodo (y con él la lista de horarios)
         return true;
      }
   }
   return false;
}

// El código de horario es único en TODO el sistema: se revisan los horarios
// de cada profesor (recorrido anidado). O(n * h)
bool GestorProfesores::existeCodigoHorario(const std::string& codigo) const
{
   for (Nodo<Profesor>* n = profesores.getCabeza(); n != nullptr; n = n->getSiguiente())
      if (n->getDato().buscarHorario(codigo) != nullptr)
         return true;
   return false;
}

std::string GestorProfesores::asignarHorario(const std::string& cedula, const std::string& codigoHorario,
                                             const std::string& codigoMateria, const std::string& nombreMateria,
                                             int diaSemana, const Hora& inicio, const Hora& fin, bool activo)
{
   Profesor* p = buscar(Validador::recortar(cedula));
   if (p == nullptr)
      return "Profesor no encontrado";
   if (!p->estaActivo() && activo)
      return "El profesor esta dado de baja: no se le pueden asignar horarios";

   std::string codHor = Validador::aMayusculas(Validador::recortar(codigoHorario));
   std::string codMat = Validador::aMayusculas(Validador::recortar(codigoMateria));
   std::string nomMat = Validador::normalizarEspacios(nombreMateria);

   if (!Validador::codigo(codHor))
      return "Codigo de horario invalido (1 a 15 caracteres: letras sin tilde, numeros y -)";
   if (existeCodigoHorario(codHor))
      return "El codigo de horario " + codHor + " ya existe";
   if (!Validador::codigo(codMat))
      return "Codigo de materia invalido (1 a 15 caracteres: letras sin tilde, numeros y -)";
   if (!Validador::textoGeneral(nomMat, 2, 60))
      return "Nombre de materia invalido (2 a 60 caracteres)";

   std::string error = Horario::validar(diaSemana, inicio, fin);
   if (!error.empty())
      return error;

   Horario nuevo(codHor, codMat, nomMat, diaSemana, inicio, fin, activo);
   const Horario* conflicto = p->buscarConflicto(nuevo);
   if (activo && conflicto != nullptr)
      return "Conflicto con el horario " + conflicto->getCodigoHorario() + " (" +
             Horario::nombreDia(conflicto->getDiaSemana()) + " " +
             conflicto->getHoraInicio().aTextoCorto() + "-" + conflicto->getHoraFin().aTextoCorto() + ")";

   p->agregarHorario(nuevo);
   return "";
}

std::string GestorProfesores::desactivarHorario(const std::string& cedula, const std::string& codigoHorario)
{
   Profesor* p = buscar(Validador::recortar(cedula));
   if (p == nullptr)
      return "Profesor no encontrado";
   Horario* h = p->buscarHorario(Validador::aMayusculas(Validador::recortar(codigoHorario)));
   if (h == nullptr)
      return "El profesor no tiene un horario con ese codigo";
   if (!h->estaActivo())
      return "El horario ya estaba desactivado";
   h->setActivo(false);
   return "";
}

const ListaDoble<Profesor>& GestorProfesores::getProfesores() const
{
   return profesores;
}
