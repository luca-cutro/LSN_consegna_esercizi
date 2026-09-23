#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "gplot++.h"
#include "random.h"

using namespace std;

double error(const vector<double>& AV, const vector<double>& AV2, int n){
    if(n == 0){
        return 0.0;
    } else {
        return sqrt((AV2[n] - AV[n]*AV[n]) / n);
    }
}

int main(){
    Random ran;
    int seed[4] = {1, 2, 3, 4};
    int p1 = 289;
    int p2 = 978;
    ran.SetRandom(seed, p1, p2);

    double S0 = 100.0;
    double T = 1.0;
    double K = 100.0;
    double r = 0.1;
    double sigma = 0.25;

    int M = 10000;      // numero totale di random walks
    int N = 100;        // numero di passi
    int m = 100;        // numero di blocchi
    int L = M / m;      // walks per blocco

    vector<double> C_dir(M), C_dis(M);
    vector<double> P_dir(M), P_dis(M);

    int J = 100;
    double Delta = T / (double)J;

    for(int i = 0; i < M; i++){
        double Z = ran.Gauss(0, 1);
        double ST = S0 * exp((r - 0.5 * sigma * sigma) * T + sigma * Z * sqrt(T));

        vector<double> S(J);
        S [0] = S0;
        S[1] = S0 * exp((r - 0.5 * sigma * sigma) * Delta + sigma * Z * sqrt(Delta));
        for (int j = 2; j < J; j++){
            Z = ran.Gauss(0, 1);
            S[j] = S[j - 1] * exp((r - 0.5 * sigma * sigma) * Delta + sigma * Z * sqrt(Delta));
        }

        if(S[J-1] - K > 0){
            C_dis[i] = exp(- r * T) * (S[J-1] -K);
        } else {
            C_dis[i] = 0;
        }
        if(S[J-1] - K < 0){
            P_dis[i] = - exp(- r * T) * (S[J-1] -K);
        } else {
            P_dis[i] = 0;
        }

        if(ST - K > 0){
            C_dir[i] = exp(-r * T) * (ST - K);
        } else {
            C_dir[i] = 0;
        }
        if(ST - K < 0){
            P_dir[i] = - exp(-r * T) * (ST - K);
        } else {
            P_dir[i] = 0;
        }
    }

    vector<double> ave_C_dir(N, 0), ave2_C_dir(N, 0);
    vector<double> ave_C_dis(N, 0), ave2_C_dis(N, 0);
    vector<double> ave_P_dir(N, 0), ave2_P_dir(N, 0);
    vector<double> ave_P_dis(N, 0), ave2_P_dis(N, 0);

    for(int i = 0; i < N; i++){
        double sum_C_dir = 0;
        double sum_C_dis = 0;
        double sum_P_dir = 0;
        double sum_P_dis = 0;

        for(int j = 0; j < L; j++){
            int k = i * L + j;
            sum_C_dir += C_dir[k];
            sum_C_dis += C_dis[k];
            sum_P_dir += P_dir[k];
            sum_P_dis += P_dis[k];
        }

        ave_C_dir[i] = sum_C_dir / double(L);
        ave2_C_dir[i] = ave_C_dir[i] * ave_C_dir[i];
        ave_C_dis[i] = sum_C_dis / double(L);
        ave2_C_dis[i] = ave_C_dis[i] * ave_C_dis[i];
        ave_P_dir[i] = sum_P_dir / double(L);
        ave2_P_dir[i] = ave_P_dir[i] * ave_P_dir[i];
        ave_P_dis[i] = sum_P_dis / double(L);
        ave2_P_dis[i] = ave_P_dis[i] * ave_P_dis[i];
    }

    vector<double> sum_prog_C_dir(N, 0), su2_prog_C_dir(N, 0), err_prog_C_dir(N, 0);
    vector<double> sum_prog_C_dis(N, 0), su2_prog_C_dis(N, 0), err_prog_C_dis(N, 0);
    vector<double> sum_prog_P_dir(N, 0), su2_prog_P_dir(N, 0), err_prog_P_dir(N, 0);
    vector<double> sum_prog_P_dis(N, 0), su2_prog_P_dis(N, 0), err_prog_P_dis(N, 0);

    for(int i = 0; i < N; i++){
        for(int j = 0; j <= i; j++){
            sum_prog_C_dir[i] += ave_C_dir[j];
            su2_prog_C_dir[i] += ave2_C_dir[j];
            sum_prog_C_dis[i] += ave_C_dis[j];
            su2_prog_C_dis[i] += ave2_C_dis[j];

            sum_prog_P_dir[i] += ave_P_dir[j];
            su2_prog_P_dir[i] += ave2_P_dir[j];
            sum_prog_P_dis[i] += ave_P_dis[j];
            su2_prog_P_dis[i] += ave2_P_dis[j];
        }

        sum_prog_C_dir[i] /= double(i + 1);
        su2_prog_C_dir[i] /= double(i + 1);
        err_prog_C_dir[i] = error(sum_prog_C_dir, su2_prog_C_dir, i);
        sum_prog_C_dis[i] /= double(i + 1);
        su2_prog_C_dis[i] /= double(i + 1);
        err_prog_C_dis[i] = error(sum_prog_C_dis, su2_prog_C_dis, i);

        sum_prog_P_dir[i] /= double(i + 1);
        su2_prog_P_dir[i] /= double(i + 1);
        err_prog_P_dir[i] = error(sum_prog_P_dir, su2_prog_P_dir, i);
        sum_prog_P_dis[i] /= double(i + 1);
        su2_prog_P_dis[i] /= double(i + 1);
        err_prog_P_dis[i] = error(sum_prog_P_dis, su2_prog_P_dis, i);
    }

    vector<double> x(N), y_C_dir(N), y_C_dis(N), y_P_dir(N), y_P_dis(N), y_attC(N);
    ofstream outcd("Call_discrete.dat");
    ofstream outcc("Call_continuos.dat");
    ofstream outpd("Put_discrete.dat");
    ofstream outpc("Put_continousd.dat");
    for(int i = 0; i < N; i++){
        x[i] = i + 1;
        y_C_dir[i] = sum_prog_C_dir[i];
        y_C_dis[i] = sum_prog_C_dis[i];
        y_P_dir[i] = sum_prog_P_dir[i];
        y_P_dis[i] = sum_prog_P_dis[i];
        outcd<<i<<" "<<y_C_dis[i]<<" "<<err_prog_C_dis[i]<<endl;
        outcc<<i<<" "<<y_C_dir[i]<<" "<<err_prog_C_dir[i]<<endl;
        outpd<<i<<" "<<y_P_dis[i]<<" "<<err_prog_P_dis[i]<<endl;
        outpc<<i<<" "<<y_P_dir[i]<<" "<<err_prog_P_dir[i]<<endl;
        y_attC[i] = 14.975790778311286 - sum_prog_C_dir[i];
    }

    cout << "Prezzo stimato della call in modo diretto = " << y_C_dir[N-1] << " +/- " << err_prog_C_dir[N-1] << endl;
    cout << "Prezzo stimato della call in modo discreto = " << y_C_dis[N-1] << " +/- " << err_prog_C_dis[N-1] << endl;
    cout << "Prezzo stimato del put in modo diretto = " << y_P_dir[N-1] << " +/- " << err_prog_P_dir[N-1] << endl;
    cout << "Prezzo stimato del put in modo discreto = " << y_P_dis[N-1] << " +/- " << err_prog_P_dis[N-1] << endl;
    
    Gnuplot plot{};
    plot.set_xlabel("#blocks");
    plot.set_ylabel("Call price");
    plot.sendcommand("set grid");
   // plot.plot_yerr(x, y_C_dir, err_prog_C_dir, "European Call direct");
    /*plot.plot_yerr(x, y_C_dis, err_prog_C_dis, "European Call discretized");
    plot.plot_yerr(x, y_P_dir, err_prog_P_dir, "European Put direct");
    plot.plot_yerr(x, y_P_dis, err_prog_P_dis, "European Put discretized");*/
    plot.plot_yerr(x, y_attC, err_prog_C_dir, "");
    //quando fai il notebook metti anche il confronto con il valore atteso che trovo nel notebook 3.1 tipo 14.975790778311286
    plot.show();

    return 0;
}