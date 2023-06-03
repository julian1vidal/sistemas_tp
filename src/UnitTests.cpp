#include <vector>
#include "lib/littletest.hpp"

#include "../src/ListaAtomica.hpp"
#include "../src/HashMapConcurrente.hpp"
#include "../src/CargarArchivos.hpp"
#include <thread>

using namespace std;

ListaAtomica<int> lista;

void insertarListaAtomica(int cantidad){
    for (int i = 0 ; i<cantidad ; i++){
        lista.insertar(i);
    }
}

// Tests Ejercicio 1

LT_BEGIN_SUITE(TestsEjercicio1)

ListaAtomica<int> l;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio1)

LT_BEGIN_TEST(TestsEjercicio1, ListaComienzaVacia)
    LT_CHECK_EQ(l.longitud(), 0);
LT_END_TEST(ListaComienzaVacia)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElemento)
    l.insertar(42);
    LT_CHECK_EQ(l.longitud(), 1);
LT_END_TEST(InsertarAgregaElemento)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElementoCorrecto)
    l.insertar(42);
    LT_CHECK_EQ(l.cabeza(), 42);
LT_END_TEST(InsertarAgregaElementoCorrecto)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaEnOrden)
    l.insertar(4);
    l.insertar(3);
    l.insertar(2);
    l.insertar(1);
    LT_CHECK_EQ(l.longitud(), 4);
    LT_CHECK_EQ(l.iesimo(0), 1);
    LT_CHECK_EQ(l.iesimo(1), 2);
    LT_CHECK_EQ(l.iesimo(2), 3);
    LT_CHECK_EQ(l.iesimo(3), 4);
LT_END_TEST(InsertarAgregaEnOrden)

LT_BEGIN_SUITE(ConcurrenciaEjercicio1)

void set_up()
{
}

void tear_down()
{
}

LT_END_SUITE(ConcurrenciaEjercicio1)

LT_BEGIN_TEST(ConcurrenciaEjercicio1, InsertarSinRaceCondition)

thread t1 = thread(insertarListaAtomica,100000);
thread t2 = thread(insertarListaAtomica,100000);

t1.join();
t2.join();

LT_CHECK_EQ(lista.longitud(),200000);

LT_END_TEST(InsertarSinRaceCondition)

LT_BEGIN_SUITE(ConcurrenciaEjercicio2)

void set_up()
{
}

void tear_down()
{
}

LT_END_SUITE(ConcurrenciaEjercicio2)

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesBloqueaIncrementar)

HashMapConcurrente hashM;
hashM.incrementar("a");
hashM.incrementar("aa");
hashM.incrementar("aaa");
hashM.incrementar("aaaa");
hashM.incrementar("aaaaa");
hashM.incrementar("b");
hashM.incrementar("bb");
hashM.incrementar("bbb");
hashM.incrementar("bbbb");
hashM.incrementar("bbbbb");

bool flag = true; // Marca cuando empiezan a correr los incrementar

std::vector<std::string> res;

thread tClaves = thread(&HashMapConcurrente::clavesTestConLocks, &hashM, &res, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos
//std::cout << "El main supero flag" << std::endl;
//std::cout << "Esperando join no deberia aparecer instantaneamente" << std::endl;

hashM.incrementar("c");
LT_CHECK_EQ(flag,true); // Claves al final devuelve el flag a true
                        // Ya termino claves
                        // Si fuera false querria decir que el primer incrementar logro correr
hashM.incrementar("cc");
hashM.incrementar("ccc");
hashM.incrementar("cccc");
hashM.incrementar("ccccc");
hashM.incrementar("d");
hashM.incrementar("dd");
hashM.incrementar("ddd");
hashM.incrementar("dddd");
hashM.incrementar("ddddd");

//std::cout << "Esperando join" << std::endl;
// No deberia aparecer instantaneamente porque los incrementar se bloquearon
tClaves.join();
//std::cout << "Joined" << std::endl;

LT_CHECK_EQ(10, res.size());

LT_END_TEST(ClavesBloqueaIncrementar)

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesSinLocksTieneRaceCondition)

HashMapConcurrente hashM;
hashM.incrementar("a");
hashM.incrementar("aa");
hashM.incrementar("aaa");
hashM.incrementar("aaaa");
hashM.incrementar("aaaaa");
hashM.incrementar("b");
hashM.incrementar("bb");
hashM.incrementar("bbb");
hashM.incrementar("bbbb");
hashM.incrementar("bbbbb");
bool flag = true;
std::vector<std::string> res;
//std::cout << "Inicialize cosas" << std::endl;

thread tClaves = thread(&HashMapConcurrente::clavesTestSinLocks, &hashM, &res, &flag);

while (flag){}
//std::cout << "El main supero flag" << std::endl;

