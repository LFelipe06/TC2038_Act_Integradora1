import time #Importar la librería para calcular el tiempo total de ejecución
import numpy as np # Importar la librería para mostrar la gráfica
import pylab as pl # Importar librería para la graficación
from memory_profiler import memory_usage #Importar la librería para calcular el uso de memoria

# Definición de la función getBuckets que calcula los rangos de índices para cada carácter en la cadena T
def getBuckets(T):
    count = {}  # Diccionario para contar la frecuencia de cada carácter
    buckets = {}  # Diccionario para almacenar los rangos de índices
    for c in T:  # Itera a través de cada carácter en la cadena T
        count[c] = count.get(c, 0) + 1  # Actualiza el contador de cada carácter, si no existe el valor, agrega 0 al caracter y le suma 1, si existe, solo le suma 1
    start = 0  # Inicializa la variable start en 0

    for c in sorted(count.keys()):  # Itera a través de los carácteres ordenados en el diccionario count
        # Calcula y almacena los rangos de índices para cada carácter en buckets
        buckets[c] = (start, start + count[c])
        start += count[c]  # Actualiza el valor de start

    return buckets  # Devuelve el diccionario buckets con los rangos de índices

# Definición de la función sais que calcula el Suffix Array de la cadena T
def sais(T):
    t = ["_"] * len(T)  # Inicializa una lista de etiquetas t con "_" de longitud igual a la cadena T
    
    t[len(T) - 1] = "S"  # La última etiqueta se establece en "S"
    for i in range(len(T) - 1, 0, -1):  # Itera a través de la cadena de atrás hacia adelante, de forma que se establece los S y L
        if T[i-1] == T[i]:  # Compara caracteres adyacentes
            t[i - 1] = t[i]  # Asigna la misma etiqueta que el siguiente caracter
        else:
            t[i - 1] = "S" if T[i-1] < T[i] else "L"  # Asigna etiqueta "S" o "L" según la comparación

    buckets = getBuckets(T)  # Llama a la función getBuckets para obtener los rangos de índices

    count = {}  # Diccionario para contar la frecuencia de cada carácter
    SA = [-1] * len(T)  # Inicializa el Suffix Array SA con valores -1
    LMS = {}  # Diccionario para almacenar posiciones de sufijos largos más pequeños (LMS)
    end = None  # Inicializa la variable end como None

    for i in range(len(T) - 1, 0, -1):  # Itera a través de la cadena desde el final hacia el principio
        if t[i] == "S" and t[i - 1] == "L":  # Verifica si el sufijo es Sufijo pequeño (S) y Sufijo largo (L)
            revoffset = count[T[i]] = count.get(T[i], 0) + 1  # Calcula y almacena la posición de S en SA
            SA[buckets[T[i]][1] - revoffset] = i  # Actualiza SA con la posición de S
            if end is not None:  # Si end no es None
                LMS[i] = end  # Almacena la posición de end en LMS
            end = i  # Actualiza end

    LMS[len(T) - 1] = len(T) - 1  # Almacena la última posición en LMS
    count = {}  # Reinicia el diccionario count

    # Itera a través de SA para ordenar los sufijos L según sus símbolos
    for i in range(len(T)):
        if SA[i] >= 0:  # Si la posición en SA no es -1
            if t[SA[i] - 1] == "L":  # Verifica si el sufijo anterior es Sufijo largo (L)
                symbol = T[SA[i] - 1]  # Obtiene el símbolo correspondiente
                offset = count.get(symbol, 0)  # Obtiene el offset del símbolo en el diccionario count
                SA[buckets[symbol][0] + offset] = SA[i] - 1  # Actualiza SA con la posición correcta
                count[symbol] = offset + 1  # Actualiza el offset en el diccionario count

    count = {}  # Reinicia el diccionario count

    # Itera a través de SA para ordenar los sufijos S según sus símbolos
    for i in range(len(T) - 1, 0, -1):  # Itera desde el final hacia el principio de SA
        if SA[i] > 0:  # Si la posición en SA es mayor que 0
            if t[SA[i] - 1] == "S":  # Verifica si el sufijo anterior es Sufijo pequeño (S)
                symbol = T[SA[i] - 1]  # Obtiene el símbolo correspondiente
                revoffset = count[symbol] = count.get(symbol, 0) + 1  # Calcula el offset reverso
                SA[buckets[symbol][1] - revoffset] = SA[i] - 1  # Actualiza SA con la posición correcta

    namesp = [-1] * len(T)  # Inicializa una lista de nombres de sufijos con -1
    name = 0  # Inicializa el contador de nombres
    prev = None  # Inicializa la variable prev como None
    
    for i in range(len(SA)):  # Itera a través de SA
        if t[SA[i]] == "S" and t[SA[i] - 1] == "L":  # Verifica si el sufijo es Sufijo pequeño (S) y Sufijo largo (L)
            if prev is not None and T[SA[prev]:LMS[SA[prev]]] != T[SA[i]:LMS[SA[i]]]:
                # Compara los sufijos LMS para determinar si los nombres son distintos
                name += 1  # Incrementa el contador de nombres
            prev = i  # Actualiza prev con la posición actual
            namesp[SA[i]] = name  # Asigna el nombre al sufijo correspondiente

    names = []  # Inicializa una lista de nombres
    SApIdx = []  # Inicializa una lista de índices de sufijos LMS
    for i in range(len(T)):  # Itera a través de la cadena T
        if namesp[i] != -1:  # Si el nombre no es -1
            names.append(namesp[i])  # Agrega el nombre a la lista

            SApIdx.append(i)  # Agrega el índice del sufijo LMS a la lista SApIdx

    if name < len(names) - 1:  # Verifica si hay más de un nombre
        names = sais(names)  # Llama recursivamente a la función sais para ordenar los nombres

    names = list(reversed(names))  # Invierte la lista de nombres

    SA = [-1] * len(T)  # Inicializa el Suffix Array SA con valores -1
    count = {}  # Reinicia el diccionario count

    # Itera a través de los nombres ordenados para calcular la posición final en SA
    for i in range(len(names)):
        pos = SApIdx[names[i]]  # Obtiene el índice del sufijo LMS correspondiente
        revoffset = count[T[pos]] = count.get(T[pos], 0) + 1  # Calcula el offset reverso
        SA[buckets[T[pos]][1] - revoffset] = pos  # Actualiza SA con la posición correcta

    count = {}  # Reinicia el diccionario count

    # Itera a través de SA para ordenar los sufijos L según sus símbolos
    for i in range(len(T)):
        if SA[i] >= 0:  # Si la posición en SA no es -1
            if t[SA[i] - 1] == "L":  # Verifica si el sufijo anterior es Sufijo largo (L)
                symbol = T[SA[i] - 1]  # Obtiene el símbolo correspondiente
                offset = count.get(symbol, 0)  # Obtiene el offset del símbolo en el diccionario count
                SA[buckets[symbol][0] + offset] = SA[i] - 1  # Actualiza SA con la posición correcta
                count[symbol] = offset + 1  # Actualiza el offset en el diccionario count

    count = {}  # Reinicia el diccionario count
    # Itera a través de SA para ordenar los sufijos S según sus símbolos
    for i in range(len(T) - 1, 0, -1):  # Itera desde el final hacia el principio de SA
        if SA[i] > 0:  # Si la posición en SA es mayor que 0
            if t[SA[i] - 1] == "S":  # Verifica si el sufijo anterior es Sufijo pequeño (S)
                symbol = T[SA[i] - 1]  # Obtiene el símbolo correspondiente
                revoffset = count[symbol] = count.get(symbol, 0) + 1  # Calcula el offset reverso
                SA[buckets[symbol][1] - revoffset] = SA[i] - 1  # Actualiza SA con la posición correcta

    return SA  # Devuelve el Suffix Array calculado

