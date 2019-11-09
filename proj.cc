//ESPAÑA
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <ctime>
using namespace std;

struct Jugador{
    string nom, posicio, club;
    int punts, preu;
};

time_t start;
time(&start);
string data_base;
string outputFile;
string requirementsFile;

void write_position(ofstream& out, const vector<Player>& vpos) {
    int n = vpos.size();
    for(int i = 0; i < n; ++i){
        if(i != 0) out << ';'
        out << vpos[i].nom
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

    time_t end;
    time(&end)
    double time_taken = double(end - start);

    out << time_taken << endl;
    out << "POR: ";
    write_position(por);
    out << "DEF: ";
    write_position(def);
    out << "MIG: ";
    write_position(mig);
    out << "DAV: ";
    write_position(dav);
    out << "Punts: " << Punts << endl;
    out << "Preu: " << Preu << endl;

    out.close();
}

void database_reader (vector<Jugador>& por, vector<Jugador>& def,
                        vector<Jugador>& mig, vector<Jugador>& dav) {
    ifstream in(data_base);

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

        if (pos == "por") por.push_back(Jugador{nom, pos, club, punts, preu});
        else if (pos == "def") def.push_back(Jugador{nom, pos, club, punts, preu});
        else if (pos == "mig") mig.push_back(Jugador{nom, pos, club, punts, preu});
        else dav.push_back(Jugador{nom, pos, club, punts, preu});
    }

    in.close();
    }

void parameters (int& N1, int& N2, int& N3, int& T, int& J) {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
}

bool valid (Const Jugador& j, const vector<Jugador>& v, int J){
    for(Jugador k : v) if(k == j)return false;
    return j.preu <= J;
}

void generate_exh(const vector<Jugador>& Vpor,const vector<Jugador>& Vdef,
                    const vector<Jugador>& Vmig,const vector<Jugador>& Vdav,
                    vector<Jugador>& por_sol, vector<Jugador>& mig_sol,
                    vector<Jugador>& dav_sol, int N0, int N1, int N2, int N3,
                    int T, int J, int Punts, int Preu, int max_Punts) {

    if(N0 == N1 == N2 == N3 == 0) {
        if (Punts > max_Punts and Preu <= T) {
            overwrite_solution(por_sol,def_sol,mig_sol,dav_sol,Punts,Preu);
        }
    } else if(N0 > 0){
        for(Jugador P : Vpor){
            if(valid(P,por_sol,J)) {
                por_sol.push_back(P);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0-1, N1, N2, N3, T, J,
                            Punts+P.punts, Preu+P.preu, max_Punts);
                por_sol.pop_back();
            }
        }
    } else if (N1 > 0){
        for(Jugador D : Vdef){
            if(valid(D,def_sol,J)) {
                def_sol.push_back(D);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1-1, N2, N3, T, J,
                            Punts+D.punts, Preu+D.preu, max_Punts);
                def_sol.pop_back();
            }
        }
    } else if(N2 > 0){
        for(Jugador M : Vmig){
            if(valid(M,mig_sol,J)) {
                mig_sol.push_back(M);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1, N2-1, N3, T, J,
                            Punts+M.punts, Preu+M.preu, max_Punts);
                mig_sol.pop_back();
            }
        }
    } else {
        for(Jugador D : Vdav){
            if(valid(D,dav_sol,J)) {
                dav_sol.push_back(D);
                generate_exh(Vpor, Vdef, Vmig, Vdav,
                            por_sol, def_sol, mig_sol, dav_sol,
                            N0, N1, N2, N3-1, T, J,
                            Punts+D.punts, Preu+D.preu, max_Punts);
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
    outputFile = argv[2];
    requirementsFile = argv[3];

    vector<Jugador> Vpor;
    vector<Jugador> Vdef;
    vector<Jugador> Vmig;
    vector<Jugador> Vdav;

    database_reader(Vpor,Vdef,Vmig,Vdav);

    int N1,N2,N3,T,J;

    parameters(N1,N2,N3,T,J);

    vector<Jugador> por_sol;
    vector<Jugador> def_sol;
    vector<Jugador> mig_sol;
    vector<Jugador> dav_sol;

    generate_exh(Vpor, Vdef, Vmig, Vdav,
                 por_sol, def_sol, mig_sol, dav_sol,
                 1, N1, N2, N3, T, J, 0, 0, 0);
}
