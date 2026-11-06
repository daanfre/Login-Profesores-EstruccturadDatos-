/***********************************************************************
 * Module:  ListaDoble.h
 * Author:  usuario
 * Modified: jueves, 5 de noviembre de 2026
 * Purpose: Declaración e implementación del template ListaDoble<T>
 *
 * Lista doblemente enlazada genérica implementada manualmente con
 * punteros y memoria dinámica.
 *
 * Invariantes (se cumplen al terminar cada método público):
 *   1. cantidad == 0  <=>  cabeza == nullptr  y  cola == nullptr
 *   2. cabeza->anterior == nullptr  y  cola->siguiente == nullptr
 *   3. para todo nodo x (salvo la cola): x->siguiente->anterior == x
 *   4. recorrer de cabeza a cola visita exactamente 'cantidad' nodos
 *
 * Propiedad de la memoria: la lista es DUEÑA de sus nodos.
 * Los crea con new y los libera con delete (composición).
 *
 * Cómo se recorre desde afuera (lógica de punteros):
 *   for (Nodo<T>* n = lista.getCabeza(); n != nullptr; n = n->getSiguiente())
 *      ... n->getDato() ...
 *   Y hacia atrás: desde getCola() avanzando con getAnterior().
 ***********************************************************************/

#ifndef LISTADOBLE_H
#define LISTADOBLE_H

#include <stdexcept>
#include <utility>
#include "Nodo.h"

template <typename T>
class ListaDoble
{
private:
   Nodo<T>* cabeza;
   Nodo<T>* cola;
   int cantidad;

   // Devuelve el nodo en 'pos' (0..cantidad-1) o nullptr si no existe.
   // Recorre desde el extremo más cercano: como máximo cantidad/2 pasos.
   Nodo<T>* nodoEnPosicion(int pos) const
   {
      if (pos < 0 || pos >= cantidad)
         return nullptr;

      Nodo<T>* actual;
      if (pos < cantidad / 2)
      {
         actual = cabeza;                       // primera mitad: desde la cabeza
         for (int i = 0; i < pos; i++)
            actual = actual->getSiguiente();
      }
      else
      {
         actual = cola;                         // segunda mitad: desde la cola
         for (int i = cantidad - 1; i > pos; i--)
            actual = actual->getAnterior();
      }
      return actual;
   }

   // Copia profunda: crea nodos NUEVOS con copias de los datos.
   // Precondición: esta lista está vacía.
   void copiarDesde(const ListaDoble<T>& otra)
   {
      for (Nodo<T>* n = otra.cabeza; n != nullptr; n = n->getSiguiente())
         insertarFinal(n->getDato());
   }

public:
   // ---------------- Constructores, destructor y asignación -------------

   ListaDoble() : cabeza(nullptr), cola(nullptr), cantidad(0) {}

   // Constructor de copia (copia profunda).
   // Si falla a la mitad (por ejemplo, sin memoria), libera lo que ya copió
   // y relanza la excepción: no quedan nodos perdidos.
   ListaDoble(const ListaDoble<T>& otra) : cabeza(nullptr), cola(nullptr), cantidad(0)
   {
      try
      {
         copiarDesde(otra);
      }
      catch (...)
      {
         vaciar();
         throw;
      }
   }

   // Asignación por copia (copia profunda, protegida contra a = a).
   // Técnica "copiar y luego intercambiar": primero se crea la copia completa;
   // solo si salió bien se intercambia con esta lista. La lista vieja se libera
   // sola cuando 'copia' sale de alcance (su destructor llama a vaciar()).
   ListaDoble<T>& operator=(const ListaDoble<T>& otra)
   {
      if (this != &otra)
      {
         ListaDoble<T> copia(otra);
         std::swap(cabeza, copia.cabeza);
         std::swap(cola, copia.cola);
         std::swap(cantidad, copia.cantidad);
      }
      return *this;
   }

   // Constructor de movimiento: "roba" los nodos y deja 'otra' vacía.
   ListaDoble(ListaDoble<T>&& otra) noexcept
      : cabeza(otra.cabeza), cola(otra.cola), cantidad(otra.cantidad)
   {
      otra.cabeza = nullptr;
      otra.cola = nullptr;
      otra.cantidad = 0;
   }

   // Asignación por movimiento
   ListaDoble<T>& operator=(ListaDoble<T>&& otra) noexcept
   {
      if (this != &otra)
      {
         vaciar();
         cabeza = otra.cabeza;
         cola = otra.cola;
         cantidad = otra.cantidad;
         otra.cabeza = nullptr;
         otra.cola = nullptr;
         otra.cantidad = 0;
      }
      return *this;
   }

   ~ListaDoble()
   {
      vaciar();
   }

   // ---------------- Inserción ------------------------------------------

