#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"


// VERSIONES FUNCIONES VIEJAS


HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        //cantClaves[i] = 0;
        //cantMaximos[i] = 0;

        sem_init(&muteMax[i],0,1);
        sem_init(&muteCla[i],0,1);
        sem_init(&muteVal[i],0,1);
        semaforoOcupado.push_back(false);
    }
    nro_operacion = 0; // Nunca deberia una operacion poder tener el op 0, empiezan desde el 1
    this->thread_index = new std::atomic<unsigned int>();
    thread_index->store(0);
    sem_init(&mute_nro_op,0,1);
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

/*void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int indice = hashIndex(clave);

    int success = sem_wait(&muteMax[indice]);
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
    sem_post(&muteMax[indice]);

}*/

/*std::vector<std::string> HashMapConcurrente::claves() {
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
}*/

/*unsigned int HashMapConcurrente::valor(std::string clave) {
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
}*/


// VERSIONES FUNCIONES NUEVAS


void HashMapConcurrente::incrementarLocks(std::string clave) {
    unsigned int indice = hashIndex(clave);

    int success;
    success = sem_wait(&muteMax[indice]);
    success+= sem_wait(&muteCla[indice]);
    success+= sem_wait(&muteVal[indice]);

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

    sem_post(&muteVal[indice]);
    sem_post(&muteCla[indice]);
    sem_post(&muteMax[indice]);

}

// Usando nro_operacion
void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int indice = hashIndex(clave);

    int success;
    success = sem_wait(&muteMax[indice]);
    success+= sem_wait(&muteVal[indice]);

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
        sem_wait(&mute_nro_op);
        tabla[indice]->insertar({clave,1}); // Linea original
        nro_operacion++;
        _claves.insert({clave,nro_operacion});
        sem_post(&mute_nro_op);
    }

    semaforoOcupado[indice] = false;

    sem_post(&muteVal[indice]);
    sem_post(&muteMax[indice]);

}

std::vector<std::string> HashMapConcurrente::clavesLocks() {
    std::vector<std::string> res;

    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteCla[i]);
    }

    for (unsigned int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();

        while (it.haySiguiente()){
            res.push_back(it.siguiente().first);
            it.avanzar();
        }
        sem_post(&muteCla[i]);
    }

    return res;
}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> res;
    sem_wait(&mute_nro_op);
    nro_operacion++;
    unsigned int op = nro_operacion;
    sem_post(&mute_nro_op);

    // Me pregunto si esto puede tener race conditions, como que se saltee elementos o algo asi
    for (claveAndOp e : _claves){
        if (e.second < op){
            res.push_back(e.first);
        }
    }

    return res;
}

// Bloqueamos el bucket solo para tomar el valor que corresponda
// Si empezamos a buscar y nos agregan en el medio la palabra que buscamos por primera vez
// consideramos que tenemos que devolver 0 porque se llamo a valor antes
unsigned int HashMapConcurrente::valor(std::string clave) {
    unsigned int index = hashIndex(clave);
    ListaAtomica<hashMapPair>::Iterador it = tabla[index]->crearIt();

    while(it.haySiguiente()){
        if (it.siguiente().first == clave){
            sem_wait(&muteVal[index]);
            unsigned int valor = it.siguiente().second;
            sem_post(&muteVal[index]);
            return valor;
        }
        it.avanzar();
    }

    return 0;
}

// Bloqueamos el bucket al inicio de todo
unsigned int HashMapConcurrente::valorInicio(std::string clave) {
    unsigned int index = hashIndex(clave);

    sem_wait(&muteVal[index]);
    ListaAtomica<hashMapPair>::Iterador it = tabla[index]->crearIt();

    while(it.haySiguiente()){
        if (it.siguiente().first == clave){
            sem_post(&muteVal[index]);
            return it.siguiente().second;
        }
        it.avanzar();
    }
    sem_post(&muteVal[index]);

    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair max = hashMapPair(); // Esto viene de los profesores
    max.second = 0;
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteMax[i]);
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
        sem_post(&muteMax[i]);
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
        sem_post(&muteMax[bucket]);
        // Lo libero aca para que los demas no tengan que esperar a que barramos toda la tabla
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    if (cantThreads<=1){
        return HashMapConcurrente::maximo();
    }
    // Si queremos usar la funcion maximo para este caso es importante que vaya antes de pedir los semaforos
    // y que sea con un return para evitar posibles deadlocks
    
    unsigned int cThreads = cantThreads;
    
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteMax[i]);
        semaforoOcupado[i] = true;
    }
    // Los van devolviendo los threads para no tener que esperar a que esta funcion haga join

    thread_index->store(0); // Importante que vaya antes de pedir los semaforos porque la comparte con otras
    // ejecuciones de maximoParalelo. No estoy permitiendo que pueda haber multiples maximos paralelos corriendo
    // concurrentemente (si pueden ir pidiendo los mutex y "reservar" su turno)
    // Choca tambien con maximo comun

    if (cantThreads>HashMapConcurrente::cantLetras){ // Si me piden mas threads que buckets lo cappeo
        cThreads = HashMapConcurrente::cantLetras;
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


// SECCION TESTS


std::vector<std::string> HashMapConcurrente::clavesLocksTestConLocks(std::vector<std::string> *res, bool *flag){
    std::vector<std::string> aux;

    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteCla[i]);
    }

    *flag = false;
    sleep(1);

    for (unsigned int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();

        while (it.haySiguiente()){
            aux.push_back(it.siguiente().first);
            it.avanzar();
        }
        if (i==25){
            *flag = !sem_post(&muteCla[i]);
        }
        else{
            sem_post(&muteCla[i]);    
        }
    }

    *res = aux;
    return aux;
}

