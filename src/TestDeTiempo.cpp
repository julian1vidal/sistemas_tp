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

int main() {
    maximoParaleloContraMaximoUnaLetra(256, 'z');
    maximoParaleloContraMaximoTodasLasLetras(256);
    return 0;
    
}