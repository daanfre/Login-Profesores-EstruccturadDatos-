/***********************************************************************
 * Module:  Menu.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración de la clase Menu
 *
 * Única clase que lee del teclado. Toda lectura se hace con getline y
 * luego se convierte y valida, para que una entrada incorrecta nunca
 * bloquee cin ni cierre el programa.
 *
 * Modo profesor: solo marca asistencia con su cédula, usando SIEMPRE la
 * fecha y hora del equipo (no se puede escribir la hora).
 * Modo administración: protegido con clave.
 ***********************************************************************/

#ifndef MENU_H
#define MENU_H

#include <string>
#include "GestorProfesores.h"
#include "ControladorAsistencia.h"
#include "GeneradorReportes.h"
#include "GestorArchivos.h"

class Menu
{
private:
   GestorProfesores& gestor;
   ControladorAsistencia& controlador;
   GeneradorReportes& reportes;
   GestorArchivos& archivos;
   bool finEntrada;                       // true cuando cin llega a fin de archivo (Ctrl+Z / Ctrl+D)

   // ---- Lectura segura ----
   bool leerLinea(const std::string& mensaje, std::string& linea);
   int leerOpcion(const std::string& mensaje, int minimo, int maximo);   // -1 si se terminó la entrada
   bool leerHora(const std::string& mensaje, Hora& hora);
   bool leerCedula(std::string& cedula);
   bool confirmar(const std::string& mensaje);
   void pausa();

   // ---- Mensajes ----
   static std::string textoResultado(ResultadoAsistencia r);
   void autoguardar();

   // ---- Pantallas ----
   void menuMarcarAsistencia();
   void autenticarAdministrador();     // si la clave es correcta abre menuAdministracion()
   void menuAdministracion();
   void menuProfesores();
   void menuHorarios();
   void menuReportes();

   void registrarProfesor();
   void actualizarProfesor();
   void eliminarProfesor();
   void reactivarProfesor();
   void asignarHorario();
   void desactivarHorario();

public:
   Menu(GestorProfesores& gestor, ControladorAsistencia& controlador,
        GeneradorReportes& reportes, GestorArchivos& archivos);
   void ejecutar();
};

#endif
