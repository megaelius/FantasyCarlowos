//ESPAÑA
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
        return preu < x.preu;
    }
};

time_t start = time(0);
string data_base;
string outputFile;
string requirementsFile;

void write_position(ofstream& out, const vector<Jugador>& vpos) {
    int n = vpos.size();
    for(int i = 0; i < n; ++i){
        if(i != 0) out << ';';
        out << vpos[i].nom;
    }
    out << endl;
}

void overwrite_solution (const vector<Jugador>& por,const vector<Jugador>& def,
                                    const vector<Jugador>& mig,
                                    const vector<Jugador>& dav,
                                    int Punts, int Preu) {
    ofstream out(outputFile);
    out.setf(ios::fixed);
    out.precision(1);

    time_t end = time(0);
    double time_taken = double(end - start);

    out << time_taken << endl;
    out << "POR: ";
    write_position(out,por);
    out << "DEF: ";
    write_position(out,def);
    out << "MIG: ";
    write_position(out,mig);
    out << "DAV: ";
    write_position(out,dav);
    out << "Punts: " << Punts << endl;
    out << "Preu: " << Preu << endl;

    out.close();
}


//devuelve el valor máximo de los puntos de los jugadores con precio
//inferior a J euros.
int database_reader (vector<Jugador>& por, vector<Jugador>& def,
                        vector<Jugador>& mig, vector<Jugador>& dav, int J) {

    ifstream in(data_base);
    int max_individual = -1;
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
        if (preu <= J) {
            if (punts > max_individual) max_individual = punts;
            if (posicio == "por") por.push_back(j);
            else if (posicio == "def") def.push_back(j);
            else if (posicio == "mig") mig.push_back(j);
            else dav.push_back(j);
        }
    }
    in.close();
    return max_individual;
}

void parameters (int& N1, int& N2, int& N3, int& T, int& J) {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
    in.close();
}

bool valid (const Jugador& j, const vector<Jugador>& v, int max_individual,
            int jug_queden, int Punts_actuals, const int& max_Punts){
    for(Jugador k : v) if(k == j) return false;
    return Punts_actuals + j.punts + (jug_queden-1)*max_individual > max_Punts;
}

void generate_exh(const vector<Jugador>& Vpor,const vector<Jugador>& Vdef,
                    const vector<Jugador>& Vmig,const vector<Jugador>& Vdav,
                    vector<Jugador>& por_sol, vector<Jugador>& def_sol,
                    vector<Jugador>& mig_sol, vector<Jugador>& dav_sol, int N0, int N1, int N2, int N3,
                    int T, int Punts, int Preu, int& max_Punts, int max_individual,
                    int idef, int imig, int idav) {

    if(N0 == 0 and N1 == 0 and N2 == 0 and N3 == 0) {
        if (Punts > max_Punts and Preu <= T) {
            max_Punts = Punts;
            overwrite_solution(por_sol,def_sol,mig_sol,dav_sol,Punts,Preu);
        }
    } else if (N0 > 0){
        int t = Vpor.size();
        for(int i = 0; i < t; ++i){
            Jugador P = Vpor[i];
            if (Preu+P.preu > T) return;
            else if(valid(P,por_sol,max_individual,N0+N1+N2+N3,Punts,max_Punts)) {
                por_sol.push_back(P);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0-1, N1, N2, N3, T,
                            Punts+P.punts, Preu+P.preu, max_Punts, max_individual,
                            idef,imig,idav);
                por_sol.pop_back();
            }
        }
    } else if (N1 > 0){
        int t = Vdef.size();
        for(int i = idef + 1; i < t; ++i){
            Jugador D = Vdef[i];
            if (Preu+D.preu > T) return;
            else if(valid(D,def_sol,max_individual,N0+N1+N2+N3,Punts,max_Punts)) {
                def_sol.push_back(D);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1-1, N2, N3, T,
                            Punts+D.punts, Preu+D.preu, max_Punts, max_individual,
                            i,imig,idav);
                def_sol.pop_back();
            }
        }
    } else if (N2 > 0){
        int t = Vmig.size();
        for(int i = imig + 1; i < t; ++i){
            Jugador M = Vmig[i];
            if (Preu+M.preu > T) return;
            else if(valid(M,mig_sol,max_individual,N0+N1+N2+N3,Punts,max_Punts)) {
                mig_sol.push_back(M);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1, N2-1, N3, T,
                            Punts+M.punts, Preu+M.preu, max_Punts, max_individual,
                            idef,i,idav);
                mig_sol.pop_back();
            }
        }
    } else {
        int t = Vdav.size();
        for(int i = idav + 1; i < t; ++i){
            Jugador D = Vdav[i];
            if (Preu+D.preu > T) return;
            else if(valid(D,dav_sol,max_individual,N0+N1+N2+N3,Punts,max_Punts)) {
                dav_sol.push_back(D);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1, N2, N3-1, T,
                            Punts+D.punts, Preu+D.preu, max_Punts, max_individual,
                            idef,imig,i);
                dav_sol.pop_back();
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Syntax: " << argv[0] << " data_base.txt"
        << " output_file.txt" << "requirements_file.txt" << endl;
        exit(1);
    }

    data_base = argv[1];
    outputFile = argv[3];
    requirementsFile = argv[2];

    int N1,N2,N3,T,J;

    parameters(N1,N2,N3,T,J);

    vector<Jugador> Vpor(0);
    vector<Jugador> Vdef(0);
    vector<Jugador> Vmig(0);
    vector<Jugador> Vdav(0);

    int max_individual = database_reader(Vpor,Vdef,Vmig,Vdav, J);
    sort(Vpor.begin(), Vpor.end());
    sort(Vdef.begin(), Vdef.end());
    sort(Vmig.begin(), Vmig.end());
    sort(Vdav.begin(), Vdav.end());

    vector<Jugador> por_sol;
    vector<Jugador> def_sol;
    vector<Jugador> mig_sol;
    vector<Jugador> dav_sol;

    int max_Punts = -1;
    generate_exh(Vpor, Vdef, Vmig, Vdav,
                 por_sol, def_sol, mig_sol, dav_sol,
                 1, N1, N2, N3, T, 0, 0, max_Punts, max_individual, 0,0,0);
}
