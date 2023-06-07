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

/* -
   -
   Orden de los tests:
    - Tests catedra listas

    - Insertar sin race condition

    - Valor        bloquea a incrementarLocks
    - ValorInicio  bloquea a incrementarLocks
    - ClavesLocks       bloquea a incrementarLocks
    - ClavesLocks  sin locks tiene race conditions
    - Maximo       bloquea a incrementarLocks
    - Maximo Paralelo bloquea a incrementarLocks
    - Claves no bloquea a incrementar
    
    - Valor       y Maximo no se bloquean
    - ValorInicio y Maximo no se bloquean
    - Valor       y Maximo Paralelo no se bloquean
    - ValorInicio y Maximo Paralelo no se bloquean
    - Valor       y ClavesLocks no se bloquean
    - ValorInicio y ClavesLocks no se bloquean
    - ClavesLocks      y Maximo no se bloquean
    - ClavesLocks      y Maximo Paralelo no se bloquean
    - Maximo Paralelo 1 thread

    - Tests catedra ejercicio 2, 3 y 4

*/

// TESTS CATEDRA EJERCICIO 1

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





// TESTS PROPIOS EJERCICIO 1

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


// TESTS PROPIOS EJERCICIO 2

LT_BEGIN_SUITE(ConcurrenciaEjercicio2)

void set_up()
{
}

void tear_down()
{
}

LT_END_SUITE(ConcurrenciaEjercicio2)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorBloqueaincrementarLocks)

HashMapConcurrente hashM;
bool flag = true;
std::string clave = "clave";
unsigned int res;

// El false es para decirle que levante el flag despues de proteger la variable con el mutex
// Deberia bloquear a incrementarLocks
thread tValor = thread(&HashMapConcurrente::valorTest, &hashM, clave, &res, &flag, false);
// No se mete al while, libera la barrera al toque y su resultado va a ser 0

while(flag){}

hashM.incrementarLocks(clave);
LT_CHECK_EQ(flag,false); // Valor esta durmiendo, incrementarLocks debio poder movers porque esta vacio el hashmap
tValor.join();
LT_CHECK_EQ(flag,true); // Ya termino valor

unsigned int res2;

// el false hace que se quede trabado despues de reclamar el mutex, cuando va conseguir el valor de la clave
thread tValor2 = thread(&HashMapConcurrente::valorTest, &hashM, clave, &res2, &flag, false);

while(flag){}

hashM.incrementarLocks(clave);
LT_CHECK_EQ(flag,true); // Si corrio el incrementarLocks de arriba, tuvo que haber terminado valor
hashM.incrementarLocks(clave);

tValor2.join();

LT_CHECK_EQ(res2,1); // No deberian haber corrido los incrementarLocks en el medio

LT_CHECK_EQ(hashM.valor(clave),3);

std::string clave2 = "claveclave";
unsigned int res3;

// El true es para que se quede dormido despues de avanzar el iterador
// Asi incrementarLocks puede agregar una palabra nueva durante la ejecucion de valor
thread tValor3 = thread(&HashMapConcurrente::valorTest, &hashM, clave2, &res3, &flag, true);

while(flag){}

hashM.incrementarLocks(clave2);
LT_CHECK_EQ(flag,false); // Sigue corriendo valor

tValor3.join();

LT_CHECK_EQ(res3,0);

LT_CHECK_EQ(hashM.valor(clave2),1);

LT_END_TEST(ValorBloqueaincrementarLocks)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorInicioBloqueaincrementarLocks)

HashMapConcurrente hashM;

bool flag = true;
unsigned int res;

std:string clave = "clave";

hashM.incrementarLocks(clave);

thread tValor2 = thread(&HashMapConcurrente::valorInicioTest, &hashM, clave, &res, &flag);

while(flag){}

hashM.incrementarLocks(clave);
LT_CHECK_EQ(flag,true); // Valor2 ya termino
hashM.incrementarLocks(clave);

tValor2.join();

LT_CHECK_EQ(res,1);
LT_CHECK_EQ(hashM.valor(clave),3);