hashM.incrementar("c");
hashM.incrementar("cc");
hashM.incrementar("ccc");
hashM.incrementar("cccc");
hashM.incrementar("ccccc");
hashM.incrementar("d");
hashM.incrementar("dd");
hashM.incrementar("ddd");
hashM.incrementar("dddd");
hashM.incrementar("ddddd");

//std::cout << "Esperando join" << std::endl;
tClaves.join();
//std::cout << "Joined" << std::endl;

LT_CHECK_NEQ(10, res.size());

LT_END_TEST(ClavesSinLocksTieneRaceCondition)



LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesYMaximoNoSeBloquean)

HashMapConcurrente hashM2;

hashM2.incrementar("a");
hashM2.incrementar("aa");
hashM2.incrementar("aaa");
hashM2.incrementar("aaaa");
hashM2.incrementar("aaaaa");
hashM2.incrementar("b");
hashM2.incrementar("bb");
hashM2.incrementar("bbb");
hashM2.incrementar("bbbb");
hashM2.incrementar("bbbbb");
hashM2.incrementar("bbbbb");
bool flag2 = true;
std::vector<std::string> res2;

thread tClaves2 = thread(&HashMapConcurrente::clavesTestConLocks, &hashM2, &res2, &flag2);
while(flag2){}
//std::cout << "El main supero flag 2" << std::endl;

unsigned int max2 = hashM2.maximo().second;
LT_CHECK_EQ(flag2,false);
LT_CHECK_EQ(max2, 2);

//std::cout << "Esperando join" << std::endl;
tClaves2.join();
//std::cout << "Joined" << std::endl;

LT_CHECK_EQ(flag2, true);

max2 = 0;

std::vector<std::string> res3;

thread tMaximo2 = thread(&HashMapConcurrente::maximoTest, &hashM2, &flag2);
while(flag2){}
//std::cout << "El main supero flag2 por segunda vez" << std::endl;

res3 = hashM2.claves();
LT_CHECK_EQ(flag2, false);
LT_CHECK_EQ(res3.size(), 10);

//std::cout << "Esperando join" << std::endl;
tMaximo2.join();
//std::cout << "Joined" << std::endl;

LT_CHECK_EQ(flag2, true);


LT_END_TEST(ClavesYMaximoNoSeBloquean)



LT_BEGIN_TEST(ConcurrenciaEjercicio2, Claves2NoBloqueaIncrementar)

HashMapConcurrente hashM3;
hashM3.incrementar2("a");
hashM3.incrementar2("aa");
hashM3.incrementar2("aaa");
hashM3.incrementar2("aaaa");
hashM3.incrementar2("aaaaa");
hashM3.incrementar2("b");
hashM3.incrementar2("bb");
hashM3.incrementar2("bbb");
hashM3.incrementar2("bbbb");
hashM3.incrementar2("bbbbb");

bool flag = true; // Marca cuando empiezan a correr los incrementar

std::vector<std::string> res;

thread tClaves = thread(&HashMapConcurrente::clavesTest2, &hashM3, &res, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos
//std::cout << "El main supero flag" << std::endl;
//std::cout << "Esperando join no deberia aparecer instantaneamente" << std::endl;

hashM3.incrementar2("c");
hashM3.incrementar2("cc");
hashM3.incrementar2("ccc");
hashM3.incrementar2("cccc");
hashM3.incrementar2("ccccc");
hashM3.incrementar2("d");
hashM3.incrementar2("dd");
hashM3.incrementar2("ddd");
hashM3.incrementar2("dddd");
hashM3.incrementar2("ddddd");

//std::cout << "Esperando join" << std::endl;
LT_CHECK_EQ(flag, false);

tClaves.join();
//std::cout << "Joined" << std::endl;

LT_CHECK_EQ(10, res.size());

LT_END_TEST(Claves2NoBloqueaIncrementar)



LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorBloqueaIncrementar)

HashMapConcurrente hashM4;
bool flag4 = true;
std::string clave = "clave";
unsigned int res;

// El false es para decirle que levante el flag despues de proteger la variable con el mutex
// Deberia bloquear a incrementar
thread tValor = thread(&HashMapConcurrente::valorTest, &hashM4, clave, &res, &flag4, false);
// No se mete al while, libera la barrera al toque y su resultado va a ser 0

while(flag4){}
//std::cout << "El main supero flag4" << std::endl;

hashM4.incrementar(clave);
LT_CHECK_EQ(flag4,false); // Valor esta durmiendo
tValor.join();
LT_CHECK_EQ(flag4,true); // Ya termino valor

unsigned int res2;

thread tValor2 = thread(&HashMapConcurrente::valorTest, &hashM4, clave, &res2, &flag4, false);

