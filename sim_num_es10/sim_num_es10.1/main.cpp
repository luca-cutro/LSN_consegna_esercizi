#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "random.h"
#include "city.h"
#include "population.h"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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
    int n_gen = 1000;
    int n_migr = 100;

    Citys cities;
    cities.city_quadrato(n_city, r);

    Population pop(n_pop, n_city, r);

    ofstream fout("fitness_rank" + std::to_string(rank) + ".dat");

    for(int igen = 0; igen < n_gen; igen++) { 
        pop.sort_by_fitness(cities);
        if (igen != 0 && igen % n_migr == 0) {
            std::vector<int> order(size);
            if (rank == 0) {
                for (int i = 0; i < size; i++)
                    order[i] = i;
                for (int i = size - 1; i > 0; i--) {
                    int j = int(r.Rannyu() * (i + 1));
                    std::swap(order[i], order[j]);
                }
            }
            MPI_Bcast(order.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
            int pos = -1;
            for (int i = 0; i < size; i++)
                if (order[i] == rank)
                    pos = i;
            int receiver = order[(pos + 1) % size];           
            int sender   = order[(pos - 1 + size) % size];    

            Path best = pop.best();
            std::vector<int> send(n_city);
            for (int i = 0; i < n_city; i++)
                send[i] = best.get_city(i);

            std::vector<int> receive(n_city);
            MPI_Sendrecv(send.data(), n_city, MPI_INT, receiver, 0,
                        receive.data(), n_city, MPI_INT, sender,   0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Path migrant(n_city);
            for (int i = 0; i < n_city; i++)
                migrant.set_city(i, receive[i]);

            pop.replace_worst(migrant);
        }
        pop.new_generation(cities, r);
        fout << igen << " " << pop.best_length(cities) << endl;
    }

    Path best = pop.best();
    ofstream out("best_length_rank" + std::to_string(rank) + ".dat");
    out << best.length_path(cities) << endl;

    ofstream out_path("best_path_rank" + std::to_string(rank) + ".dat");
    for (int i = 0; i < n_city; i++)
        out_path << best.get_city(i) << endl;

    MPI_Finalize();

    return 0;
}