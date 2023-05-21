#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        this->tabla[i] = new ListaAtomica<hashMapPair>();
        //this->mutexes[i] = new std::mutex;
        sem_t *aux = &mutexes[i];
        sem_init(aux,0,1);
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int indice = hashIndex(clave);

    sem_wait(&mutexes[indice]);

    ListaAtomica<hashMapPair>::Iterador it = (*tabla)[indice].crearIt();

    int i = 0;
    while(it.haySiguiente() && it.siguiente().first!=clave) {
        it.avanzar();
    }

    if (it.haySiguiente()) {
        it.siguiente().second++;
    }
    else {
        (*tabla)[indice].insertar({clave,1});
    }

    sem_post(&mutexes[indice]);
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res;

    for (int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = (*tabla)[i].crearIt();

        while (it.haySiguiente()){
            res.push_back(it.siguiente().first);
            it.avanzar();
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
        it.avanzar();
    }
    // No estamos teniendo en cuenta el caso de si se agrega la clave durante la busqueda
    // Pero entendimos que no hacia falta.

    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {

        ListaAtomica<hashMapPair>::Iterador it = (*tabla)[index].crearIt();

        while (it.haySiguiente()){

            if (it.siguiente().second > max->second){
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
            it.avanzar();
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
}

void HashMapConcurrente::imprimirPorBucket(){
    for (int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        std::cout << (*tabla)[i].longitud() << std::endl;
    }
}

#endif
