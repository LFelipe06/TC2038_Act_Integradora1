#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

bool equalSubarrays(vector<int> &T, int start1, int end1, int start2, int end2)
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

map<int, pair<int, int>> getBuckets(vector<int> T)
{
    map<int, int> count;
    map<int, pair<int, int>> buckets;

    for (int c : T)
    {
        count[c]++;
    }

    int start = 0;
    vector<int> keys;
    for (auto countPair : count)
    {
        keys.push_back(countPair.first);
    }
    sort(keys.begin(), keys.end());

    for (auto c : keys)
    {
        buckets.insert({c, {start, start + count[c]}});
        start += count[c];
    }

    return buckets;
}

vector<int> sais(vector<int> T)
{
    vector<string> t(T.size(), "_");
    t[T.size() - 1] = "S";
    for (int i = T.size() - 1; i > 0; i--)
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
    for (int i = T.size() - 1; i > 0; i--)
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
    for (int i = 0; i < T.size(); i++)
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
    vector<int> namesp(T.size(), -1);
    int name = 0;
    int prev = -1;

    for (int i = 0; i < SA.size(); i++)
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

        if (a == "S" && b == "L")
        {
            if (prev != -1)
            {
                if (!equalSubarrays(T, SA[prev], LMS[SA[prev]], SA[i], LMS[SA[i]]))
                {
                    name += 1;
                }
            }
            prev = i;
            namesp[SA[i]] = name;
        }
    }
    vector<int> names;
    vector<int> SApIdx;

    for (int i = 0; i < T.size(); i++)
    {
        if (namesp[i] != -1)
        {
            names.push_back(namesp[i]);
            SApIdx.push_back(i);
        }
    }

    if (name < names.size() - 1)
    {
        names = sais(names);
    }

    reverse(names.begin(), names.end());

    SA.assign(T.size(), -1);
    count.clear();
    for (int i = 0; i < names.size(); i++)
    {
        int pos = SApIdx[names[i]];
        count[T[pos]] = count[T[pos]] + 1;
        int revoffset = count[T[pos]];
        SA[buckets[T[pos]].second - revoffset] = pos;
    }

    count.clear();

    for (int i = 0; i < T.size(); i++)
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

int search(const string& text, const string& pattern, const vector<int>& SA) {
    int left = 0;
    int right = text.length() - 1;
    int first_occurrence = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        string suffix = text.substr(SA[mid]);
        if (suffix.compare(0, pattern.length(), pattern) == 0) {
            first_occurrence = mid;
            right = mid - 1;  // Continuar buscando a la izquierda
        } else if (pattern < suffix) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return first_occurrence;
}

vector<int> allOccurrences(const string& text, const string& pattern, const vector<int>& SA) {
    int firstOccurrence = search(text, pattern, SA);
    if (firstOccurrence == -1) {
        return vector<int>(1, -1);
    }
    int i = firstOccurrence;
    vector<int> occurrences;
    while (text.substr(SA[i]).compare(0, pattern.length(), pattern) == 0) {
        occurrences.push_back(SA[i]);
        i++;
    }
    sort(occurrences.begin(), occurrences.end());
    return occurrences;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Es necesario ejecutar el código con el nombre del archivo a examinar al final" << std::endl;
        return 1;
    }

    const char* filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo '" << filename << "'" << std::endl;
        return 1;
    }

    std::string stringData;
    std::string line;
    while (std::getline(file, line)) {
        stringData += line;
    }
    file.close();

    std::vector<int> T(stringData.begin(), stringData.end());
    T.push_back(0);

    std::clock_t start = std::clock();
    vector<int> SA = sais(T);
    std::clock_t end = std::clock();
    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    map<int,pair<int,int>> buckets=getBuckets(T);
        std::cout << "KEY ELEMENT\n";
        for (auto itr = buckets.begin(); itr != buckets.end(); ++itr) {
            cout << itr->first << " " << itr->second.first<<" "<<itr->second.second<<"\n";
        cout<<endl;
        }
    
    cout << "*****SAIS*****" << endl;
    for (int i = 0; i < SA.size(); i++) {
      cout << SA[i] << " ";
    }

    cout << endl;

    vector<int> ocurrences = allOccurrences(stringData, "red", SA);
  
    std::cout << "\nTiempo de ejecución: " << elapsed << " segundos" << std::endl;

    cout << "Número de ocurrencias: " << ocurrences.size() << endl;

  return 0;
}