#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <set>
#include <semaphore.h>
#include <thread>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

    void imprimirPorBucket(); // Funcion que agregue por motivos de debugging

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

    //std::mutex (*mutexes)[HashMapConcurrente::cantLetras];
    sem_t mutexes[HashMapConcurrente::cantLetras]; // Use semaforos porque no me salia usar mutex con c++

    static unsigned int hashIndex(std::string clave);
    std::atomic<unsigned int> thread_index; // Para maximo concurrente
    void buscarMaximo(unsigned int id, std::vector<hashMapPair>* res);
};

#endif  /* HMC_HPP */
