#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include "city.h"
#include "population.h"

using namespace std;

int main() {
    Random r;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
    ifstream input("seed.in");
    string property;
    if (input.is_open()){
        while ( !input.eof() ){
            input >> property;
            if( property == "RANDOMSEED" ){
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                r.SetRandom(seed,p1,p2);
            }
        }
        input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;

    int n_city = 34;
    int n_pop = 500;
    int n_gen = 600;


    //  CERCHIO

    Citys cities;
    cities.city_cerchio(n_city, r);

    Population pop(n_pop, n_city, r);

    ofstream fout("fitness_circle.dat");

    for(int igen = 0; igen < n_gen; igen++) {

        pop.sort_by_fitness(cities);

        fout << igen << " "
             << pop.best_length(cities) << " "
             << pop.average_best_half(cities)
             << endl;

        cout << "Circle - Generation = " << igen
             << " Best length = "
             << pop.best_length(cities)
             << endl;

        pop.new_generation(cities, r);
    }

    fout.close();


    Path best = pop.best();

    ofstream bestout("best_path_circle.dat");

    for(int i = 0; i < n_city; i++) {

        City c = cities.get_city(best.get_city(i));

        bestout << c.x << " "
                << c.y
                << endl;
    }

    City c0 = cities.get_city(best.get_city(0));

    bestout << c0.x << " "
            << c0.y
            << endl;

    bestout.close();



    // QUADRATO

    Citys square;

    square.city_quadrato(n_city, r);

    Population pop_square(n_pop, n_city, r);

    ofstream fout2("fitness_square.dat");

    for(int igen = 0; igen < n_gen; igen++) {

        pop_square.sort_by_fitness(square);

        fout2 << igen << " "
              << pop_square.best_length(square) << " "
              << pop_square.average_best_half(square)
              << endl;


        cout << "Square - Generation = " << igen
             << " Best length = "
             << pop_square.best_length(square)
             << endl;


        pop_square.new_generation(square, r);
    }


    fout2.close();


    Path best_square = pop_square.best();


    ofstream bestout2("best_path_square.dat");


    for(int i = 0; i < n_city; i++) {

        City c = square.get_city(best_square.get_city(i));

        bestout2 << c.x << " "
                 << c.y
                 << endl;
    }


    City c1 = square.get_city(best_square.get_city(0));

    bestout2 << c1.x << " "
             << c1.y
             << endl;


    bestout2.close();


    r.SaveSeed();

    return 0;
}