LT_END_TEST(ValorInicioBloqueaincrementarLocks)

// // // // // // // // // // // // // // //


LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesLocksBloqueaincrementarLocks)

HashMapConcurrente hashM;
hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");

bool flag = true; // Marca cuando empiezan a correr los incrementarLocks

std::vector<std::string> res;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestConLocks, &hashM, &res, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos

hashM.incrementarLocks("c");
LT_CHECK_EQ(flag,true); // Claves al final devuelve el flag a true
                        // Ya termino claves
                        // Si fuera false querria decir que el primer incrementarLocks logro correr
hashM.incrementarLocks("cc");
hashM.incrementarLocks("ccc");
hashM.incrementarLocks("cccc");
hashM.incrementarLocks("ccccc");
hashM.incrementarLocks("d");
hashM.incrementarLocks("dd");
hashM.incrementarLocks("ddd");
hashM.incrementarLocks("dddd");
hashM.incrementarLocks("ddddd");

// No deberia aparecer instantaneamente porque los incrementarLocks se bloquearon
tClavesLocks.join();

LT_CHECK_EQ(10, res.size());

LT_END_TEST(ClavesLocksBloqueaincrementarLocks)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesLocksSinLocksTieneRaceCondition)

HashMapConcurrente hashM;
hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
bool flag = true;
std::vector<std::string> res;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestSinLocks, &hashM, &res, &flag);

while (flag){}

hashM.incrementarLocks("c");
hashM.incrementarLocks("cc");
hashM.incrementarLocks("ccc");
hashM.incrementarLocks("cccc");
hashM.incrementarLocks("ccccc");
hashM.incrementarLocks("d");
hashM.incrementarLocks("dd");
hashM.incrementarLocks("ddd");
hashM.incrementarLocks("dddd");
hashM.incrementarLocks("ddddd");

tClavesLocks.join();

LT_CHECK_NEQ(10, res.size());

LT_END_TEST(ClavesLocksSinLocksTieneRaceCondition)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, MaximoBloqueaAincrementarLocks)

HashMapConcurrente hashM;
hashM.incrementarLocks("a");
hashM.incrementarLocks("b");
hashM.incrementarLocks("b");

bool flag = true; // Marca cuando empiezan a correr los incrementarLocks

hashMapPair res;

thread tMaximo = thread(&HashMapConcurrente::maximoTest, &hashM, &res, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos

hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
LT_CHECK_EQ(flag,true); // Claves al final devuelve el flag a true
                        // Ya termino claves
                        // Si fuera false querria decir que el primer incrementarLocks logro correr
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");

// No deberia aparecer instantaneamente porque los incrementarLocks se bloquearon
tMaximo.join();

LT_CHECK_EQ(2, res.second);
LT_CHECK_EQ("b", res.first);

LT_END_TEST(MaximoBloqueaAincrementarLocks)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, MaximoParaleloBloqueaAincrementarLocks)

HashMapConcurrente hashM;
hashM.incrementarLocks("a");
hashM.incrementarLocks("b");
hashM.incrementarLocks("b");

bool flag = true; // Marca cuando empiezan a correr los incrementarLocks

hashMapPair res;

// maximoParaleloTest tiene se duerme por 2 segundos en el bucket de la "b"
thread tMaximoP = thread(&HashMapConcurrente::maximoParaleloTest, &hashM, &res, 2, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos


hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
LT_CHECK_EQ(flag,false); // libero el primer bucket pero todavia no termino

hashM.incrementarLocks("b");
hashM.incrementarLocks("b");
hashM.incrementarLocks("b");
LT_CHECK_EQ(flag,true); // MaximoP deberia haber terminado porque "b" era el ultimo bucket


// No deberia aparecer instantaneamente porque los incrementarLocks se bloquearon
tMaximoP.join();

LT_CHECK_EQ(2, res.second);
LT_CHECK_EQ("b", res.first);

LT_END_TEST(MaximoParaleloBloqueaAincrementarLocks)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesNoBloqueaincrementarLocks)

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

