/***********************************************************************
 * Module:  Nodo.h
 * Author:  usuario
 * Modified: lunes, 2 de noviembre de 2026
 * Purpose: Declaración e implementación del template Nodo<T>
 *
 * Nota: al ser un template, TODA la implementación va en el .h.
 *       El compilador necesita ver el código para generar Nodo<int>,
 *       Nodo<Profesor>, etc. en el lugar donde se usan.
 ***********************************************************************/

#ifndef NODO_H
#define NODO_H

template <typename T>
class Nodo
{
private:
   T dato;               // valor almacenado (copia propia)
   Nodo<T>* anterior;    // nodo previo  (nullptr si es la cabeza)
   Nodo<T>* siguiente;   // nodo posterior (nullptr si es la cola)

public:
   explicit Nodo(const T& valor)
      : dato(valor), anterior(nullptr), siguiente(nullptr)
   {
   }

   // Un nodo NO se copia: copiar sus punteros crearía dos nodos
   // enlazados a los mismos vecinos. Solo ListaDoble crea nodos.
   Nodo(const Nodo<T>&) = delete;
   Nodo<T>& operator=(const Nodo<T>&) = delete;

   // El destructor por defecto es suficiente: el nodo NO libera a sus
   // vecinos (eso lo hace ListaDoble), así se evita la liberación en cascada.
   ~Nodo() = default;

   T& getDato() { return dato; }               // permite modificar el dato
   const T& getDato() const { return dato; }   // solo lectura

   Nodo<T>* getAnterior() const { return anterior; }
   void setAnterior(Nodo<T>* nuevo) { anterior = nuevo; }

   Nodo<T>* getSiguiente() const { return siguiente; }
   void setSiguiente(Nodo<T>* nuevo) { siguiente = nuevo; }
};

#endif
