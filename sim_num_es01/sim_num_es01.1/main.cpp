#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "random.h"

using namespace std;

double error(vector<double>& AV, vector<double>& AV2, int n) {
    if(n == 0){
        return 0.0;
    } else {
        return sqrt((AV2[n] - AV[n]*AV[n]) / n);
    }

}

int main(){
    int M=100000; // numero eventi
    int m=100; // numero blocchi
    int L;
    L=(int)M/m; // eventi per blocco
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

    // calcolo integrale e varianza
    vector<double> v(M, 0.0);
    vector<double> ave(m, 0.0);
    vector<double> av2(m, 0.0);
    vector<double> ave1(m, 0.0);
    vector<double> av21(m, 0.0);
    vector<double> sum_prog(m,0.0);
    vector<double> su2_prog(m,0.0);
    vector<double> err_prog(m,0.0);
    vector<double> sum_prog1(m,0.0);
    vector<double> su2_prog1(m,0.0);
    vector<double> err_prog1(m,0.0);


    for(int i=0; i < M; i++){
        v[i] = r.Rannyu();
    }

    for(int i=0; i<m; i++){
        double sum1=0;
        double sum2=0;
        double sum=0;
        for(int j=0; j<L; j++){
            int k=j+L*i;
            sum1 += v[k];
            sum2 += v[k]*v[k];
            sum += pow(v[k] - 0.5, 2);
        }
        ave[i]= sum1/L;
        av2[i]=ave[i]*ave[i];
        ave1[i]= sum/L;
        av21[i]=ave1[i]*ave1[i];
    }

    ofstream out("Integrale.dat");
    ofstream out1("Varianza.dat");
    for(int i = 0; i < m; i++) {

        for(int j = 0; j <= i; j++) {
            sum_prog[i] += ave[j];   
            su2_prog[i] += av2[j];
            sum_prog1[i] += ave1[j];   
            su2_prog1[i] += av21[j];   
        }

        sum_prog[i] /= (i+1);   
        su2_prog[i] /= (i+1);  
        err_prog[i] = error (sum_prog, su2_prog, i);
        out<<i<<" "<<sum_prog[i]<<" "<<err_prog[i]<<endl;
        sum_prog1[i] /= (i+1);   
        su2_prog1[i] /= (i+1);  
        err_prog1[i] = error (sum_prog1, su2_prog1, i);
        out1<<i<<" "<<sum_prog1[i]<<" "<<err_prog1[i]<<endl;
    }

    // calcolo chi2
    int P = 100;     // numero di intervalli (bins)
    int n = 10000;   // numeri generati per ogni test
    int k = 100;     // numero di test indipendenti

    int E = n/P;     // valore atteso per ogni intervallo


    vector<int> C(P, 0);    // conteggi per ogni intervallo

    ofstream chi("chi.dat");
    for(int i=0; i < k; i++){
        double sum = 0;
        for(int j=0; j<P; j++){
            C[j]=0;
        }

        for(int j=0; j<n; j++){
            C[int(P*r.Rannyu())]++;
        }

        for(int j=0; j<P; j++){
            sum = sum + pow(C[j] - E, 2);
        }
        double x = sum/E;
        chi<<i<<" "<<x<<endl;
    }

    return 0;
}