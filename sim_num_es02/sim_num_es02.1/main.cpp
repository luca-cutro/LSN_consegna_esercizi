#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "random.h"

using namespace std;

double error(vector<double>& AV, vector<double>& AV2, int n){
    if(n == 0){
        return 0.0;
    } else {
        return sqrt((AV2[n] - AV[n]*AV[n]) / n);
    }
}

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


    int N = 10000;
    int n=100;
    int M=N/n;

    vector<double> ave(M), ave2(M);
    vector<double> sum_prog(M), su2_prog(M), err_prog(M);

    // CAMPIONAMENTO UNIFORME
    for(int i=0; i<M; i++){
        double sum=0.0;
        for(int j=0; j<n; j++){
            double x = M_PI / 2.0 * cos(r.Rannyu(0.0, 1.0) * M_PI / 2);
            sum += x;
        }
        ave[i] = sum / n;
        ave2[i] = ave[i]*ave[i];
    }

    ofstream out("uniform.dat");
    for(int i=0; i<M; i++){ 
        for(int j = 0; j <= i; j++) {
            sum_prog[i] += ave[j];   
            su2_prog[i] += ave2[j];  
        }

        sum_prog[i] /= (i+1);   
        su2_prog[i] /= (i+1);  
        err_prog[i] = error(sum_prog, su2_prog, i);
        out<<sum_prog[i]<<" "<<err_prog[i]<<" "<<i<<endl;
    }
    cout<<"L'integrale e': "<<sum_prog[M-1]<<" con incertezza pari a "<<err_prog[M-1]<<endl;

    // IMPORTANCE SAMPLING
    for(int i = 0; i < M; i++){
        double sum = 0.0;
        for(int j = 0; j < n; j++){
            double x = 1.0 - sqrt(1.0 - r.Rannyu());   // p(x)=2(1-x)
            double f = M_PI / 2.0 * cos(x * M_PI / 2.0);
            double p = 2.0 * (1.0 - x);
            sum += f / p;
        }
        ave[i] = sum / n;
        ave2[i] = ave[i] * ave[i];
    }

    vector<double> sum_prog1(M), su2_prog1(M), err_prog1(M);
    ofstream outs("non_uniform.dat");
    for(int i=0; i<M; i++){ 
        for(int j = 0; j <= i; j++) {
            sum_prog1[i] += ave[j];   
            su2_prog1[i] += ave2[j];  
        }

        sum_prog1[i] /= (i+1);   
        su2_prog1[i] /= (i+1);  
        err_prog1[i] = error(sum_prog1, su2_prog1, i);
        outs<<sum_prog1[i]<<" "<<err_prog1[i]<<" "<<i<<endl;
    }
    cout<<"L'integrale e': "<<sum_prog1[M-1]<<" con incertezza pari a "<<err_prog1[M-1]<<endl;

}