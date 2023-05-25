#ifndef HMC_ARCHIVOS_HPP
#define HMC_ARCHIVOS_HPP

#include <ctype.h>
#include <vector>
#include <string>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
);

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
);

// Funcion auxiliar usada por cargarMultiplesArchivos
int cargarArchivoParalelo(
    std::atomic<unsigned int> *thread_index,
    std::vector<std::string> *filePaths,
    HashMapConcurrente *hashMap
);

#endif /* HMC_ARCHIVOS_HPP */