bool flag = true; // Marca cuando empiezan a correr los incrementarLocks

std::vector<std::string> res;
std::vector<std::string> res2;

thread tClaves = thread(&HashMapConcurrente::clavesTest, &hashM, &res, &flag);

while (flag){} // Hacemos busy waiting, se libera cuando claves logra tomar los semaforos

hashM.incrementar("c");
hashM.incrementar("cc");
hashM.incrementar("ccc");
hashM.incrementar("cccc");
hashM.incrementar("ccccc");
LT_CHECK_EQ(flag, false);
bool flag2 = true;
thread tClaves2 = thread(&HashMapConcurrente::clavesTest, &hashM, &res2, &flag2);
while(flag2){}
hashM.incrementar("d");
hashM.incrementar("dd");
hashM.incrementar("ddd");
hashM.incrementar("dddd");
hashM.incrementar("ddddd");

LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(flag2, false);
// Todavia no termino ningun claves, estan corriendo en paralelo

tClaves.join();
tClaves2.join();

LT_CHECK_EQ(10, res.size());
LT_CHECK_EQ(15, res2.size());

LT_END_TEST(ClavesNoBloqueaincrementarLocks)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorYMaximoNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");

bool flag = true;
unsigned int resVal;

thread tValor = thread(&HashMapConcurrente::valorTest, &hashM, "a", &resVal, &flag, false);

while(flag){}

hashMapPair resMax = hashM.maximo();
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(resMax.first, "a");
LT_CHECK_EQ(resMax.second, 3);

tValor.join();

LT_CHECK_EQ(resVal,3);
LT_CHECK_EQ(flag, true);

resVal = 0;
hashMapPair resMax2;

thread tMaximoP = thread(&HashMapConcurrente::maximoTest, &hashM, &resMax2, &flag);
while(flag){}

resVal = hashM.valor("a");
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(resVal, 3);

tMaximoP.join();

LT_CHECK_EQ("a",resMax2.first);
LT_CHECK_EQ(3,resMax2.second);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorYMaximoNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorInicioYMaximoNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");

bool flag = true;
unsigned int resVal;

thread tValor = thread(&HashMapConcurrente::valorInicioTest, &hashM, "a", &resVal, &flag);

while(flag){}

hashMapPair resMax = hashM.maximo();
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(resMax.first, "a");
LT_CHECK_EQ(resMax.second, 3);

tValor.join();

LT_CHECK_EQ(resVal,3);
LT_CHECK_EQ(flag, true);

resVal = 0;
hashMapPair resMax2;

thread tMaximoP = thread(&HashMapConcurrente::maximoTest, &hashM, &resMax2, &flag);
while(flag){}

resVal = hashM.valorInicio("a");
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(resVal, 3);

tMaximoP.join();

LT_CHECK_EQ("a",resMax2.first);
LT_CHECK_EQ(3,resMax2.second);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorInicioYMaximoNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorYMaximoParaleloNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");

bool flag = true;
unsigned int resVal;

thread tValor = thread(&HashMapConcurrente::valorTest, &hashM, "a", &resVal, &flag, false);

while(flag){}

hashMapPair resMax = hashM.maximoParalelo(2);
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(resMax.first, "a");
LT_CHECK_EQ(resMax.second, 3);

tValor.join();

LT_CHECK_EQ(resVal,3);
LT_CHECK_EQ(flag, true);

resVal = 0;
hashMapPair resMax2;

thread tMaximoP = thread(&HashMapConcurrente::maximoParaleloTest, &hashM, &resMax2, 2, &flag);
while(flag){}

resVal = hashM.valor("a");
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(resVal, 3);

tMaximoP.join();

LT_CHECK_EQ("a",resMax2.first);
LT_CHECK_EQ(3,resMax2.second);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorYMaximoParaleloNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorInicioYMaximoParaleloNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("a");
hashM.incrementarLocks("a");

bool flag = true;
unsigned int resVal;

thread tValor = thread(&HashMapConcurrente::valorInicioTest, &hashM, "a", &resVal, &flag);

while(flag){}

