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
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

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

pair<int,int> maximoParaleloContraMaximoTodasLasLetras(int cantidad){
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
    int tiempo_maximo_paralelo = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo2 = (*miHashMap).maximo();
    end = std::chrono::high_resolution_clock::now();
    int tiempo_maximo = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count(); 
    delete miHashMap;
    return make_pair(tiempo_maximo,tiempo_maximo_paralelo);
}

pair<int,int> maximoParaleloContraMaximoUnaLetra(int cantidad, char letra){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    for (int i = 0; i < cantidad; i++) {
        std::string palabra = generateRandomWord(letra);
        (*miHashMap).incrementar(palabra);
    }
    auto start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo = (*miHashMap).maximoParalelo(26);
    auto end = std::chrono::high_resolution_clock::now();
    int tiempoMaximoParalelo = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    start = std::chrono::high_resolution_clock::now();
    hashMapPair maximo2 = (*miHashMap).maximo();
    end = std::chrono::high_resolution_clock::now();
    int tiempoMaximo = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    delete miHashMap;
    return make_pair(tiempoMaximo,tiempoMaximoParalelo);
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

void writeToFile(std::vector<pair<double,double>> vector, std::string nombreArchivo) {
    std::ofstream file(nombreArchivo);
     if (!file) {
          throw std::runtime_error("Error al abrir el archivo");
     }
     for (int i = 0; i < vector.size(); i++) {
         file << vector[i].first << " - " << vector[i].second << " -- ";
     }
     file.close();
}

void tiemposTodasLasLetras(){
    double num = 16;
    pair<double,double> valores;
    vector<pair<double,double>> valoresMaximo = {};
    vector<pair<double,double>> valoresMaximoParalelo = {};
    for (double i = 0; i < 10; i++) {
        num = num * 2;
        vector<double> suma_maximo = {};
        vector<double> suma_maximo_paralelo = {};
        for(double j = 0; j < 10; j++){
            valores = maximoParaleloContraMaximoTodasLasLetras(num);
            suma_maximo.push_back(valores.first);
            suma_maximo_paralelo.push_back(valores.second);
        }
        double promedio_maximo = 0;
        double promedio_maximo_paralelo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            promedio_maximo += suma_maximo[k];
            promedio_maximo_paralelo += suma_maximo_paralelo[k];
        }
        promedio_maximo = promedio_maximo / suma_maximo.size();
        promedio_maximo_paralelo = promedio_maximo_paralelo / suma_maximo_paralelo.size();
        double std_maximo = 0;
        double std_maximo_paralelo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            // cout << std_maximo << endl;
            cout << abs(suma_maximo_paralelo[k] - promedio_maximo_paralelo) << endl;
            cout <<"hola"  << endl;
            std_maximo += abs(suma_maximo[k] - promedio_maximo)*abs(suma_maximo[k] - promedio_maximo);
            
            std_maximo_paralelo += abs(suma_maximo_paralelo[k] - promedio_maximo_paralelo)*abs(suma_maximo_paralelo[k] - promedio_maximo_paralelo);
        }
        // cout << "std_maximo: " << std_maximo << endl;
        std_maximo = sqrt(std_maximo / suma_maximo.size());
        cout << "std_maximo: " << std_maximo_paralelo << endl;
        std_maximo_paralelo = sqrt(std_maximo_paralelo / suma_maximo_paralelo.size());
        cout << "std_maximo: " << std_maximo_paralelo << endl;
        pair<double,double> maximo = make_pair(promedio_maximo, std_maximo);
        pair<double,double> maximoParalelo = make_pair(promedio_maximo_paralelo, std_maximo_paralelo);
        valoresMaximo.push_back(maximo);
        valoresMaximoParalelo.push_back(maximoParalelo);
        cout << "Termine con " << i << endl;
    }
    writeToFile(valoresMaximo, "maximo.txt");
    writeToFile(valoresMaximoParalelo, "maximoParalelo.txt");
}

void tiemposUnaLetra(){
    double num = 16;
    pair<double,double> valores;
    vector<pair<double,double>> valoresMaximo = {};
    vector<pair<double,double>> valoresMaximoParalelo = {};
    for (double i = 0; i < 10; i++) {
        num = num * 2;
        vector<double> suma_maximo = {};
        vector<double> suma_maximo_paralelo = {};
        for(double j = 0; j < 10; j++){
            valores = maximoParaleloContraMaximoUnaLetra(num, 'a');
            suma_maximo.push_back(valores.first);
            suma_maximo_paralelo.push_back(valores.second);
        }
        double promedio_maximo = 0;
        double promedio_maximo_paralelo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            promedio_maximo += suma_maximo[k];
            promedio_maximo_paralelo += suma_maximo_paralelo[k];
        }
        promedio_maximo = promedio_maximo / suma_maximo.size();
        promedio_maximo_paralelo = promedio_maximo_paralelo / suma_maximo_paralelo.size();
        double std_maximo = 0;
        double std_maximo_paralelo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            std_maximo += abs(suma_maximo[k] - promedio_maximo)*abs(suma_maximo[k] - promedio_maximo);
            std_maximo_paralelo += abs(suma_maximo_paralelo[k] - promedio_maximo_paralelo)*abs(suma_maximo_paralelo[k] - promedio_maximo_paralelo);
        }
        cout << "std_maximo: " << std_maximo << endl;
        std_maximo = sqrt(std_maximo / suma_maximo.size());
        cout << "std_maximo: " << std_maximo << endl;
        std_maximo_paralelo = sqrt(std_maximo_paralelo / suma_maximo_paralelo.size());
        pair<double,double> maximo = make_pair(promedio_maximo, std_maximo);
        pair<double,double> maximoParalelo = make_pair(promedio_maximo_paralelo, std_maximo_paralelo);
        valoresMaximo.push_back(maximo);
        valoresMaximoParalelo.push_back(maximoParalelo);
        cout << "Termine con " << i << endl;
    }
    writeToFile(valoresMaximo, "maximo.txt");
    writeToFile(valoresMaximoParalelo, "maximoParalelo.txt");
}

void tiemposMaximoConThreads(){
    HashMapConcurrente* miHashMap = new HashMapConcurrente();
    for (double j=0; j<25; j++){
        for (double i = 0; i < 100000; i++) {
            std::string palabra = generateRandomWord(letras[j]);
            (*miHashMap).incrementar(palabra);
        }
    }
    vector<pair<double,double>> valoresMaximo = {};
    for (double i = 1; i <= 26; i++) {
        vector<double> suma_maximo = {};

        for(double j=0; j<10;j++){
            auto start = std::chrono::high_resolution_clock::now();
            (*miHashMap).maximoParalelo(i);
            auto end = std::chrono::high_resolution_clock::now();
            suma_maximo.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count());
        }
        double promedio_maximo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            promedio_maximo += suma_maximo[k];
        }
        promedio_maximo = promedio_maximo / suma_maximo.size();
        double std_maximo = 0;
        for (double k = 0; k < suma_maximo.size(); k++) {
            std_maximo += abs(suma_maximo[k] - promedio_maximo)*abs(suma_maximo[k] - promedio_maximo);
        }
        std_maximo = sqrt(std_maximo / suma_maximo.size());
        pair<double,double> maximo = make_pair(promedio_maximo, std_maximo);
        valoresMaximo.push_back(maximo);
        cout << "Termine con " << i << endl;
    }
    writeToFile(valoresMaximo, "maximoThreads.txt");


    delete miHashMap;
}

int main() {
    

    tiemposTodasLasLetras();
    //tiemposUnaLetra();
    //tiemposMaximoConThreads();
    return 0;
    
}
