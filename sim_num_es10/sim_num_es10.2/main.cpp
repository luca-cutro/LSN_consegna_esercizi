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

    int n_pop = 50;
    int n_gen = 100000;
    int n_migr = 50;

    Citys cities;
    cities.city_from_file("cap_prova_ita.dat");
    int n_city = cities.size();

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
                    int j = int(r.Rannyu(0, i+1));
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

    // SELEZIONE DEL MIGLIORE GLOBALE

    Path best = pop.best();
    double my_length = best.length_path(cities);

    std::vector<double> all_best;
    if (rank == 0) all_best.resize(size);

    MPI_Gather(&my_length, 1, MPI_DOUBLE,
               rank == 0 ? all_best.data() : nullptr, 1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    int best_rank = 0;
    double best_len = my_length;

    if (rank == 0) {
        best_len = all_best[0];
        for (int i = 1; i < size; i++) {
            if (all_best[i] < best_len) {
                best_len = all_best[i];
                best_rank = i;
            }
        }
    }

    MPI_Bcast(&best_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> my_path(n_city);
    for (int i = 0; i < n_city; i++)
        my_path[i] = best.get_city(i);

    std::vector<int> global_path(n_city);

    if (rank == best_rank) {
        MPI_Send(my_path.data(), n_city, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        MPI_Recv(global_path.data(), n_city, MPI_INT, best_rank, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        ofstream out("best_global_length.dat");
        out << best_len << endl;
        out.close();

        ofstream out_path("best_global_path.dat");
        for (int i = 0; i < n_city; i++) {
            int id = global_path[i];
            City c = cities.get_city(id);
            out_path << id << " " << c.x << " " << c.y << endl;
        }
        out_path << 0 << " "<<cities.get_city(0).x<<" "<<cities.get_city(0).y<<endl;
        out_path.close();
    }

    // GA SENZA MIGRAZIONE (ogni rank evolve da solo)

    ofstream fout_solo("fitness_solo_rank" + std::to_string(rank) + ".dat");

    // nuova popolazione indipendente
    Population pop_solo(n_pop, n_city, r);

    for(int igen = 0; igen < n_gen; igen++) {

        pop_solo.sort_by_fitness(cities);
        pop_solo.new_generation(cities, r);

        fout_solo << igen << " " << pop_solo.best_length(cities) << endl;
    }

    fout_solo.close();

    // salvo il percorso migliore di questo rank
    Path best_solo = pop_solo.best();

    ofstream out_solo("best_solo_rank" + std::to_string(rank) + ".dat");

    for(int i = 0; i < n_city; i++) {
        int id = best_solo.get_city(i);
        City c = cities.get_city(id);
        out_solo << id << " " << c.x << " " << c.y << endl;
    }

    // chiudo il ciclo
    int id0 = best_solo.get_city(0);
    City c0 = cities.get_city(id0);
    out_solo << id0 << " " << c0.x << " " << c0.y << endl;

    out_solo.close();

    MPI_Finalize();
    return 0;
}
