#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

// Función que determina si dos subarreglos son iguales dentro de la cadena T
bool findEqual(const vector<int> &T, int start1, int end1, int start2, int end2)
{
    if (end1 - start1 != end2 - start2)
    {
        return false;
    }

    for (int i = start1, j = start2; i < end1 && j < end2; i++, j++)
    {
        if (T[i] != T[j])
        {
            return false;
        }
    }
    return true;
}

// De a partir de T, se calculan las cubetas requeridas
map<int, pair<int, int>> getBuckets(vector<int> T)
{
    map<int, int> count;
    map<int, pair<int, int>> buckets;

    for (int c : T) // Se itera a través de T y se suma a los valores repetidos de c dentro de count
    {
        count[c]++;
    }

    int start = 0;
    vector<int> keys;
    for (auto countPair : count) // Se itera a través de count agrega a la llave los valores dentro de Count
    {
        keys.push_back(countPair.first);
    }
    sort(keys.begin(), keys.end()); // Se ordena el vector de forma ascendente

    for (auto c : keys)
    {
        buckets.insert({c, {start, start + count[c]}}); // Se agrega al mapa buckets los valores de c mediante el recorrido ordenado de keys
        start += count[c]; // Se modifica el start para que la siguiente cubeta comience en esa posición
    }

    return buckets;
}

vector<int> sais(vector<int> T)
{
    vector<string> t(T.size(), "_"); // Se inicializa un vector vacío con el tamaño de T
    t[T.size() - 1] = "S"; 
    for (int i = T.size() - 1; i > 0; i--) // Se realiza una iteración para asignar S o L en t
    {
        if (T[i - 1] == T[i])
        {
            t[i - 1] = t[i];
        }
        else
        {
            t[i - 1] = T[i - 1] < T[i] ? "S" : "L";
        }
    }

    map<int, pair<int, int>> buckets = getBuckets(T);

    map<int, int> count;
    vector<int> SA(T.size(), -1);
    map<int, int> LMS;
    int end = -1;
    for (int i = T.size() - 1; i > 0; i--) //Se realiza un seguimiento para definir el LMS 
    {
        if (t[i] == "S" and t[i - 1] == "L")
        {
            count[T[i]]++;
            int revoffset = count[T[i]];
            SA[buckets[T[i]].second - revoffset] = i;
            if (end != -1)
            {
                LMS[i] = end;
            }
            end = i;
        }
    }
    LMS[T.size() - 1] = T.size() - 1;
    count.clear();
    for (int i = 0; i < T.size(); i++) // Se recorre el arreglo de forma inversa para determinar el LMS 
    {
        if (SA[i] >0)
        {
            if (t[SA[i] - 1] == "L")
            {
                int symbol = T[SA[i] - 1];
                int offset = count[symbol];
                count[symbol] = count[symbol] + 1;
                SA[buckets[symbol].first + offset] = SA[i] - 1;
            }
        }
        else if (SA[i] == 0)
        {
            if (t[t.size() - 1] == "L")
            {
                int symbol = T[T.size() - 1];
                int offset = count[symbol];
                count[symbol] = count[symbol] + 1;
                SA[buckets[symbol].first + offset] = SA[i] - 1;
            }
        }
    }

    count.clear();

    for (int i = T.size() - 1; i > 0; i--)
    {
        if (SA[i] > 0)
        {
            if (t[SA[i] - 1] == "S")
            {
                int symbol = T[SA[i] - 1];
                count[symbol] = count[symbol] + 1;
                int revoffset = count[symbol];
                SA[buckets[symbol].second - revoffset] = SA[i] - 1;
            }
        }
    }
    vector<int> tempNames(T.size(), -1);
    int name = 0;
    int prev = -1;

    for (int i = 0; i < SA.size(); i++) //Se realiza un recorrido para almacenar los nombres temporales en tempNames
    {
        string a = t[SA[i]];
        string b;
        if (SA[i] > 0)
        {
            b = t[SA[i] - 1];
        }
        else if (SA[i] == 0)
        {
            b = t[t.size() - 1];
        }

        if (a == "S" && b == "L") // Se le asigna un nombre a cada sufijo
        {
            if (prev != -1)
            {
                if (!findEqual(T, SA[prev], LMS[SA[prev]], SA[i], LMS[SA[i]]))
                {
                    name += 1;
                }
            }
            prev = i;
            tempNames[SA[i]] = name;
        }
    }
    vector<int> names;
    vector<int> SApIdx;

    for (int i = 0; i < T.size(); i++) // Se le asigna el nombre final y la posición a los vectores names y SApIdx
    {
        if (tempNames[i] != -1)
        {
            names.push_back(tempNames[i]);
            SApIdx.push_back(i);
        }
    }

    if (name < names.size() - 1) // Si hay sufijos con nombres, se llama a la función sais de manera recursiva para ordenar estos sufijos.
    {
        names = sais(names);
    }

    reverse(names.begin(), names.end()); // Se invierten los nombres

    SA.assign(T.size(), -1);
    count.clear();
    for (int i = 0; i < names.size(); i++) // Se compara con los cubetas en buckets
    {
        int pos = SApIdx[names[i]];
        count[T[pos]] = count[T[pos]] + 1;
        int revoffset = count[T[pos]];
        SA[buckets[T[pos]].second - revoffset] = pos;
    }

    count.clear();

    for (int i = 0; i < T.size(); i++) // Se recorre nuevamente el vector de forma inversa para ordenarlo 
    {
        if (SA[i] > 0)
        {
            if (t[SA[i] - 1] == "L")
            {
                int symbol = T[SA[i] - 1];
                int offset = count[symbol];
                SA[buckets[symbol].first + offset] = SA[i] - 1;
                count[symbol] = offset + 1;
            }
        }
    }
    count.clear();
    for (int i = T.size() - 1; i > 0; i--)
    {
        if (SA[i] > 0)
        {
            if (t[SA[i] - 1] == "S")
            {
                int symbol = T[SA[i] - 1];
                count[symbol] = count[symbol] + 1;
                int revoffset = count[symbol];
                SA[buckets[symbol].second - revoffset] = SA[i] - 1;
            }
        }
    }

    return SA;
}