hashMapPair resMax = hashM.maximoParalelo(2);
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(resMax.first, "a");
LT_CHECK_EQ(resMax.second, 3);

tValor.join();

LT_CHECK_EQ(resVal,3);
LT_CHECK_EQ(flag, true);

resVal = 0;
hashMapPair resMax2;

thread tMaximoP = thread(&HashMapConcurrente::maximoParaleloTest, &hashM, &resMax2, 2, &flag);
while(flag){}

resVal = hashM.valorInicio("a");
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(resVal, 3);

tMaximoP.join();

LT_CHECK_EQ("a",resMax2.first);
LT_CHECK_EQ(3,resMax2.second);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorInicioYMaximoParaleloNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorYClavesLocksNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
hashM.incrementarLocks("bbbbb");
bool flag = true;
std::vector<std::string> res2;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestConLocks, &hashM, &res2, &flag);
while(flag){}

unsigned int valor = hashM.valor("bbbbb");
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(valor, 2);

tClavesLocks.join();

LT_CHECK_EQ(flag, true);

valor = 0;

std::vector<std::string> res3;

thread tValor = thread(&HashMapConcurrente::valorTest, &hashM, "bbbbb", &valor, &flag, false);
while(flag){}

res3 = hashM.clavesLocks();
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(res3.size(), 10);

tValor.join();

LT_CHECK_EQ(2,valor);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorYClavesLocksNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ValorInicioYClavesLocksNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
hashM.incrementarLocks("bbbbb");
bool flag = true;
std::vector<std::string> res2;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestConLocks, &hashM, &res2, &flag);
while(flag){}

unsigned int valor = hashM.valorInicio("bbbbb");
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(valor, 2);

tClavesLocks.join();

LT_CHECK_EQ(flag, true);

valor = 0;

std::vector<std::string> res3;

thread tValor = thread(&HashMapConcurrente::valorInicioTest, &hashM, "bbbbb", &valor, &flag);
while(flag){}

res3 = hashM.clavesLocks();
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(res3.size(), 10);

tValor.join();

LT_CHECK_EQ(2,valor);
LT_CHECK_EQ(flag, true);

LT_END_TEST(ValorInicioYClavesLocksNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesLocksYMaximoNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
hashM.incrementarLocks("bbbbb");
bool flag = true;
std::vector<std::string> res2;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestConLocks, &hashM, &res2, &flag);
while(flag){}

unsigned int max2 = hashM.maximo().second;
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(max2, 2);

tClavesLocks.join();

LT_CHECK_EQ(flag, true);

max2 = 0;

std::vector<std::string> res3;
hashMapPair resMaximo;

thread tMaximo2 = thread(&HashMapConcurrente::maximoTest, &hashM, &resMaximo, &flag);
while(flag){}

res3 = hashM.clavesLocks();
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(res3.size(), 10);

tMaximo2.join();

LT_CHECK_EQ("bbbbb",resMaximo.first);
LT_CHECK_EQ(2,resMaximo.second);
LT_CHECK_EQ(flag, true);


LT_END_TEST(ClavesLocksYMaximoNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, ClavesLocksYMaximoParaleloNoSeBloquean)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
hashM.incrementarLocks("bbbbb");
bool flag = true;
std::vector<std::string> res2;

thread tClavesLocks = thread(&HashMapConcurrente::clavesLocksTestConLocks, &hashM, &res2, &flag);
while(flag){}

unsigned int max2 = hashM.maximo().second;
LT_CHECK_EQ(flag,false);
LT_CHECK_EQ(max2, 2);

tClavesLocks.join();

LT_CHECK_EQ(flag, true);

max2 = 0;

std::vector<std::string> res3;
hashMapPair resMaximo;

thread tMaximoP = thread(&HashMapConcurrente::maximoParaleloTest, &hashM, &resMaximo, 2, &flag);
while(flag){}

res3 = hashM.clavesLocks();
LT_CHECK_EQ(flag, false);
LT_CHECK_EQ(res3.size(), 10);

tMaximoP.join();

