#include <vector>
#include <cmath>
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
