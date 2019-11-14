#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <time.h>
#include <algorithm>
using namespace std;

struct Player{
    string name;
    string position;
    string club;
    int points;
    int price;

    bool operator==(const Player x) {
        return name == x.name and position == x.position and
               club == x.club and points == x.points and price == x.price;
    }

    bool operator<(const Player x) const{
        return -points < -x.points;
    }
};

clock_t start = clock();
string data_base;
string outputFile;
string requirementsFile;
int N1, N2, N3, T, J;
int n0 = 0, n1 = 0, n2 = 0, n3 = 0;
int max_Points = -1;
vector<Player> Vplayers;
vector<Player> por_sol, def_sol, mig_sol, dav_sol;


void write_position(ofstream& out, const vector<Player>& sol) {
    for(int i = 0; i < sol.size(); ++i){
        if(i != 0) out << ';';
        out << sol[i].name;
    }
    out << endl;
}

void overwrite_solution (int Points, int Price) {
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
    out << "Points: " << Points << endl;
    out << "Price: " << Price << endl;

    out.close();
}


void database_reader () {
    ifstream in(data_base);
    Player j;
    while (not in.eof()) {
        string name, position, club;
        int points, price;
        getline(in,name,';');    if (name == "") break;
        getline(in,position,';');
        in >> price;
        char aux; in >> aux;
        getline(in,club,';');
        in >> points;
        string aux2;
        getline(in,aux2);
        j.name = name;
        j.position = position;
        j.club = club;
        j.points = points;
        j.price = price;
        if (price <= J) Vplayers.push_back(j);
    }
    in.close();
}

void parameters () {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
    in.close();
}

bool usat(const vector<Jugador>& v, const Jugador& P){
    for(Jugador J : v){
        if(J == P)return true;
    }
    return false;
}

int cota_pos(const vector<Jugador>& v_sol,int N, string pos,int i){
    int cota = 0;
    int j = 0;
    while(i < N and j < Vplayers.size()){
        if(Vplayers[j].posicio == pos and not usat(v_sol,Vplayers[j])){
            ++i;
            cota += Vplayers[j].punts;
        }
        ++j;
    }
    return cota;
}

bool valid2(int Punts, int i){
    int cota = 0;
    cota += cota_pos(por_sol,1-n0,"por",i);
    cota += cota_pos(def_sol,N1-n1,"def",i);
    cota += cota_pos(mig_sol,N2-n2,"mig",i);
    cota += cota_pos(dav_sol,N3-n3,"dav",i);
    if(Punts + cota < max_Punts)return false;
    return true;
}

void generate_exh (int i, int Punts, int Preu, int Team) {
    if (Team == 11) {
        if (Points > max_Points) {
            max_Points = Points;
            overwrite_solution(Points, Price);
        }
    }
    if (i >= Vplayers.size()) return;
    Player j = Vplayers[i];
    if (valid2(Punts,i)) {//es el tio con mas puntos que puedes añadir
        if (Price + j.price <= T) {
            if (j.position == "por") {
                if (n0 < 1) {
                    por_sol[n0] = j; ++n0;
                    generate_exh(i+1, Points+j.points, Price+j.price, Team+1);
                    --n0;
                }
            } else if (j.position == "def") {
                if (n1 < N1) {
                    def_sol[n1] = j; ++n1;
                    generate_exh(i+1, Points+j.points, Price+j.price, Team+1);
                    --n1;
                }
            } else if (j.position == "mig") {
                if (n2 < N2) {
                    mig_sol[n2] = j; ++n2;
                    generate_exh(i+1, Points+j.points, Price+j.price, Team+1);
                    --n2;
                }
            } else if (j.position == "dav") {
                if (n3 < N3) {
                    dav_sol[n3] = j; ++n3;
                    generate_exh(i+1, Points+j.points, Price+j.price, Team+1);
                    --n3;
                }
            }
        }
    } else return;
    generate_exh(i+1, Points, Price, Team);
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

    por_sol = vector<Player> (1);
    def_sol = vector<Player> (N1);
    mig_sol = vector<Player> (N2);
    dav_sol = vector<Player> (N3);
    generate_exh(0, 0, 0, 0);
}
