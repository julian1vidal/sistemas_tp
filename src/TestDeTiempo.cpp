#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "HashMapConcurrente.hpp"
#include <chrono>
#include <random>
#include <string>
#include <vector>

std::string generateRandomWord(char startLetter) {
    // Generador de números aleatorios (instancia única)
    static std::random_device rd;
    static std::mt19937 rng(rd());

    // Distribución de letras del alfabeto
    std::uniform_int_distribution<> dist(0, 25);

    // Calcular el código ASCII de la letra inicial
    int startCode = (startLetter >= 'A' && startLetter <= 'Z') ? (startLetter - 'A') : (startLetter - 'a');

    // Generar palabra aleatoria
    std::string word;
    word += startLetter; // Agregar la letra inicial

    // Generar letras aleatorias adicionales
    for (int i = 1; i < 10; i++) {
        int letterCode = (startCode + dist(rng)) % 26;
        char letter = static_cast<char>(letterCode + ((startLetter >= 'A' && startLetter <= 'Z') ? 'A' : 'a'));
        word += letter;
    }

    return word;
}

std::vector<char> letras = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
                       'o','p','q','r','s','t','u','v','w','x','y','z'};

void maximoParaleloContraMaximoTodasLasLetras(int cantidad){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    for (int j=0; j<25; j++){
        for (int i = 0; i < cantidad; i++) {
            std::string palabra = generateRandomWord(letras[j]);
            (*miHashMap).incrementar(palabra);
        }
    }
    auto start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo = (*miHashMap).maximoParalelo(26);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Tiempo maximo paralelo: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo2 = (*miHashMap).maximo();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Tiempo maximo: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
    delete miHashMap;
}

void maximoParaleloContraMaximoUnaLetra(int cantidad, char letra){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    for (int i = 0; i < cantidad; i++) {
        std::string palabra = generateRandomWord(letra);
        (*miHashMap).incrementar(palabra);
    }
    auto start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo = (*miHashMap).maximoParalelo(26);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Tiempo maximo paralelo: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo2 = (*miHashMap).maximo();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Tiempo maximo: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << std::endl;
    delete miHashMap;
}

void buscarValorFinal(HashMapConcurrente *hm, bool *flag, bool valor){
    if (valor){
        while(*flag){}
        unsigned int val = hm->valorInicio("aaa");
    }
    else{
        while(*flag){}
        unsigned int val = hm->valor("aaa");
    }
}

void experimentosValor(unsigned int cantPalabras,unsigned int cantValor){
    auto total = std::chrono::high_resolution_clock::now();

    HashMapConcurrente hashM;
    hashM.incrementar("aaa");
    for (unsigned int i = 0 ; i<cantPalabras ; i++){
        std::string a = "a";
        std::string palabra = generateRandomWord('a');
        hashM.incrementar(a+palabra);
    }
    std::cout << "Termine de incrementar" << std::endl;
    bool flag = true;

    // Chequeo de tiempos sin incrementar en el medio


    std::vector<std::thread> threadsValor(cantValor);
    for (unsigned int i = 0 ; i<cantValor ; i++){
        threadsValor[i] = std::thread(buscarValorFinal, &hashM, &flag, false);
    }

    auto start = std::chrono::high_resolution_clock::now();
    sleep(2);
    flag = false;

    for (unsigned int i = 0 ; i<cantValor ; i++){
        threadsValor[i].join();
    }
    auto end = std::chrono::high_resolution_clock::now();

    flag = true;
    std::cout << "Tiempo valor = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " nanosegundos" << std::endl;

    std::vector<std::thread> threadsValorInicio(cantValor);
    for (unsigned int i = 0 ; i<cantValor ; i++){
        threadsValorInicio[i] = std::thread(buscarValorFinal, &hashM, &flag, true);
    }

    auto start2 = std::chrono::high_resolution_clock::now();
    sleep(2);
    flag = false;

    for (unsigned int i = 0 ; i<cantValor ; i++){
        threadsValorInicio[i].join();
    }
    auto end2 = std::chrono::high_resolution_clock::now();

    flag = true;
    std::cout << "Tiempo valor Inicio = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end2-start2).count() << " nanosegundos" << std::endl;

    std::cout << "La diferencia es de " << std::chrono::duration_cast<std::chrono::nanoseconds>(end2-start2).count()-std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " nanosegundos" << std::endl;
    std::cout << "El porcentaje de perdida es de " << 100*(std::chrono::duration_cast<std::chrono::nanoseconds>(end2-start2).count()-std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count())/std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << "%" << std::endl;

    auto totalEnd = std::chrono::high_resolution_clock::now();

    std::cout << "En total tarde " << std::chrono::duration_cast<std::chrono::nanoseconds>(totalEnd-total).count()/1000000000 << " segundos" << std::endl;
}

int main() {
    std::cout << "Comienza" << std::endl << std::endl;
    //maximoParaleloContraMaximoUnaLetra(50000, 'z');
    //maximoParaleloContraMaximoTodasLasLetras(2500);
    experimentosValor(100000, 50000); // 50000, 10000 me tardo 5 min

    return 0;
    
}