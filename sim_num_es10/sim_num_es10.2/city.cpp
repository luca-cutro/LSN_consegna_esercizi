#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include "random.h"
#include "city.h"

using namespace std;

Citys :: Citys(){}
Citys :: ~Citys(){}

void Citys::city_cerchio(int n, Random& r) {
    vec_city.clear();

    for(int i=0; i<n; i++) {
        double teta = r.Rannyu(0, 2*M_PI);
        vec_city.push_back(City(cos(teta), sin(teta), i));
    }
}

void Citys::city_quadrato(int n, Random& r) {
    vec_city.clear();

    for(int i=0; i<n; i++) {
        vec_city.push_back(City(r.Rannyu(), r.Rannyu(), i));
    }
}

void Citys::city_from_file(const std::string& filename) {
    vec_city.clear();

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Errore: impossibile aprire " << filename << endl;
        exit(1);
    }
    double lat, lon;
    int i=0;
    while (file >> lat >> lon) {
        vec_city.push_back(City(lat, lon, i));
        i++;
    }

    file.close();
}
