/***********************************************************************
 * Module:  ControladorAsistencia.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase ControladorAsistencia
 *
 * Coordina el registro de entradas y salidas. Es dueño del historial
 * de asistencias (ListaDoble<RegistroAsistencia>) y consulta a los
 * profesores a través de GestorProfesores (asociación por referencia).
 *
 * La fecha y la hora llegan como PARÁMETROS: el Menu envía siempre el
 * reloj del equipo y las pruebas envían horas simuladas (RN17).
 ***********************************************************************/

#ifndef CONTROLADORASISTENCIA_H
#define CONTROLADORASISTENCIA_H

#include <string>
#include "ListaDoble.h"
#include "GestorProfesores.h"
#include "RegistroAsistencia.h"
#include "ResultadoAsistencia.h"
#include "OperacionDisponible.h"
#include "Fecha.h"
#include "Hora.h"

class ControladorAsistencia
{
private:
   GestorProfesores& gestor;
   ListaDoble<RegistroAsistencia> registros;

   RegistroAsistencia* buscarRegistro(const std::string& cedula, const std::string& codigoHorario,
                                      const Fecha& fecha);
   const RegistroAsistencia* buscarRegistro(const std::string& cedula, const std::string& codigoHorario,
                                            const Fecha& fecha) const;
   // Pasos comunes de validación: cédula, existencia y estado del profesor
   ResultadoAsistencia validarProfesor(const std::string& cedula, const Profesor*& profesor) const;

public:
   explicit ControladorAsistencia(GestorProfesores& gestor);

   // Llena 'operaciones' con lo que el profesor puede hacer ahora.
   // Devuelve EXITO si hay al menos una operación, o el motivo si no hay.
   ResultadoAsistencia operacionesDisponibles(const std::string& cedula, const Fecha& fecha,
                                              const Hora& hora,
                                              ListaDoble<OperacionDisponible>& operaciones) const;

   // Busca la siguiente ventana pendiente del día (para informar al profesor)
   bool proximaVentana(const std::string& cedula, const Fecha& fecha, const Hora& hora,
                       OperacionDisponible& proxima) const;

   ResultadoAsistencia registrarEntrada(const std::string& cedula, const std::string& codigoHorario,
                                        const Fecha& fecha, const Hora& hora);
   ResultadoAsistencia registrarSalida(const std::string& cedula, const std::string& codigoHorario,
                                       const Fecha& fecha, const Hora& hora);

   bool tieneRegistros(const std::string& cedula) const;
   bool horarioTieneRegistros(const std::string& codigoHorario) const;

   // Si tiene registros lo da de baja (inactivo); si no, lo elimina. RN15 / D5
   std::string eliminarProfesor(const std::string& cedula);

   // Usado al cargar archivos: valida referencias y duplicados
   bool agregarRegistroCargado(const RegistroAsistencia& registro);

   const ListaDoble<RegistroAsistencia>& getRegistros() const;
};

#endif
