#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "random.h"

using namespace std;

double error(vector<double>& AV, vector<double>& AV2, int n){
    if(n == 0){
        return 0.0;
    } else {
        return sqrt((AV2[n] - AV[n]*AV[n]) / n);
    }
}

double psi(double x, double sigma, double mu){
    return exp(-pow(x - mu, 2) / (2 * pow(sigma, 2))) + exp(-pow(x + mu, 2) / (2 * pow(sigma, 2)));
}

double psi2(double x, double sigma, double mu){
    return pow(psi(x, sigma, mu), 2);
}

double nabla2_psi(double x, double sigma, double mu){
    return pow(M_E, - 0.5 * pow(x- mu, 2) / pow(sigma, 2)) * (pow(x-mu, 2) / pow(sigma, 4) - 1 / pow(sigma, 2)) + pow(M_E, - 0.5 * pow(x + mu, 2) / pow(sigma, 2)) * (pow(x+mu, 2) / pow(sigma, 4) - 1 / pow(sigma, 2));
}
double E_loc(double x, double sigma, double mu){
    return - 0.5 * nabla2_psi(x, sigma, mu) / psi(x, sigma, mu) + pow(x, 4) - 2.5 * pow(x, 2);
}
int main() {
    Random r;
    int seed[4] = {1,2,3,4};
    int p1 = 289;
    int p2 = 977;
    r.SetRandom(seed,p1,p2);

    int N = 100000;
    int n=100;
    int M=N/n;
    //double sigma = 0.6;
    //double mu = 0.8;

    double delta = 1;

    double Emin = 1e10;
    double sigma_best;
    double mu_best;

    ofstream out("energy.dat");
    //ofstream pos("positions.dat");
    ofstream scan("scan.dat");
    for(double sigma = 0.5; sigma <= 1.0; sigma += 0.05){
        for(double mu = 0.5; mu <= 1.0; mu += 0.05){
            vector<double> ave(M), ave2(M);
            vector<double> sum_prog(M), su2_prog(M), err_prog(M);
            double x = 0.0;

            //equilibrazione
            int N_eq = 5000;
            for(int i=0; i<N_eq; i++){
                double x_new = x + r.Rannyu(-delta, delta);
                double A = min(1.0, psi2(x_new,sigma,mu)/psi2(x,sigma,mu));
                if(r.Rannyu() < A){
                    x = x_new;
                }
            }

            for(int i=0; i<M; i++){
                double sum=0.0;
                for(int j=0; j<n; j++){
                    double x_new = x + r.Rannyu(-delta, delta);
                    double A = min(1.0 , psi2(x_new, sigma, mu) / psi2(x, sigma, mu));
                    if(r.Rannyu(0, 1) < A){
                        x = x_new;
                    }
                    sum += E_loc(x, sigma, mu);
                   // pos << x << endl;
                }
                ave[i] = sum / n;
                ave2[i] = ave[i]*ave[i];
            }
            for(int i=0; i<M; i++){
                for(int j = 0; j <= i; j++) {
                    sum_prog[i] += ave[j];   
                    su2_prog[i] += ave2[j];  
                }
            
                sum_prog[i] /= (i+1);   
                su2_prog[i] /= (i+1);  
                err_prog[i] = error(sum_prog, su2_prog, i);
                out << sigma << " "
                    << mu << " "
                    << i+1 << " "
                    << sum_prog[i] << " "
                    << err_prog[i] << endl;
            }
            scan << sigma << " "
                 << mu << " "
                 << sum_prog[M-1]
                 << endl;

            double E = sum_prog[M-1];
            if(E < Emin){
                Emin = E;
                sigma_best = sigma;
                mu_best = mu;
            }
        }
    }

    cout << "Minimum energy = " << Emin << endl;
    cout << "sigma = " << sigma_best << endl;
    cout << "mu = " << mu_best << endl;
    return 0;
}