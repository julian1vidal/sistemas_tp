#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "HashMapConcurrente.hpp"



void incrementarPalabra(int threadId, int cantidad, std::string palabra, HashMapConcurrente* miHashMap) {
    for (int i = 0; i < cantidad; i++) {
        (*miHashMap).incrementar(palabra);
    }
    
}

bool testIncrementar() {
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    std::thread t1(incrementarPalabra, 1, 10000, "ejemplo", miHashMap);
    std::thread t2(incrementarPalabra, 2, 10000, "ejemplo", miHashMap);

    t1.join();
    t2.join();

    unsigned int resultado = (*miHashMap).valor("ejemplo");
    delete miHashMap;
    return resultado == 20000;
}

bool testIncrementarDiferentesPalabras(){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    std::thread t1(incrementarPalabra, 1, 10000, "ejemplo", miHashMap);
    std::thread t2(incrementarPalabra, 2, 10000, "otroEjemplo", miHashMap);

    t1.join();
    t2.join();

    unsigned int resultado1 = (*miHashMap).valor("ejemplo");
    unsigned int resultado2 = (*miHashMap).valor("otroEjemplo");

    delete miHashMap;
    return resultado1 == 10000 && resultado2 == 10000;
}

hashMapPair maximo(HashMapConcurrente* miHashMap){
    return (*miHashMap).maximo();
}

void incrementarFork(int threadId, int cantidad, std::string palabra, HashMapConcurrente* miHashMap) {
    int pid = fork();
    if (pid == 0) {
        for (int i = 0; i < cantidad; i++) {
            (*miHashMap).incrementar(palabra);
        }
        std::cout << "Sali" << std::endl;
        exit(0);
    }
    else{
        while (!(*miHashMap).semaforoOcupado[5]){
            std::cout << "iterando" << std::endl;
        }
        std::cout << "Entre y me duermo" << std::endl;
        kill(pid, SIGSTOP);
        sleep(5);
        kill(pid, SIGCONT);
    }

}

bool testMaximoConcurrencia(){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    std::thread t1(incrementarPalabra, 1, 5, "ejemplo", miHashMap);
    std::thread t2(incrementarPalabra, 2, 3, "otroEjemplo", miHashMap);
    std::thread t3(incrementarPalabra, 3, 4, "river", miHashMap);

    t1.join();
    t2.join();
    t3.join();

    hashMapPair resultado = (*miHashMap).maximo();
    bool antes = resultado.first == "ejemplo" && resultado.second == 5;
    std::thread t4(incrementarFork, 4, 100, "ejemplo", miHashMap);
    std::thread t5(maximo, miHashMap);
    std::thread t6(incrementarPalabra, 6, 500, "river", miHashMap);
    t4.join();
    t6.join();
    t5.join();
    resultado = (*miHashMap).maximo();
    bool despues = resultado.first == "river" && resultado.second == 504;
    delete miHashMap;
    return antes && despues;
}

std::string result_to_string(bool result) {
    return result ? "OK" : "ERROR";
}

int main(){
    std::cout << "testIncrementar: Corriendo\n" 
        << result_to_string(testIncrementar()) << std::endl;
    std::cout << "testIncrementarDiferentesPalabras: Corriendo\n"
        << result_to_string(testIncrementarDiferentesPalabras()) << std::endl;
    std::cout << "testMaximoConcurrencia: Corriendo\n"
        << result_to_string(testMaximoConcurrencia()) << std::endl;
}