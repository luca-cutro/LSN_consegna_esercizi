#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "gplot++.h"
#include "random.h"

using namespace std;

double error(vector<double>& AV, vector<double>& AV2, int n){
    if(n == 0){
        return 0.0;
    } else {
        return sqrt((AV2[n] - AV[n]*AV[n]) / n);
    }
}

double length2(double x, double y, double z){
    double l = x*x + y*y + z*z;
    return l;
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

    int M = 10000;      // numero totale di random walks
    int N = 100;        // numero di passi
    int m = 100;        // numero di blocchi
    int L = M / m;      // walks per blocco

    vector<vector<double>> block_latt(m, vector<double>(N+1, 0.0));
    vector<vector<double>> block_cont(m, vector<double>(N+1, 0.0));

    for(int iblk=0; iblk<m; iblk++){

        vector<double> sum_r2_latt(N+1, 0.0);
        vector<double> sum_r2_cont(N+1, 0.0);

        for(int iwalk=0; iwalk<L; iwalk++){

            // posizione reticolo
            double x_l=0.0, y_l=0.0, z_l=0.0;

            // posizione continuo
            double x_c=0.0, y_c=0.0, z_c=0.0;

            for(int istep=1; istep<=N; istep++){

                // RETICOLO
                int dir = int(r.Rannyu(0,6));

                if(dir == 0) x_l += 1.0;
                else if(dir == 1) x_l -= 1.0;
                else if(dir == 2) y_l += 1.0;
                else if(dir == 3) y_l -= 1.0;
                else if(dir == 4) z_l += 1.0;
                else if(dir == 5) z_l -= 1.0;

                sum_r2_latt[istep] += length2(x_l,y_l,z_l);

                // CONTINUO
                double phi = r.Rannyu(0.0, 2.0*M_PI);
                double costheta = r.Rannyu(-1.0, 1.0);
                double sintheta = sqrt(1.0 - costheta*costheta);

                x_c += sintheta * cos(phi);
                y_c += sintheta * sin(phi);
                z_c += costheta;

                sum_r2_cont[istep] += length2(x_c,y_c,z_c);
            }
        }

        for(int istep=0; istep<=N; istep++){
            block_latt[iblk][istep] = sqrt(sum_r2_latt[istep] / double(L));
            block_cont[iblk][istep] = sqrt(sum_r2_cont[istep] / double(L));
        }
    }

    vector<double> x_plot(N+1, 0.0);
    vector<double> y_latt(N+1, 0.0);
    vector<double> err_latt(N+1, 0.0);
    vector<double> y_cont(N+1, 0.0);
    vector<double> err_cont(N+1, 0.0);


    ofstream out1("continuous.dat");
    ofstream out2("lattice.dat");

    for(int istep = 0; istep <= N; istep++){

        vector<double> ave_latt(m,0.0), ave2_latt(m,0.0);
        vector<double> sum_prog_latt(m,0.0), su2_prog_latt(m,0.0), err_prog_latt(m,0.0);

        vector<double> ave_cont(m,0.0), ave2_cont(m,0.0);
        vector<double> sum_prog_cont(m,0.0), su2_prog_cont(m,0.0), err_prog_cont(m,0.0);

        for(int iblk = 0; iblk < m; iblk++){
            ave_latt[iblk]  = block_latt[iblk][istep];
            ave2_latt[iblk] = ave_latt[iblk] * ave_latt[iblk];

            ave_cont[iblk]  = block_cont[iblk][istep];
            ave2_cont[iblk] = ave_cont[iblk] * ave_cont[iblk];
        }

        for(int iblk = 0; iblk < m; iblk++){
            for(int j = 0; j <= iblk; j++){
                sum_prog_latt[iblk] += ave_latt[j];
                su2_prog_latt[iblk] += ave2_latt[j];

                sum_prog_cont[iblk] += ave_cont[j];
                su2_prog_cont[iblk] += ave2_cont[j];
            }

            sum_prog_latt[iblk] /= double(iblk + 1);
            su2_prog_latt[iblk] /= double(iblk + 1);
            err_prog_latt[iblk] = error(sum_prog_latt, su2_prog_latt, iblk);

            sum_prog_cont[iblk] /= double(iblk + 1);
            su2_prog_cont[iblk] /= double(iblk + 1);
            err_prog_cont[iblk] = error(sum_prog_cont, su2_prog_cont, iblk);
        }

        x_plot[istep]   = istep;
        y_latt[istep]   = sum_prog_latt[m-1];
        err_latt[istep] = err_prog_latt[m-1];
        y_cont[istep]   = sum_prog_cont[m-1];
        err_cont[istep] = err_prog_cont[m-1];

        out1<<istep<<" "<<y_cont[istep]<<" "<<err_cont[istep]<<endl;
        out2<<istep<<" "<<y_latt[istep]<<" "<<err_latt[istep]<<endl;
    }
    
    return 0;
}