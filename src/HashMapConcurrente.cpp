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
    //this->_claves = new ListaAtomica<hashMapPair>();
    //atomic set for claves

    (*nro_operacion).store(0);
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

    int nro_op;
    if (it.haySiguiente()) {
        it.siguiente().second++;
        nro_op = (*nro_operacion).fetch_add(1);
    }
    else {
        (*tabla)[indice].insertar({clave,1});
        //(*_claves).insertar({clave,nro_op = (*nro_operacion).fetch_add(1)}); // Alguno sabe si esta linea es atomica?
        (*_claves).load().insert(clave);
        //Importante que insertar en _claves vaya despues para mantener consistencia con claves()
    }

    (*mutexes)[indice].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    // // Completar (Ejercicio 2)
    // std::vector<std::string> res;

    // unsigned int nro_op = (*nro_operacion).fetch_add(1);
    // ListaAtomica<hashMapPair>::Iterador it = (*_claves).crearIt();

    // while(it.haySiguiente()) {
    //     if (it.siguiente().second<nro_op) {
    //         res.push_back(it.siguiente().first);
    //     }
    // }

    // return res;

    std:: set<std::string> claves = (*_claves).load();
    // Crear un std::vector vacío con el tamaño adecuado
    std::vector<std::string> res(claves.size());

    // Copiar los elementos del std::set al std::vector
    std::copy(claves.begin(), claves.end(), res.begin());

    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
}

#endif
