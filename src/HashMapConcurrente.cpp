#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        this->tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int indice = hashIndex(clave);

    (*mutexes)[indice].lock();

    ListaAtomica<hashMapPair>::Iterador it = (*tabla)[indice].crearIt();
    while(it.haySiguiente() && it.siguiente().first!=clave) {
        it.avanzar();
    }

    if (it.haySiguiente()) {
        it.siguiente().second++;
    }
    else {
        (*tabla)[indice].insertar({clave,1});
    }

    (*mutexes)[indice].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res;

    for (int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = (*tabla)[i].crearIt();

        while (it.haySiguiente()){
            res.push_back(it.siguiente().first);
        }
    }
    // Va a devolver al menos las claves que existian cuando se llamo la funcion.

    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)

    ListaAtomica<hashMapPair>::Iterador it = (*tabla)[hashIndex(clave)].crearIt();

    while(it.haySiguiente()){
        if (it.siguiente().first == clave){
            return it.siguiente().second;
        }
    }
    // No estamos teniendo en cuenta el caso de si se agrega la clave durante la busqueda
    // Pero entendimos que no hacia falta.

    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        (*mutexes)[i].lock();
        for (
            ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        (*mutexes)[i].unlock();
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
}

#endif
