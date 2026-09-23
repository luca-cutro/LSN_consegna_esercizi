#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "random.h"
#include "individual.h"

using namespace std;

Path ::Path() {}

Path::Path(int n_city) { // costruttore ordinato
    for(int i=0; i<n_city; i++) {
        vec_path.push_back(i);
    }
}

Path :: Path(int n_city, Random& r){  // costruttore casuale
    for(int i=0; i<n_city; i++) {
        vec_path.push_back(i);
    }

    for(int i=1; i<n_city; i++) {
        int j = int(r.Rannyu(1, n_city));
        swap(vec_path[i], vec_path[j]);
    }
}

Path :: ~Path() {}

int Path :: size(){
    return vec_path.size();
}

void Path :: swapath(Random& r){ // inverte l'oridne di 2 città
    int n = vec_path.size();
    int i = int(r.Rannyu(1, n));
    int j = int(r.Rannyu(1, n));

    if(i != j){
        swap(vec_path[i], vec_path[j]);
    }
}

void Path::shift(int n) { // shifta le città di n
    int s = vec_path.size() - 1;
    
    vector<int> v(s);

    for(int i = 0; i < s; i++) {
        v[i] = vec_path[i + 1];
    }

    vector<int> shifted(s);

    for(int i = 0; i < s; i++) {
        shifted[(i + n) % s] = v[i];
    }

    for(int i = 0; i < s; i++) {
        vec_path[i + 1] = shifted[i];
    }
}

void Path :: random_swap(int n, Random& r){ // scambia n volte le città
    for(int i=0; i<n; i++){
        swapath(r);
    }
}

double Path :: length_city(const City& C, const City& D) const { // distanza tra due città
    return sqrt(pow((C.x - D.x), 2) + pow((C.y - D.y), 2));
}

double Path :: length_path(const Citys& C) const { // lunghezza del percorso è la FUNZIONE COSTO
    double sum = 0;
    int n = vec_path.size();
    for(int i=0; i< n; i++){
        int id1 = vec_path[i];
        int id2 = vec_path[(i+1)%n];

        City A = C.get_city(id1);
        City B = C.get_city(id2);

        sum += length_city(A, B);      
    }

    return sum;
}

bool Path::check() { 
    int n = vec_path.size();

    if(vec_path[0] != 0) return false; // controlla che la prima città sia 0

    vector<int> count(n, 0);

    for(int i=0; i<n; i++) {
        if(vec_path[i] < 0 || vec_path[i] >= n) return false; 
        count[vec_path[i]]++;
    }

    for(int i=0; i<n; i++) {
        if(count[i] != 1) return false; // controlla che non ci siano ripetizioni tra le città
    }

    return true;
}

bool Path :: check_city(int k){ // vedo se la città k è nel path
    int n = vec_path.size();
    for(int i=0; i<n; i++){
        if(vec_path[i] == k){
            return true;
        }
    }
    return false;
}

void Path::permute_blocks(Random& r) {
    int n = size();

    // dimensione dei blocchi (almeno 1, massimo n/2)
    int dim = int(r.Rannyu(1, n/2));

    // primo blocco nella prima metà
    int start1 = int(r.Rannyu(1, n/2 - dim + 1));

    // secondo blocco nella seconda metà
    int start2 = int(r.Rannyu(n/2, n - dim));

    for(int k = 0; k < dim; k++) {
        int c1 = get_city(start1 + k);
        int c2 = get_city(start2 + k);
        set_city(start1 + k, c2);
        set_city(start2 + k, c1);
    }

    if(!check()) {
        cerr << "Errore: permutazione non valida" << endl;
        exit(1);
    }
}

void Path::invert_segment(Random& r) {
    int n = size();

    // lunghezza del segmento (almeno 2)
    int dim = int(r.Rannyu(2, n));

    // punto di partenza (non toccare la città 0)
    int start = int(r.Rannyu(1, n - dim + 1));
    int end = start + dim - 1;

    while(start < end) {
        int c1 = get_city(start);
        int c2 = get_city(end);
        set_city(start, c2);
        set_city(end, c1);
        start++;
        end--;
    }

    if(!check()) {
        cerr << "Errore: inversione non valida" << endl;
        exit(1);
    }
}


