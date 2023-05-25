#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>

#include "CargarArchivos.hpp"

int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath) {

    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual); // Solo agregue esta linea
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

// No entendi porque cargarArchivo es int, pero cargarMultiplesArchivos es void
// No se cuando es que usariamos la cantidad de palabras cargadas pero para mantener la idea lo calcula
int cargarArchivoParalelo(std::atomic<unsigned int> *thread_index, std::vector<std::string> *filePaths,
                          HashMapConcurrente *hashMap){
    
    unsigned int file_index;
    int cant = 0;

    while ((file_index = thread_index->fetch_add(1)) < filePaths->size()){ // Importantes los parentesis
        int cant_este_archivo = cargarArchivo(*hashMap, (*filePaths)[file_index]);
        cant = cant + cant_este_archivo;
    }
    return cant;
}

void cargarMultiplesArchivos(HashMapConcurrente &hashMap,
                             unsigned int cantThreads, std::vector<std::string> filePaths) {
    // Completar (Ejercicio 4)
    unsigned int c_threads = cantThreads;

    if (cantThreads>filePaths.size()){ // Cappeo a la cantidad de archivos
        c_threads = filePaths.size();
    }

    std::atomic<unsigned int> thread_index;
    thread_index.store(0);

    std::vector<std::thread> threads(c_threads);

    for (unsigned int i = 0 ; i<c_threads ; i++){
        threads[i] = std::thread(cargarArchivoParalelo, &thread_index, &filePaths, &hashMap);
    }

    for (unsigned int i = 0 ; i<c_threads ; i++){
        threads[i].join();
    }
}

#endif
