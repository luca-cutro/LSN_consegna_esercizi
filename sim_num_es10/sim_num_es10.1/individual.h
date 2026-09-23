#ifndef __individual__
#define __individual__

#include <vector>
#include "random.h"
#include "city.h"
using namespace std;

class Path {
private:
    vector<int> vec_path;

public:
    Path();
    Path(int n_city);
    Path(int n_city, Random& r);
    ~Path();

    int size();
    int get_city(int i){return vec_path[i];}
    void set_city(int i, int x){vec_path[i] = x;}

    void swapath(Random& r);
    void shift(int n);
    void random_swap(int n, Random& r); //permutazione
    
    bool check_city(int k);
    bool check();

    double length_city(const City& C, const City& D) const;
    double length_path(const Citys& C) const;
};

#endif