std::vector<std::string> HashMapConcurrente::clavesLocksTestSinLocks(std::vector<std::string> *res, bool *flag){
    std::vector<std::string> aux;

    *flag = false;
    sleep(1);

    for (unsigned int i = 0 ; i < HashMapConcurrente::cantLetras ; i++){
        ListaAtomica<hashMapPair>::Iterador it = tabla[i]->crearIt();

        while (it.haySiguiente()){
            aux.push_back(it.siguiente().first);
            it.avanzar();
        }
    }

    *flag = true;
    *res = aux;
    return aux;
}

std::vector<std::string> HashMapConcurrente::clavesTest(std::vector<std::string> *res, bool *flag){

    std::vector<std::string> aux;
    sem_wait(&mute_nro_op);
    nro_operacion++;
    unsigned int op = nro_operacion;
    sem_post(&mute_nro_op);

    *flag = false;
    sleep(1);

    // Me pregunto si esto puede tener race conditions, como que se saltee elementos o algo asi
    for (claveAndOp e : _claves){
        if (e.second < op){
            aux.push_back(e.first);
        }
    }

    *flag = true;
    *res = aux;

    return aux;
}



unsigned int HashMapConcurrente::valorTest(std::string clave, unsigned int *res, bool *flag, bool naptime){
    unsigned int index = hashIndex(clave);
    ListaAtomica<hashMapPair>::Iterador it = tabla[index]->crearIt();

    while(it.haySiguiente()){

        if (it.siguiente().first == clave){

            sem_wait(&muteVal[index]);
            if (!naptime){
                *flag = false;
                sleep(1);
                // Para testear que no se le metan incrementar en el medio
            }
            unsigned int valor = it.siguiente().second;
            sem_post(&muteVal[index]);
            if (!naptime){
                *flag = true;
            }
            *res = valor;
            return valor;
        }
        it.avanzar();
        if (naptime){
            *flag = false;
            sleep(1);
            // Para testear que realmente de 0 al agregar la palabra que buscamos despues de empezar
        }
    }

    *flag = false;
    sleep(1);
    *flag = true;
    *res = 0;

    return 0;
}

unsigned int HashMapConcurrente::valorInicioTest(std::string clave, unsigned int *res, bool *flag){
    unsigned int index = hashIndex(clave);

    sem_wait(&muteVal[index]);
    *flag = false;
    sleep(1);
    ListaAtomica<hashMapPair>::Iterador it = tabla[index]->crearIt();

    while(it.haySiguiente()){
        if (it.siguiente().first == clave){
            *res = it.siguiente().second;
            *flag = true;
            sem_post(&muteVal[index]);
            return it.siguiente().second;
        }
        it.avanzar();
    }
    *flag = !sem_post(&muteVal[index]);

    *res = 0;

    return 0;
}

hashMapPair HashMapConcurrente::maximoTest(hashMapPair *res, bool *flag){
    hashMapPair max = hashMapPair();
    max.second = 0;
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteMax[i]);
        semaforoOcupado[i] = true;
    }
    *flag = false;
    sleep(1);

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
        if (i==25){
            *flag = !sem_post(&muteMax[i]); // Para hacerlo lo mas instantaneo posible
            // Dependiendo de como funciona c++, puede que haya una minuscula ventana donde
            // puedan bloquearme justo despues de levantar el semaforo pero antes de la asignacion
            // que al hilo corran los incrementar del main mientras estoy bloqueado
            // y que la comparacion del flag me de mal
        }
        else{
            sem_post(&muteMax[i]);
        }
    }

    *res = max;

    return max;
}

void HashMapConcurrente::buscarMaximoTest(unsigned int id, std::vector<hashMapPair>* res, bool *flag){
    unsigned int bucket;
    hashMapPair parcial = hashMapPair();
    parcial.second = 0;

    while ((bucket = thread_index->fetch_add(1))<HashMapConcurrente::cantLetras){ // Confirmar que esto sea atomico
        if (bucket==1){
            sleep(2);
        }
        
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
        if (bucket==1){ // El de la "b" esta siendo artificialmente forzado a ser el ultimo
            *flag = !sem_post(&muteMax[bucket]);
        }
        else{
            sem_post(&muteMax[bucket]);
        }
        // Lo libero aca para que los demas no tengan que esperar a que barramos toda la tabla
    }
}

hashMapPair HashMapConcurrente::maximoParaleloTest(hashMapPair *refRes, unsigned int cantThreads, bool *flag){
    if (cantThreads<=1){
        return HashMapConcurrente::maximo();
    }
    
    unsigned int cThreads = cantThreads;
    
    for (unsigned int i = 0 ; i<HashMapConcurrente::cantLetras ; i++){
        sem_wait(&muteMax[i]);
        semaforoOcupado[i] = true;
    }

    *flag = false;
    sleep(1);

    thread_index->store(0);

    if (cantThreads>HashMapConcurrente::cantLetras){
        cThreads = HashMapConcurrente::cantLetras;
    }

    std::vector<std::thread> threads(cThreads);
    std::vector<hashMapPair> res(HashMapConcurrente::cantLetras);

    for (unsigned int i = 0 ; i<cThreads ; i++){
        threads[i] = std::thread(&HashMapConcurrente::buscarMaximoTest, this, i, &res, flag);
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
    sleep(1);
    *refRes = res[max_index];

    return res[max_index];
}

#endif
