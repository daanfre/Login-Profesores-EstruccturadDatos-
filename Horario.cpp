/***********************************************************************
 * Module:  Horario.cpp
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Implementación de la clase Horario
 ***********************************************************************/

#include "Horario.h"
#include <stdexcept>

Horario::Horario(const std::string& codigo, const std::string& codMateria,
                 const std::string& nomMateria, int dia,
                 const Hora& inicio, const Hora& fin, bool estaActivo)
   : codigoHorario(codigo), codigoMateria(codMateria), nombreMateria(nomMateria),
     diaSemana(dia), horaInicio(inicio), horaFin(fin), activo(estaActivo)
{
   std::string error = validar(dia, inicio, fin);
   if (!error.empty())
      throw std::invalid_argument(error);
}

std::string Horario::validar(int dia, const Hora& inicio, const Hora& fin)
{
   if (dia < 1 || dia > 5)
      return "Dia invalido: solo hay clases de lunes (1) a viernes (5)";
   if (fin <= inicio)
      return "La hora de fin debe ser posterior a la hora de inicio";
   int duracion = (fin.getSegundosDelDia() - inicio.getSegundosDelDia()) / 60;
   if (duracion < DURACION_MINIMA_MIN)
      return "La duracion minima de una clase es " + std::to_string(DURACION_MINIMA_MIN) + " minutos";
   Hora limite;
   if (!fin.sumarMinutos(VENTANA_MINUTOS, limite))
      return "No se admiten horarios que crucen la medianoche (la hora de fin maxima es 23:49)";
   return "";
}

std::string Horario::nombreDia(int dia)
{
   switch (dia)
   {
   case 1:  return "Lunes";
   case 2:  return "Martes";
   case 3:  return "Miercoles";
   case 4:  return "Jueves";
   case 5:  return "Viernes";
   case 6:  return "Sabado";
   case 7:  return "Domingo";
   default: return "?";
   }
}

const std::string& Horario::getCodigoHorario() const { return codigoHorario; }
const std::string& Horario::getCodigoMateria() const { return codigoMateria; }
const std::string& Horario::getNombreMateria() const { return nombreMateria; }
int Horario::getDiaSemana() const { return diaSemana; }
const Hora& Horario::getHoraInicio() const { return horaInicio; }
const Hora& Horario::getHoraFin() const { return horaFin; }
bool Horario::estaActivo() const { return activo; }
void Horario::setActivo(bool valor) { activo = valor; }

int Horario::duracionMinutos() const
{
   return (horaFin.getSegundosDelDia() - horaInicio.getSegundosDelDia()) / 60;
}

Hora Horario::finVentanaEntrada() const
{
   Hora resultado;
   horaInicio.sumarMinutos(VENTANA_MINUTOS, resultado);   // siempre cabe: inicio < fin <= 23:49
   return resultado;
}

Hora Horario::finVentanaSalida() const
{
   Hora resultado;
   horaFin.sumarMinutos(VENTANA_MINUTOS, resultado);      // validar() garantiza que cabe
   return resultado;
}

bool Horario::seSolapaCon(const Horario& otro) const
{
   // Mismo día y los intervalos (de inicio a fin, sin incluir el fin) se cruzan.
   // Las clases consecutivas (fin == inicio del otro) NO se solapan.
   return diaSemana == otro.diaSemana &&
          horaInicio < otro.horaFin &&
          otro.horaInicio < horaFin;
}

bool Horario::permiteEntrada(int dia, const Hora& hora) const
{
   // Extremos inclusivos: desde inicio hasta inicio + 10 min
   return activo && dia == diaSemana && horaInicio <= hora && hora <= finVentanaEntrada();
}

bool Horario::permiteSalida(int dia, const Hora& hora) const
{
   // Extremos inclusivos: desde fin hasta fin + 10 min
   return activo && dia == diaSemana && horaFin <= hora && hora <= finVentanaSalida();
}