   // O(1)
   void insertarInicio(const T& dato)
   {
      Nodo<T>* nuevo = new Nodo<T>(dato);
      if (cabeza == nullptr)
      {
         cabeza = cola = nuevo;
      }
      else
      {
         nuevo->setSiguiente(cabeza);
         cabeza->setAnterior(nuevo);
         cabeza = nuevo;
      }
      cantidad++;
   }

   // O(1) gracias al puntero cola
   void insertarFinal(const T& dato)
   {
      Nodo<T>* nuevo = new Nodo<T>(dato);
      if (cola == nullptr)
      {
         cabeza = cola = nuevo;
      }
      else
      {
         nuevo->setAnterior(cola);
         cola->setSiguiente(nuevo);
         cola = nuevo;
      }
      cantidad++;
   }

   // Posiciones válidas: 0..cantidad (cantidad = insertar al final). O(n)
   bool insertarEnPosicion(int pos, const T& dato)
   {
      if (pos < 0 || pos > cantidad)
         return false;
      if (pos == 0)        { insertarInicio(dato); return true; }
      if (pos == cantidad) { insertarFinal(dato);  return true; }

      Nodo<T>* actual = nodoEnPosicion(pos);      // el nuevo va ANTES de este
      Nodo<T>* ant = actual->getAnterior();
      Nodo<T>* nuevo = new Nodo<T>(dato);

      nuevo->setAnterior(ant);
      nuevo->setSiguiente(actual);
      ant->setSiguiente(nuevo);
      actual->setAnterior(nuevo);
      cantidad++;
      return true;
   }

   // ---------------- Eliminación ----------------------------------------

   // Desenlaza y libera un nodo de ESTA lista. O(1).
   // Se usa después de encontrar el nodo recorriendo con punteros.
   // Precondición: 'nodo' pertenece a esta lista (lo obtuvo de getCabeza/getCola).
   void eliminarNodo(Nodo<T>* nodo)
   {
      if (nodo == nullptr)
         return;
      Nodo<T>* ant = nodo->getAnterior();
      Nodo<T>* sig = nodo->getSiguiente();

      if (ant != nullptr) ant->setSiguiente(sig);
      else                cabeza = sig;          // era la cabeza

      if (sig != nullptr) sig->setAnterior(ant);
      else                cola = ant;            // era la cola

      delete nodo;
      cantidad--;
   }

   // O(n) para llegar a la posición; el desenlace es O(1)
   bool eliminarEnPosicion(int pos)
   {
      Nodo<T>* nodo = nodoEnPosicion(pos);
      if (nodo == nullptr)
         return false;
      eliminarNodo(nodo);
      return true;
   }

   // Libera todos los nodos. O(n)
   void vaciar()
   {
      Nodo<T>* actual = cabeza;
      while (actual != nullptr)
      {
         Nodo<T>* sig = actual->getSiguiente();   // guardar ANTES de liberar
         delete actual;
         actual = sig;
      }
      cabeza = cola = nullptr;
      cantidad = 0;
   }

   // ---------------- Acceso ---------------------------------------------

   // Primer y último nodo: punto de partida de los recorridos con punteros.
   Nodo<T>* getCabeza() const { return cabeza; }
   Nodo<T>* getCola() const   { return cola; }

   // Acceso por posición. Lanza std::out_of_range si la posición no existe.
   T& obtener(int pos)
   {
      Nodo<T>* nodo = nodoEnPosicion(pos);
      if (nodo == nullptr)
         throw std::out_of_range("ListaDoble::obtener: posicion fuera de rango");
      return nodo->getDato();
   }

   const T& obtener(int pos) const
   {
      Nodo<T>* nodo = nodoEnPosicion(pos);
      if (nodo == nullptr)
         throw std::out_of_range("ListaDoble::obtener: posicion fuera de rango");
      return nodo->getDato();
   }

   // ---------------- Consultas ------------------------------------------

   bool estaVacia() const { return cantidad == 0; }
   int getCantidad() const { return cantidad; }

   // Verifica las 4 invariantes. Se usa en las pruebas. O(n)
   bool esConsistente() const
   {
      if (cantidad == 0)
         return cabeza == nullptr && cola == nullptr;
      if (cabeza == nullptr || cola == nullptr) return false;
      if (cabeza->getAnterior() != nullptr || cola->getSiguiente() != nullptr) return false;

      int adelante = 0;
      for (Nodo<T>* n = cabeza; n != nullptr; n = n->getSiguiente())
      {
         if (n->getSiguiente() != nullptr && n->getSiguiente()->getAnterior() != n)
            return false;
         adelante++;
         if (adelante > cantidad) return false;   // evita ciclo infinito si hay un bucle
      }
      int atras = 0;
      for (Nodo<T>* n = cola; n != nullptr; n = n->getAnterior())
      {
         atras++;
         if (atras > cantidad) return false;
      }
      return adelante == cantidad && atras == cantidad;
   }
};

#endif
