#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "HashMapConcurrente.hpp"

using namespace std;

// void testMaximoThreads(){
//     HashMapConcurrente hM;    
//     hashMapPair max;
//     std::vector<std::thread> threads;


//     thread t1([&hM]() { hM->incrementar("juli"); });
//     threads.emplace_back([&hM]() { hM->incrementar("juli"); });
//     threads.emplace_back([&hM, &max]() { max = hM->maximo(); });
//     threads.emplace_back([&hM]() { hM->incrementar("juli"); });
//     threads.emplace_back([&hM]() { hM->incrementar("juli"); });
//     threads.emplace_back([&hM]() { hM->incrementar("juli"); });

//     // Realizar join en todos los hilos
//     for (auto& thread : threads) {
//         thread.join();
//     }

//     unsigned int resultado = hM->valor("juli");
//     cout << "Valor: " << resultado << endl;

//     //cout << "MAXIMO -> " << max.first << ": " << max.second << endl;
// }

void incrementarPalabra(int threadId, int cantidad, std::string palabra, HashMapConcurrente* miHashMap) {
    for (int i = 0; i < cantidad; i++) {
        (*miHashMap).incrementar(palabra);
    }   
}

void testMaximoThreads(){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    
    thread t1(incrementarPalabra, 1, 1, "juli", miHashMap);

    t1.join();

    unsigned int resultado = (*miHashMap).valor("juli");
    cout << "Valor: " << resultado << endl;

    delete miHashMap;
}



// Test basico para maximo y maximoParalelo
bool testMaximosSecuencial(){
    HashMapConcurrente* hM = new HashMapConcurrente();
    hM->incrementar("juli");

    hashMapPair res_antes = hM->maximo();
    hashMapPair res_antes_paralelo = hM->maximoParalelo(hM->cantLetras);

    for (int i = 0; i < 5; i++){
        hM->incrementar("juli");
    }

    hashMapPair res_despues = hM->maximo();
    hashMapPair res_despues_paralelo = hM->maximoParalelo(hM->cantLetras);

    bool max_antes = res_antes.first == "juli" && res_antes.second == 1;
    bool max_despues = res_despues.first == "juli" && res_despues.second == 6;
    bool max_antes_paralelo = res_antes_paralelo.first == "juli" && res_antes_paralelo.second == 1;
    bool max_despues_paralelo = res_despues_paralelo.first == "juli" && res_despues_paralelo.second == 6;

    delete hM;

    return (max_antes && max_despues && max_antes_paralelo && max_despues_paralelo);

    
}

string result_to_string(bool result) {
    return result ? "OK" : "ERROR";
}

int main(){
    cout << "testMaximosSecuencial: Corriendo\n"
        << result_to_string(testMaximosSecuencial()) << endl;
    cout << "testMaximoThreads: Corriendo\n" << endl;
    testMaximoThreads();
    
    
}