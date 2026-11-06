/***********************************************************************
 * Module:  OperacionDisponible.h
 * Author:  usuario
 * Modified: jueves, 5 de noviembre de 2026
 * Purpose: Declaración del struct OperacionDisponible
 *
 * Una operación que el profesor puede realizar en este momento
 * (por ejemplo, a las 09:05: salida de Matemáticas o entrada de Física).
 ***********************************************************************/

#ifndef OPERACIONDISPONIBLE_H
#define OPERACIONDISPONIBLE_H

#include <string>
#include "Hora.h"
#include "TipoOperacion.h"

struct OperacionDisponible
{
   TipoOperacion tipo;
   std::string codigoHorario;
   std::string codigoMateria;
   std::string nombreMateria;
   Hora inicioVentana;
   Hora finVentana;
};

#endif
