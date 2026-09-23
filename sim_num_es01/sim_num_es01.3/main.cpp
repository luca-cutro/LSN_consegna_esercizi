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
    int M=10000; 
    int m=100; // numero blocchi
    int k;
    k=(int)M/m; //eventi per blocco
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

    double pi;
    vector<double> ave(m, 0.0);
    vector<double> av2(m, 0.0);
    vector<double> sum_prog(m,0.0);
    vector<double> su2_prog(m,0.0);
    vector<double> err_prog(m,0.0);

    double x;
    double y;
    double teta;

    double L = 0.7;
    double d = 1.0;

    for(int i=0; i<m; i++){
        double n=0;
        for(int j=0; j<k; j++){
            do{
                x = r.Rannyu();
                y = r.Rannyu();
            }while(x*x + y*y > 1);
            teta = atan(x/y);
            double pos = r.Rannyu();
            if(pos+L*sin(teta)>1){
                n++;
            }
        }
        pi = (2.0 * L * k) / (n * d);
        ave[i] = pi;
        av2[i] = pi * pi;   
    }

    ofstream out("Buffon.dat");
    for(int i=0; i<m; i++){
        for(int j = 0; j <= i; j++) {
            sum_prog[i] += ave[j];   
            su2_prog[i] += av2[j]; 
        }
        sum_prog[i] /= (i+1);
        su2_prog[i] /= (i+1);  
        err_prog[i] = error (sum_prog, su2_prog, i);
        out<<i<<" "<<sum_prog[i]<<" "<<err_prog[i]<<endl;
    }
    return 0;
}