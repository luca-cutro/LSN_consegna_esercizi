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

double nabla2_psi(double x, double sigma, double mu){ //???
    return pow(M_E, - 0.5 * pow(x- mu, 2) / pow(sigma, 2)) * (pow(x-mu, 2) / pow(sigma, 4) - 1 / pow(sigma, 2)) + pow(M_E, - 0.5 * pow(x + mu, 2) / pow(sigma, 2)) * (pow(x+mu, 2) / pow(sigma, 4) - 1 / pow(sigma, 2));
}
double E_loc(double x, double sigma, double mu){
    return - 0.5 * nabla2_psi(x, sigma, mu) / psi(x, sigma, mu) + pow(x, 4) - 2.5 * pow(x, 2);
}

struct result {
    double energy;

    double error;
};

result VMC(Random &r, double sigma, double mu, int N, bool save_data) {
    int n=1000;
    int M=N/n;

    double delta = 1;

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

    ofstream pos;
    if(save_data)
        pos.open("positions.dat");
    for(int i=0; i<M; i++){
        double sum=0.0;
        for(int j=0; j<n; j++){
            double x_new = x + r.Rannyu(-delta, delta);
            double A = min(1.0 , psi2(x_new, sigma, mu) / psi2(x, sigma, mu));
            if(r.Rannyu(0, 1) < A){
                x=x_new;
            }
            if(save_data)
                pos << x << endl;
            sum += E_loc(x, sigma, mu);
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
    }

    if(save_data) {
    ofstream out("final_energy.dat");

    for(int i = 0; i < M; i++) {
        out << i+1 << " "
            << sum_prog[i] << " "
            << err_prog[i] << endl;
    }
    }

    result res;
    res.energy = sum_prog[M-1];
    res.error = err_prog[M-1];
    return res;
}

int main() {
    Random r;
    int seed[4] = {1,2,3,4};
    int p1 = 289;
    int p2 = 977;
    r.SetRandom(seed,p1,p2);

    int N = 100000;

    double sigma = 0.8;
    double mu = 0.8;
    double T = 1;
    double Tmin = 0.01;
    result old = VMC(r, sigma, mu, N, false);

    double sigma_best = sigma;
    double mu_best = mu;
    result best = old;

    ofstream SA("SA.dat");
    while(T > Tmin){
        double sigma_new = sigma + r.Rannyu(-0.05, 0.05);
        double mu_new = mu + r.Rannyu(-0.05, 0.05);
        if(sigma_new <= 0.0)
            continue;
        result trial = VMC(r, sigma_new, mu_new, N, false);
        if(old.energy > trial.energy){
            old = trial;
            sigma = sigma_new;
            mu = mu_new;
        } else {
            if(exp(-(trial.energy - old.energy)/T) > r.Rannyu()){ //old può peggiorare
                old = trial;
                sigma = sigma_new;
                mu = mu_new;
            }
        }

        if(old.energy < best.energy){ //dato che old può peggiorare
            best = old;
            sigma_best = sigma;
            mu_best = mu;
        }

        SA << T << " "
           << sigma << " "
           << mu << " "
           << old.energy << " "
           << old.error << endl;
        T *= 0.98;
    }
    cout << "Best sigma = " << sigma_best << endl;
    cout << "Best mu = " << mu_best << endl;
    cout << "Best energy = " << best.energy << endl;
    VMC(r, sigma_best, mu_best, N, true);
    return 0;
}