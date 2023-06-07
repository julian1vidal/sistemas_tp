#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <set>
#include <semaphore.h>
#include <thread>
#include <unistd.h>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;
typedef std::pair<std::string, unsigned int> claveAndOp;

class HashMapConcurrente {
 public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();
    ~HashMapConcurrente();

    void incrementarLocks(std::string clave);
    void incrementar(std::string clave);
    std::vector<std::string> clavesLocks();
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);
    unsigned int valorInicio(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

    void imprimirPorBucket(); // Funcion que agregue por motivos de debugging

    // Seccion tests

    std::vector<std::string> clavesLocksTestConLocks(std::vector<std::string> *res, bool *flag);
    std::vector<std::string> clavesLocksTestSinLocks(std::vector<std::string> *res, bool *flag);
    std::vector<std::string> clavesTest(std::vector<std::string> *res, bool *flag);
    unsigned int valorTest(std::string clave, unsigned int *res, bool *flag, bool naptime);
    unsigned int valorInicioTest(std::string clave, unsigned int *res, bool *flag);
    hashMapPair maximoTest(hashMapPair *res, bool *flag);
    void buscarMaximoTest(unsigned int id, std::vector<hashMapPair> *res, bool *flag);
    hashMapPair maximoParaleloTest(hashMapPair *res, unsigned int cantThreads, bool *flag);

    std::vector<bool> semaforoOcupado;

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    std::set<claveAndOp> _claves;

    // Herramientas de sincronizacion
    sem_t muteMax[HashMapConcurrente::cantLetras]; // Use semaforos porque no me salia usar mutex con c++
    sem_t muteCla[HashMapConcurrente::cantLetras];
    sem_t muteVal[HashMapConcurrente::cantLetras];
    //unsigned int cantClaves[HashMapConcurrente::cantLetras];
    //unsigned int cantMaximos[HashMapConcurrente::cantLetras];
    std::atomic<unsigned int> *thread_index; // Para maximo concurrente
    unsigned int nro_operacion;
    sem_t mute_nro_op;

    static unsigned int hashIndex(std::string clave);
    void buscarMaximo(unsigned int id, std::vector<hashMapPair>* res);
};

#endif  /* HMC_HPP */
