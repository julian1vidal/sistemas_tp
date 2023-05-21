#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <set>

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

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    //ListaAtomica<hashMapPair> *_claves; // Use la lista atomica para que sea una estructura atomica
    // Y porque de todas formas hay que recorrerla entera cada vez que se hace claves()
    std::atomic< std::set<std::string> > *_claves; //atomic set for claves
    std::atomic<unsigned int> *nro_operacion;
    std::mutex *mutexes[cantLetras];

    static unsigned int hashIndex(std::string clave);
};

#endif  /* HMC_HPP */
