#include <vector>
#include <iostream>
#include <cmath>
#include "random.h"
#include "individual.h"
#include "population.h"

using namespace std;

Population :: Population(int n_pop, int n_city, Random& r){ // costruisce popolazione con n_pop individui con n_city città
    vec_pop.clear();
    vec_pop.reserve(n_pop);

    for(int i = 0; i < n_pop; i++) {

        // 1. Costruisco percorso ordinato
        Path p(n_city);
        for(int j = 0; j < n_city; j++)
            p.set_city(j, j);

        for(int k = 0; k < 100; k++) {
            int a = int(r.Rannyu(1, n_city));   // da 1 a n_city-1
            int b = int(r.Rannyu(1, n_city));
            int ca = p.get_city(a);
            int cb = p.get_city(b);
            p.set_city(a, cb);
            p.set_city(b, ca);
        }
        vec_pop.push_back(p);
    }
}

Population :: ~Population(){}

void Population::sort_by_fitness(const Citys& cities) { //ordina popolazione dal percorso più lungo al più corto
    int n = vec_pop.size();

    for(int i=0; i<n-1; i++) {
        for(int j=i+1; j<n; j++) {
            double Li = vec_pop[i].length_path(cities);
            double Lj = vec_pop[j].length_path(cities);

            if(Lj < Li) {
                swap(vec_pop[i], vec_pop[j]);
            }
        }
    }
}

Path Population::best() const {
    return vec_pop[0];
}

double Population::best_length(const Citys& cities) const {
    return vec_pop[0].length_path(cities);
}

double Population::average_best_half(const Citys& cities) const { //lunghezza media metà migliore della popolazione
    double sum = 0.0;
    int half = vec_pop.size()/2;

    for(int i=0; i<half; i++) {
        sum += vec_pop[i].length_path(cities);
    }

    return sum / half;
}

int Population::select(Random& r, double p) const { // per selezionare genitori

    int M = vec_pop.size();

    double x = r.Rannyu();

    return int(M * pow(x, p));
}

Path Population :: half_crossover(Path& M, Path& F, Random& r) {
    int n = M.size();

    Path child(n);

    for(int i=0; i<n; i++) {
        child.set_city(i, -1);
    }

    child.set_city(0, 0);

    int cut = int(r.Rannyu(1, n));

    for(int i=1; i<cut; i++) {
        child.set_city(i, M.get_city(i));
    }

    int j = cut;

    for(int i=1; i<n; i++) {
        int city = F.get_city(i);

        if(!child.check_city(city)) {
            child.set_city(j, city);
            j++;
        }
    }

    return child;
}

pair<Path, Path> Population :: crossover(Path& M, Path& F,Random& r) {
    Path son1 = half_crossover(M, F, r);
    Path son2 = half_crossover(F, M, r);

    return {son1, son2};
}

void Population::new_generation(const Citys& cities, Random& r) {
    sort_by_fitness(cities);

    vector<Path> new_pop;
    new_pop.push_back(best());

    while(new_pop.size() < vec_pop.size()) {
        Path M = vec_pop[select(r, 2)];
        Path F = vec_pop[select(r, 2)];

        Path S1 = M;
        Path S2 = F;

        if(r.Rannyu() < 0.7) {
            pair<Path, Path> sons = crossover(M, F, r);
            S1 = sons.first;
            S2 = sons.second;
        }

        if(r.Rannyu() < 0.3) S1.swapath(r);
        if(r.Rannyu() < 0.3) S2.swapath(r);

        if(r.Rannyu() < 0.1) {
            int n = int(r.Rannyu(1,  110));
            S1.shift(n);
        }
        if(r.Rannyu() < 0.1) {
            int n = int(r.Rannyu(1, 110));
            S2.shift(n);
        }

        if(r.Rannyu() < 0.1) S1.permute_blocks(r);
        if(r.Rannyu() < 0.1) S2.permute_blocks(r);

        if(r.Rannyu() < 0.1) S1.invert_segment(r);
        if(r.Rannyu() < 0.1) S2.invert_segment(r);



        if(!S1.check() || !S2.check()) {
            cerr << "Errore: figlio non valido" << endl;
            exit(1);
        }

        new_pop.push_back(S1);

        if(new_pop.size() < vec_pop.size()) {
            new_pop.push_back(S2);
        }
    }

    vec_pop = new_pop;
}

void Population::replace_worst(const Path& p) {
    vec_pop.back() = p;
}
