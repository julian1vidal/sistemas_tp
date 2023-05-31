#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        //std::cout << &tabla[i] << std::endl;

        // Si llegara a no estar siendo concurrente, sospecho que parte del problema podria ser como los inicializamos
        sem_init(&mutexes[i],0,1);
        semaforoOcupado.push_back(false);
    }
    this->thread_index = new std::atomic<unsigned int>();
    thread_index->store(0);
}

HashMapConcurrente::~HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        delete tabla[i];
    }
    delete this->thread_index;
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int indice = hashIndex(clave);

    int success = sem_wait(&mutexes[indice]);
    if (success != 0){
        std::cout << "ERROR EN EL SEM WAIT" << std::endl;
        return;
    }
    semaforoOcupado[indice] = true;

    ListaAtomica<hashMapPair>::Iterador it = tabla[indice]->crearIt();
    while(it.haySiguiente() && it.siguiente().first!=clave) {
        it.avanzar();
    }

    if (it.haySiguiente()) {
        it.siguiente().second++;
    }
    else {
        tabla[indice]->insertar({clave,1});
    }
    semaforoOcupado[indice] = false;
    sem_post(&mutexes[indice]);

}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res;

    for (unsigned int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();

        while (it.haySiguiente()){
            res.push_back(it.siguiente().first);
            it.avanzar();
        }
    }
    // Va a devolver al menos las claves que existian cuando se llamo la funcion.
    // Podria llegar a devolver mas de las que corresponde

    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
    //std::cout << "INICIO 0" << std::endl;

    ListaAtomica<hashMapPair>::Iterador it = tabla[hashIndex(clave)]->crearIt();
    //imprimirPorBucket();

    //std::cout << "INICIO 1" << std::endl;

    while(it.haySiguiente()){
        //std::cout << it.siguiente().second << std::endl;
        if (it.siguiente().first == clave){
            return it.siguiente().second;
        }
        it.avanzar();
        //std::cout << "POST AVANZAR" << std::endl;
    }
    // No estamos teniendo en cuenta el caso de si se agrega la clave durante la busqueda
    // Pero entendimos que no hacia falta.
    //std::cout << "FIN" << std::endl;

    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair max = hashMapPair(); // Esto viene de los profesores
    max.second = 0;
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&mutexes[i]);
        semaforoOcupado[i] = true;
    }
    // Estamos pidiendo todos a la vez y recien vamos a considerar que maximo comenzo cuando los haya
    // terminado de pedir a todos
    // No deberia haber race condition porque incrementar hace el post al final de su ejecucion
    // Por pedir los semaforos en orden tampoco deberia poder haber deadlock con otro maximo

    // Esta definicion es la que le mantiene la consistencia

    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {

        ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();
        while (it.haySiguiente()){
            if (it.siguiente().second > max.second){
                max.first = it.siguiente().first;
                max.second = it.siguiente().second;
            }
            it.avanzar();
        }
        semaforoOcupado[i] = false;
        sem_post(&mutexes[i]);
        // Puede pasar que justo un maximo se quede con el semaforo cuando este lo libere y tenga que esperar
        // A que este maximo termine de ejecutar (efectivamente funcionando como si no liberaramos nada)
        // Por el momento lo considero reglas del juego pero diria que es lo menos eficiente que tiene nuestro tp
    }

    return max;
}

void HashMapConcurrente::buscarMaximo(unsigned int id, std::vector<hashMapPair>* res){
    unsigned int bucket;
    hashMapPair parcial = hashMapPair();
    parcial.second = 0;

    while ((bucket = thread_index->fetch_add(1))<HashMapConcurrente::cantLetras){ // Confirmar que esto sea atomico
        ListaAtomica<hashMapPair>::Iterador it = tabla[bucket]->crearIt();

        while (it.haySiguiente()){

            if (it.siguiente().second > parcial.second){

                parcial.first = it.siguiente().first;
                parcial.second = it.siguiente().second;
            }
            it.avanzar();
        }

        (*res)[bucket] = parcial;
        semaforoOcupado[bucket] = false;
        sem_post(&mutexes[bucket]);
        // Lo libero aca para que los demas no tengan que esperar a que barramos toda la tabla
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
    if (cantThreads<=1){
        return HashMapConcurrente::maximo();
    }
    // Si queremos usar la funcion maximo para este caso es importante que vaya antes de pedir los semaforos
    // y que sea con un return para evitar posibles deadlocks
    
    unsigned int cThreads = cantThreads;
    
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&mutexes[i]);
        semaforoOcupado[i] = true;
    }
    // Los van devolviendo los threads para no tener que esperar a que esta funcion haga join

    thread_index->store(0); // Importante que vaya antes de pedir los semaforos porque la comparte con otras
    // ejecuciones de maximoParalelo. No estoy permitiendo que pueda haber multiples maximos paralelos corriendo
    // concurrentemente (si pueden ir pidiendo los mutex y "reservar" su turno)
    // Choca tambien con maximo comun

    if (cantThreads>26){ // Si me piden mas threads que buckets lo cappeo
        cThreads = 26;
    }

    std::vector<std::thread> threads(cThreads);
    std::vector<hashMapPair> res(HashMapConcurrente::cantLetras);

    for (unsigned int i = 0 ; i<cThreads ; i++){
        threads[i] = std::thread(&HashMapConcurrente::buscarMaximo, this, i, &res);
    }
    
    for (unsigned int i = 0 ; i< cThreads ; i++){
        threads[i].join();
    }

    unsigned int maximo = 0;
    unsigned int max_index = 0;
    for (unsigned int i = 0 ; i<res.size() ; i++){
        if (res[i].second > maximo){
            maximo = res[i].second;
            max_index = i;
        }
    }
    // Toma los 26 valores, 1 por bucket y se fija cual es el de mayor apariciones.
    // Se me hizo mas facil esta forma a que los threads lo vayan chequeando durante su ejecucion.

    return res[max_index];
}

void HashMapConcurrente::imprimirPorBucket(){

    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        std::cout << tabla[i]->longitud() << std::endl;
    }
}

#endif
