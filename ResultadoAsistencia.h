/***********************************************************************
 * Module:  ResultadoAsistencia.h
 * Author:  usuario
 * Modified: jueves, 5 de noviembre de 2026
 * Purpose: Declaración del enum ResultadoAsistencia
 *
 * El controlador devuelve estos códigos en vez de imprimir mensajes:
 * así la lógica no depende de la consola y se puede probar.
 ***********************************************************************/

#ifndef RESULTADOASISTENCIA_H
#define RESULTADOASISTENCIA_H

enum class ResultadoAsistencia
{
   EXITO,
   CEDULA_INVALIDA,
   PROFESOR_NO_EXISTE,
   PROFESOR_INACTIVO,
   HORARIO_NO_EXISTE,
   SIN_CLASES_HOY,
   SIN_OPERACIONES_DISPONIBLES,   // tiene clases hoy, pero ahora no hay ventana abierta pendiente
   FUERA_DE_VENTANA,
   ENTRADA_DUPLICADA,
   SALIDA_DUPLICADA,
   SALIDA_SIN_ENTRADA
};

#endif
