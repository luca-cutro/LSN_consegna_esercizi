#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "gplot++.h"
#include "random.h"

using namespace std;

int main(){
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

    int n = 10000;
    int N[4] = {1, 2, 10, 100};

    for(int j=0; j<4; j++){
        string nomefile = "dati" + to_string(N[j]) + ".dat";
        ofstream out(nomefile);
        for(int i=0; i < n; i++){
            double sum_uni = 0;
            double sum_exp = 0;
            double sum_lor = 0;
            for(int k=0; k<N[j]; k++){
                sum_uni += r.Rannyu();
                sum_exp += r.Exponential(1.0);
                sum_lor += r.Lorentz(0.0, 1.0);
            }
            out<<sum_uni/N[j]<<" "<<sum_exp/N[j]<<" "<<sum_lor/N[j]<<endl;
        }
    }

    return 0;
}