if __name__ == '__main__':
    
    choice = 4 #Declaración de la variable para la elección del libro
    while (choice < 0 or choice > 3): #Ciclo para mostrar el menú y el usuario elija la opción
        choice = int(input("Elige un libro a examinar\nFrankenstein = 0\nDr. Jekyll and Mr. Hyde = 1\nA Modest Proposal = 2\nThe Yellow Wallpaper = 3\n"))
        
    match (choice): #Declaración de la variable name con respecto a la elección elegida que se utilizará para abrir el archivo con ese nombre
        case 0:
            name = "books/frankenstein.txt"
        case 1:
            name = "books/dr_jekyll_and_mr_hyde.txt"
        case 2:
            name = "books/a_modest_proposal.txt"
        case 3:
            name = "books/the_yellow_wallpaper.txt"
        

    start_time = time.time() #Comienza el conteo del tiempo 

    def openFile(filename): #Función para abrir el archivo en modo utf8
        f=open(filename,encoding="utf8")
        return f.read()


    def clearCharacters(input_file):
        with open(input_file, 'r') as archivo_entrada:
            contenido = archivo_entrada.read()

        # Elimina espacios y saltos de línea, dejando solo letras mayúsculas, minúsculas y números
        contenido_limpio = ''.join(caracter for caracter in contenido if caracter.isalpha())

        return contenido_limpio

    # Usar la función para limpiar un archivo
    input_file = name # Reemplaza con el nombre de tu archivo de entrada
    string = clearCharacters(input_file)

    # Imprimir el resultado
    print(string)
    
    # Cadena de entrada
    # string = "GTCCCGATGTCATGTCAGGA$"

    print("SAIS:")
    # Convierte cada carácter de la cadena en su valor ASCII y almacena los valores en la lista T
    T = [ord(c) for c in string]
    T.append(0) #Agrega un null al final del string del archivo para la conversión en ASCII

    # Llama a la función para calcular el Suffix Array con la cadena T anteriormente calculada
    SA = sais(T)

    # print(string)
    print(SA) # Imprime el Suffix Array
    print("%s seconds" % (time.time() - start_time)) # Imprime el tiempo total de ejcución
    memoryUsage = memory_usage((sais, (T, )), interval = 0.01) # Se guarda el registro del uso de memoria en una lista
    pl.plot(np.arange(len(memoryUsage)) * 0.1, memoryUsage, label = 'SA-IS Algorithm') #Se mandan los datos para graficar
    pl.title('Consumo de memoria') #Se define el título y el label de x y y
    pl.xlabel('Tiempo (S)')
    pl.ylabel('Uso de memoria')
    pl.show() #Se manda a mostrar la gráfica