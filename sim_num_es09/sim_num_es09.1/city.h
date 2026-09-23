#ifndef __city__
#define __city__

#include <vector>
#include "random.h"

using namespace std;

struct City{
    double x, y;
    int id;
    City(double x_, double y_, int id_) : x(x_), y(y_), id(id_) {}
};

class Citys{
private:
vector<City> vec_city;

public:
Citys();
~Citys();

City get_city(int i) const { return vec_city[i]; }

int size() const { return vec_city.size(); }

vector<City> get_cities() const { return vec_city; }

void city_cerchio(int n, Random& r);
void city_quadrato(int n, Random& r);
};

#endif