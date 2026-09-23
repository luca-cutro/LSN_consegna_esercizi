#ifndef __population__
#define __population__

#include <vector>
#include "random.h"
#include "individual.h"

using namespace std;

class Population {
private:
    vector<Path> vec_pop;

public:
    Population();
    Population(int n_pop, int n_city, Random& r);
    ~Population();

    int size() { return vec_pop.size();}
    Path get_path(int i){return vec_pop[i];}

    void sort_by_fitness(const Citys& cities);
    
    int select(Random& r, double p) const;

    Path best() const;
    double best_length(const Citys& cities) const;
    double average_best_half(const Citys& cities) const;

    Path half_crossover(Path& M, Path& F, Random& r);
    pair<Path, Path> crossover(Path& M, Path& F,Random& r);

    void new_generation(const Citys& cities, Random& r);
    
    bool check_population() const;
    void replace_worst(const Path& p);
};
#endif