LT_CHECK_EQ("bbbbb",resMaximo.first);
LT_CHECK_EQ(2,resMaximo.second);
LT_CHECK_EQ(flag, true);


LT_END_TEST(ClavesLocksYMaximoParaleloNoSeBloquean)

// // // // // // // // // // // // // // //

LT_BEGIN_TEST(ConcurrenciaEjercicio2, MaximoParalelo1Thread)

HashMapConcurrente hashM;

hashM.incrementarLocks("a");
hashM.incrementarLocks("aa");
hashM.incrementarLocks("aaa");
hashM.incrementarLocks("aaaa");
hashM.incrementarLocks("aaaaa");
hashM.incrementarLocks("b");
hashM.incrementarLocks("bb");
hashM.incrementarLocks("bbb");
hashM.incrementarLocks("bbbb");
hashM.incrementarLocks("bbbbb");
hashM.incrementarLocks("bbbbb");

hashMapPair res = hashM.maximoParalelo(1);
LT_CHECK_EQ("bbbbb", res.first);
LT_CHECK_EQ(2, res.second);
res = hashM.maximoParalelo(0);
LT_CHECK_EQ("bbbbb", res.first);
LT_CHECK_EQ(2, res.second);
res = hashM.maximoParalelo(30);
LT_CHECK_EQ("bbbbb", res.first);
LT_CHECK_EQ(2, res.second);

LT_END_TEST(MaximoParalelo1Thread)

// TESTS CATEDRA EJERCICIO 2

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
    LT_CHECK_EQ(hM.valorInicio("tiranosaurio"), 0);
LT_END_TEST(ValorEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoEnHashMapVacio)
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> actualLocks = hM.clavesLocks();
    std::vector<std::string> expected = {};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
    LT_CHECK_COLLECTIONS_EQ(actualLocks.begin(), actualLocks.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasUnaInsercion)
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 1);
    LT_CHECK_EQ(hM.valorInicio("tiranosaurio"), 1);
LT_END_TEST(ValorEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasUnaInsercion)
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> actualLocks = hM.clavesLocks();
    std::vector<std::string> expected = {"tiranosaurio"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
    LT_CHECK_COLLECTIONS_EQ(actualLocks.begin(), actualLocks.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valorInicio("tiranosaurio"), 2);
LT_END_TEST(ValorEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> actualLocks = hM.clavesLocks();
    std::vector<std::string> expected = {"tiranosaurio"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
    LT_CHECK_COLLECTIONS_EQ(actualLocks.begin(), actualLocks.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("triceratops");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("triceratops"), 1);
    LT_CHECK_EQ(hM.valorInicio("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valorInicio("triceratops"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("tiranosaurio");
    hM.incrementar("triceratops");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> actualLocks = hM.clavesLocks();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "tiranosaurio") != actual.end());
    LT_CHECK(std::find(actualLocks.begin(), actualLocks.end(), "tiranosaurio") != actualLocks.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "triceratops") != actual.end());
    LT_CHECK(std::find(actualLocks.begin(), actualLocks.end(), "triceratops") != actualLocks.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("tiranosaurio");
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("estegosaurio"), 1);
    LT_CHECK_EQ(hM.valorInicio("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valorInicio("estegosaurio"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesDistintoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("tiranosaurio");
    hM.incrementar("estegosaurio");
    hM.incrementar("tiranosaurio");
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> actualLocks = hM.clavesLocks();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "tiranosaurio") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "estegosaurio") != actual.end());
    LT_CHECK(std::find(actualLocks.begin(), actualLocks.end(), "tiranosaurio") != actualLocks.end());
    LT_CHECK(std::find(actualLocks.begin(), actualLocks.end(), "estegosaurio") != actualLocks.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)

// TESTS CATEDRA EJERCICIO 3

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
    
    LT_CHECK_EQ(hM.valor("tiranosaurio"), 2);
    LT_CHECK_EQ(hM.valor("linux"), 3);
    LT_CHECK_EQ(hM.valor("estegosaurio"), 4);
    LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaDosThreads)

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
    AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