int searchString(const string& text, const string& stringToSearch, const vector<int>& SA) { // Función para buscar algún string
    int left = 0;
    int right = text.length() - 1;
    int firstOccurrence = -1;

    while (left <= right) { // Ciclo que compara el string a buscar con los sufijos en SA
        int mid = (left + right) / 2; // Comienza en la mitad de los subarreglos SA
        string suffix = text.substr(SA[mid]);
        if (suffix.compare(0, stringToSearch.length(), stringToSearch) == 0) { //Si la búsqueda coincide, la primera ocurrencia es igual a mid y se continua buscando en mid -1 
            firstOccurrence = mid;
            right = mid - 1;  // Continuar buscando a la izquierda
        } else if (stringToSearch < suffix) { // se continúa buscando a la izquierda si el sufijo es patron es menor
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return firstOccurrence;
}

vector<int> allOccurrences(const string& text, const string& stringToSearch, const vector<int>& SA) { //Función para buscar todas las ocurrencias en el SA
    int firstOccurrence = searchString(text, stringToSearch, SA); // Llama a la función searchString para buscar la primera ocurrencia
    if (firstOccurrence == -1) { //Si no se encuentra ninguna ocurrencia, se almacena un -1 en el vector, indicando que no existen ocurrencias
        return vector<int>(1, -1);
    }
    int i = firstOccurrence;
    vector<int> occurrences;
    while (text.substr(SA[i]).compare(0, stringToSearch.length(), stringToSearch) == 0) { //Ciclo para encontrar todas las ocurrencias que se almacenan en la variable ocurrences
        occurrences.push_back(SA[i]);
        i++;
    }
    sort(occurrences.begin(), occurrences.end()); // Se ordenan las ocurrencias dentro del vector
    return occurrences;
}

int main() {
    int choice = 4;
    string name, searchOcurrence;
    
    while (choice < 0 || choice > 3) { // Ciclo para que el usuario elija un texto a examinar
        cout << "Elige un libro a examinar\nWings of the phoenix = 0\nThe Mysterious Stranger = 1\nA Modest Proposal = 2\nThe Yellow Wallpaper = 3\n" << endl;
        cin >> choice;
    }

    switch (choice) { // Dependiendo de la elección del usuario, se almacena el nombre del texto a examinar
        case 0:
            name = "books/wings_of_the_phoenix.txt";
            break;
        case 1:
            name = "books/the_mysterious_stranger.txt";
            break;
        case 2:
            name = "books/a_modest_proposal.txt";
            break;
        case 3:
            name = "books/the_yellow_wallpaper.txt";
            break;
    }

    cout << "Ingresa una palabra a buscar dentro del texto" << endl;
    cin >> searchOcurrence;

    ifstream file(name); // Se abre el texto
    ofstream output;

    string stringData;
    string line;
    while (getline(file, line)) { // Se guarda el texto en la variable stringData
        for (int i = 0; i < line.length(); i++) {
        char c = line.at(i);         // se obtiene un char del string
    
        // Si está en ese rango, es un caracter
        if (( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ) ) {
            stringData += c;
            }
        }
    }
    file.close();

    vector<int> T(stringData.begin(), stringData.end()); // Se almacena en el vector T con un caracter 0 al final para la lectura
    T.push_back(0);

    clock_t start = clock(); //Variable para tomar el tiempo
    vector<int> SA = sais(T);
    clock_t end = clock();//Se finaliza el tiempo 
    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC; // Se hace la conversión para el tiempo 
    
    output.open("output.txt");
    cout << "*****SAIS*****" << endl; // Se itera a trevés de SA y se imprime
    for (int i = 0; i < SA.size(); i++) {
      cout << SA[i] << " ";
        output <<  SA[i] << " ";
    }
    cout << endl;
    output << endl;

    vector<int> ocurrences = allOccurrences(stringData, searchOcurrence, SA); // Se almacenan las ocurrencias
  
    cout << "\nTiempo de ejecución: " << elapsed << " segundos" << endl; 
    output << "\nTiempo de ejecución: " << elapsed << " segundos" << endl;

    cout << "La primera ocurrencia de: " << searchOcurrence << " se encuentra en: " << searchString(stringData, searchOcurrence, SA) << endl;
    output << "La primera ocurrencia de: " << searchOcurrence << " se encuentra en: " << searchString(stringData, searchOcurrence, SA) << endl;

    cout << "Número total de ocurrencias: " << ocurrences.size() << endl;
    output << "Número total de ocurrencias: " << ocurrences.size() << endl;

    output.close();

  return 0;
}