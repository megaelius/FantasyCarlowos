#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <time.h>
#include <algorithm>
using namespace std;

struct Jugador{
    string nom;
    string posicio;
    string club;
    int punts;
    int preu;

    bool operator==(const Jugador x) {
        return nom == x.nom and posicio == x.posicio and
               club == x.club and punts == x.punts and preu == x.preu;
    }

    bool operator<(const Jugador x) const{
        return -punts < -x.punts;
    }
};

clock_t start = clock();
string data_base;
string outputFile;
string requirementsFile;
int N1, N2, N3, T, J;
int n0 = 0, n1 = 0, n2 = 0, n3 = 0;
int max_Punts = -1;
vector<Jugador> Vplayers;
vector<Jugador> por_sol, def_sol, mig_sol, dav_sol;


void write_position(ofstream& out, const vector<Jugador>& sol) {
    for(int i = 0; i < sol.size(); ++i){
        if(i != 0) out << ';';
        out << sol[i].nom;
    }
    out << endl;
}

void overwrite_solution (int Punts, int Preu) {
    ofstream out(outputFile);
    out.setf(ios::fixed);
    out.precision(1);

    clock_t end = clock();
    double time_taken = double(end - start)/CLOCKS_PER_SEC;

    out << time_taken << endl;
    out << "POR: ";
    write_position(out,por_sol);
    out << "DEF: ";
    write_position(out,def_sol);
    out << "MIG: ";
    write_position(out,mig_sol);
    out << "DAV: ";
    write_position(out,dav_sol);
    out << "Punts: " << Punts << endl;
    out << "Preu: " << Preu << endl;

    out.close();
}


void database_reader () {
    ifstream in(data_base);
    Jugador j;
    while (not in.eof()) {
        string nom, posicio, club;
        int punts, preu;
        getline(in,nom,';');    if (nom == "") break;
        getline(in,posicio,';');
        in >> preu;
        char aux; in >> aux;
        getline(in,club,';');
        in >> punts;
        string aux2;
        getline(in,aux2);
        j.nom = nom;
        j.posicio = posicio;
        j.club = club;
        j.punts = punts;
        j.preu = preu;
        if (preu <= J) Vplayers.push_back(j);
    }
    in.close();
}

void parameters () {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
    in.close();
}

void generate_exh (int i, int Punts, int Preu, int Team) {
    if (Team == 11) {
        if (Punts > max_Punts) {
            max_Punts = Punts;
            overwrite_solution(Punts, Preu);
        }
    }
    if (i >= Vplayers.size()) return;
    Jugador j = Vplayers[i];
    if (Punts + j.punts*(11-Team) > max_Punts) {//es el tio con mas puntos que puedes añadir
        if (Preu + j.preu <= T) {
            if (j.posicio == "por") {
                if (n0 < 1) {
                    cout << j.nom << endl;
                    por_sol[n0] = j; ++n0;
                    generate_exh(i+1, Punts+j.punts, Preu+j.preu, Team+1);
                    --n0;
                }
            } else if (j.posicio == "def") {
                if (n1 < N1) {
                    def_sol[n1] = j; ++n1;
                    generate_exh(i+1, Punts+j.punts, Preu+j.preu, Team+1);
                    --n1;
                }
            } else if (j.posicio == "mig") {
                if (n2 < N2) {
                    mig_sol[n2] = j; ++n2;
                    generate_exh(i+1, Punts+j.punts, Preu+j.preu, Team+1);
                    --n2;
                }
            } else if (j.posicio == "dav") {
                if (n3 < N3) {
                    dav_sol[n3] = j; ++n3;
                    generate_exh(i+1, Punts+j.punts, Preu+j.preu, Team+1);
                    --n3;
                }
            }
        }
    } else return;
    generate_exh(i+1, Punts, Preu, Team);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt"
        << " requirements_file.txt" << "output_file.txt" << endl;
        exit(1);
    }

    data_base = argv[1];
    outputFile = argv[3];
    requirementsFile = argv[2];

    parameters();

    database_reader();

    sort(Vplayers.begin(), Vplayers.end());

    por_sol = vector<Jugador> (1);
    def_sol = vector<Jugador> (N1);
    mig_sol = vector<Jugador> (N2);
    dav_sol = vector<Jugador> (N3);
    generate_exh(0, 0, 0, 0);
}
