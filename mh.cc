#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <time.h>
#include <algorithm>
#include <cmath>
using namespace std;


/*
Stores a player with all the data given by the database.
*/
struct Player{
    string name;
    string position;
    string club;
    int points;
    int price;

    /*Comparison operator between two players. Only returns true if all of their
      atributes are equal.
    */
    bool operator==(const Player x) {
        return name == x.name and position == x.position and
               club == x.club and points == x.points and price == x.price;
    }
    /*Lesser operator. We use it to order the vector of players in decreasing
      value of points/log(price).
      We use the log(price) because the price of good players, grows way faster
      than their points.
    */
    bool operator<(const Player x) const{
        if(price != 0 and x.price != 0){
            return double(points)/log(price) > double(x.points)/log(x.price);
        }
        else if(price == 0)return false;
        else return true;
    }
};

clock_t start = clock();
//Filenames:
string data_base;
string outputFile;
string requirementsFile;
//Conditions of the team:
int N1, N2, N3, T, J;
//Points of the best solution found yet
int max_Points;
//The database is stored in this vector:
vector<Player> Vplayers;
//These vectors store the line up during the execution of the algorithm:
vector<Player> por_sol, def_sol, mig_sol, dav_sol, por_N, def_N, mig_N, dav_N;
//COMENTAR
int Points, Price, Points_N, Price_N;

/*Writes in the file "out", all the players of the solution
  playing in the same position */
void write_position(ofstream& out, const vector<Player>& sol) {
    for(int i = 0; i < sol.size(); ++i){
        if(i != 0) out << ';';
        out << sol[i].name;
    }
    out << endl;
}

/*Writes the new solution found and the time it took to find it
in the asked format and in the given output file.
*/
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
    out << "Punts: " << Points << endl;
    out << "Preu: " << Price << endl;

    out.close();
}

/*Reads the database and only stores in Vplayers the ones we can afford.
*/
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

//Reads N1, N2, N3, T, J from the requirementsFile
void parameters () {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
    in.close();
}

/*
We sort the database in decreasing order of productivity = points/log(price)
and we choose the first team that we can afford. This is done by taking
the players with higher productivity in the first place.
*/
void greedy(){
    sort(Vplayers.begin(), Vplayers.end());
    //We initialize the solution to 4 empty vectors.
    por_sol = vector<Player> (0);
    def_sol = vector<Player> (0);
    mig_sol = vector<Player> (0);
    dav_sol = vector<Player> (0);
    /*Each variable "ni" stores the number of players of each position
    (Goalkeeper, Defense, Midfield and Forward) that we have filled
    during the execution of the algorithm.*/
    int n0 = 0, n1 = 0, n2 = 0, n3 = 0;
    int Price = 0, Points = 0;
    for(int i = 0; i < Vplayers.size() and not(n0+n1+n2+n3 == 11); ++i){
        if(Price + Vplayers[i].price <= T){
            bool choose = false;
            if((choose = Vplayers[i].position == "por" and n0 == 0)){
                n0++;
                por_sol.push_back(Vplayers[i]);
            }
            else if((choose = Vplayers[i].position == "def" and n1 < N1)){
                n1++;
                def_sol.push_back(Vplayers[i]);
            }
            else if((choose = Vplayers[i].position == "mig" and n2 < N2)){
                n2++;
                mig_sol.push_back(Vplayers[i]);
            }
            else if((choose = Vplayers[i].position == "dav" and n3 < N3)){
                n3++;
                dav_sol.push_back(Vplayers[i]);
            }
            if(choose){
                Points += Vplayers[i].points;
                Price += Vplayers[i].price;
            }
        }
    }
    overwrite_solution(Points, Price);
    max_Points = Points;
}

double probability() { return exp((puntsS'-puntsS)/T); }

// Returns a random number in the interval [l, u].
double random(int l, int u) { return l + (double)rand() % (u-l+1); }

/*
After greedy, variables with val:
por_sol, def_sol, mig_sol, dav_sol
price
points
n0, n1, n2, n3
are full
*/
void mh(){
    /*
    RESETS: Sometimes it is better to move back to a solution that was significantly better rather than always moving from the current state.
    To do this we set s and e to sbest and ebest and perhaps restart the annealing schedule. Popular criteria: restarting based on a fixed
    number of steps, based on whether the current energy is too high compared to the best energy obtained so far, restarting randomly...
    */
    greedy(); //Generates an initial solution
    int Points_s = max_Points;
    int Points_sN;
    double T = 1e9;
    //vector<bool> S(J, false);//mirar cuantos llevo usados?
    for (int i = 0; i < Vplayers.size(); ++i) { //and not found?
        find_Neighbor();//genera otra solucion cambiando un jugador busca un random del vector tocho y si no esta en el team lo metes
        if (Points_s < Points_sN) {
            Points_s = Points_sN;
            por_sol = por_N; def_sol = def_N; mig_sol = mig_N; dav_sol = dav_N;
            if (max_Points < Points_s) {
                int Price = find_Price();
                max_Points = Points_s;
                overwrite_solution(max_Points, Price);
            }
        } else if (probability(T) > random(0, 1)) {
            por_sol = por_N; def_sol = def_N; mig_sol = mig_N; dav_sol = dav_N;
            Points_s = Points_sN;
        }
        T *= 0.9;//probar valores e ir calibrando
    }
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

    parameters(); //Reads N1, N2, N3, T, J from the requirementsFile

    //Checks if the parameters are correct.
    if (N1+N2+N3 != 10 or N1 < 1 or N1 > 5 or N2 < 1 or N2 > 5 or N3 < 1 or N3 > 5) {
        cout << "Error: Invalid requirements" << endl;
        exit(1);
    }

    database_reader(); //Reads players from database and saves them in Vplayers

    mh();
}