while(flag4){}
//std::cout << "El main supero flag4 por segunda vez" << std::endl;

hashM4.incrementar(clave);
LT_CHECK_EQ(flag4,true); // Si corrio el primer incrementar, tuvo que haber terminado valor
hashM4.incrementar(clave);

tValor2.join();

LT_CHECK_EQ(res2,1); // No deberian haber corrido los incrementar en el medio

LT_CHECK_EQ(hashM4.valor(clave),3);

std::string clave2 = "claveclave";
unsigned int res3;

// El true es para que se quede dormido despues de avanzar el iterador
// Asi incrementar puede agregar una palabra nueva durante la ejecucion de valor
thread tValor3 = thread(&HashMapConcurrente::valorTest, &hashM4, clave2, &res3, &flag4, true);

while(flag4){}
//std::cout << "El main supero flag 4 por tercera vez" << std::endl;

hashM4.incrementar(clave2);
LT_CHECK_EQ(flag4,false); // Sigue corriendo valor

tValor3.join();

LT_CHECK_EQ(res3,0);

LT_CHECK_EQ(hashM4.valor(clave2),1);

LT_END_TEST(ValorBloqueaIncrementar)



LT_BEGIN_TEST(ConcurrenciaEjercicio2, Valor2BloqueaIncrementar)

HashMapConcurrente hashM5;

bool flag5 = true;
unsigned int res;

std:string clave = "clave";

hashM5.incrementar(clave);

thread tValor2 = thread(&HashMapConcurrente::valorTest2, &hashM5, clave, &res, &flag5);

while(flag5){}
//std::cout << "El main supero flag 5" << std::endl;

hashM5.incrementar(clave);
LT_CHECK_EQ(flag5,true); // Valor2 ya termino
hashM5.incrementar(clave);

tValor2.join();

LT_CHECK_EQ(res,1);
LT_CHECK_EQ(hashM5.valor(clave),3);

LT_END_TEST(Valor2BloqueaIncrementar)

// Tests Ejercicio 2*/

LT_BEGIN_SUITE(TestsEjercicio2)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio2)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoEnHashMapVacio)
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 0);
LT_END_TEST(ValorEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoEnHashMapVacio)
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasUnaInsercion)
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 1);
LT_END_TEST(ValorEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasUnaInsercion)
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {"tiranosaurio"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
LT_END_TEST(ValorEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {"tiranosaurio"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("triceratops");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("triceratops"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("triceratops");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "tiranosaurio") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "triceratops") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("estegosaurio"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesDistintoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "tiranosaurio") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "estegosaurio") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)

// Tests Ejercicio 3

LT_BEGIN_SUITE(TestsEjercicio3)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio3)

LT_BEGIN_TEST(TestsEjercicio3, MaximoEsCorrecto)

    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("estegosaurio");

    hashMapPair actual = hM.maximo();

    LT_CHECK_EQ(actual.first, "tiranosaurio");
    LT_CHECK_EQ(actual.second, 4);
LT_END_TEST(MaximoEsCorrecto)

LT_BEGIN_TEST(TestsEjercicio3, MaximoParaleloEsCorrectoUnThread)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("estegosaurio");

    hashMapPair actual = hM.maximoParalelo(1);
    LT_CHECK_EQ(actual.first, "tiranosaurio");
    LT_CHECK_EQ(actual.second, 4);
LT_END_TEST(MaximoParaleloEsCorrectoUnThread)

LT_BEGIN_TEST(TestsEjercicio3, MaximoParaleloEsCorrectoDosThreads)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("estegosaurio");

    hashMapPair actual = hM.maximoParalelo(2);
    LT_CHECK_EQ(actual.first, "tiranosaurio");
    LT_CHECK_EQ(actual.second, 4);
LT_END_TEST(MaximoParaleloEsCorrectoDosThreads)

// Tests Ejercicio 4

LT_BEGIN_SUITE(TestsEjercicio4)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio4)

LT_BEGIN_TEST(TestsEjercicio4, CargarArchivoFunciona)
    cargarArchivo(hM, "data/test-1");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 1);
    LT_CHECK_EQ(hM.claves().size(), 5);
LT_END_TEST(CargarArchivoFunciona)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaUnThread)
    cargarMultiplesArchivos(hM, 1, {"data/test-1", "data/test-2", "data/test-3"});
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("linux"), 3);
    LT_CHECK_EQ(hM.valor("estegosaurio"), 4);
    LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaUnThread)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaDosThreads)
    cargarMultiplesArchivos(hM, 2, {"data/test-1", "data/test-2", "data/test-3"});
    // Habian seteado para que solo haya 1 thread...
    
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("linux"), 3);
    LT_CHECK_EQ(hM.valor("estegosaurio"), 4);
    LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaDosThreads)

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
    AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
