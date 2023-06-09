import matplotlib.pyplot as plt
import os

def plotGraph():
    

    if os.path.isfile("maximoThreads.txt") == True:
        f = open("maximoThreads.txt", "r")
        line = f.readline()
        f.close()
        values = line.split("--")
        values = values[:-1]
        values = [i.split("-") for i in values]
        print(values)
        values_max = [values[i][0] for i in range(len(values))]
        std = [values[i][1] for i in range(len(values)) ]
        values_max = [float(i) for i in values_max]
        std = [float(i) for i in std]
        X = range(1, 27)
        
        plt.errorbar(X, values_max, yerr=std ,fmt='-bo')
        plt.xlabel("Cantidad de threads")
        plt.ylabel("Tiempo de ejecucion (ns)")
        plt.show()
        #os.remove("maximoThreads.txt")

    else:
        #Read from maximo.txt
        f = open("maximo.txt", "r")
        line = f.readline()
        f.close()
        values = line.split("--")
        values = values[:-1]
        values = [i.split("-") for i in values]
        print(values)
        values_max = [values[i][0] for i in range(len(values))]
        std = [values[i][1] for i in range(len(values)) ]
        #Read from maximoParalelo.txt
        f = open("maximoParalelo.txt", "r")
        line = f.readline()
        f.close()
        valuesParalelo = line.split("--")
        valuesParalelo = valuesParalelo[:-1]
        valuesParalelo = [i.split("-") for i in valuesParalelo]
        valuesParalelo_max = [valuesParalelo[i][0] for i in range(len(valuesParalelo))]
        stdParalelo = [valuesParalelo[i][1] for i in range(len(valuesParalelo)) ]
        values_max = [float(i) for i in values_max]
        std = [float(i) for i in std]
        valuesParalelo_max = [float(i) for i in valuesParalelo_max]
        stdParalelo = [float(i) for i in stdParalelo]
        #Plot
        X = [16*2**i for i in range(10)]
        
        plt.errorbar(X, values_max, yerr=std ,fmt='-bo', label="Secuencial")
        plt.errorbar(X, valuesParalelo_max, yerr=stdParalelo ,fmt='-ro', label="Paralelo")
        plt.xlabel("Palabras en el hashmap en todas las letras")
        plt.ylabel("Tiempo de ejecucion (ns)")
        plt.legend()
        plt.show()
        os.remove("maximo.txt")
        os.remove("maximoParalelo.txt")

plotGraph()
