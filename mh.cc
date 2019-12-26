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
    bool operator==(const Player x) const {
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
vector<Player> Vpor;
vector<Player> Vdef;
vector<Player> Vmig;
vector<Player> Vdav;
//These vectors store the line up during the execution of the algorithm:
vector<Player> por_sol, def_sol, mig_sol, dav_sol;
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
        if (price <= J){
            if(position == "por"){
                Vpor.push_back(j);
            }
            else if(position == "def"){
                Vdef.push_back(j);
            }
            else if(position == "mig"){
                Vmig.push_back(j);
            }

            else if(position == "dav"){
                Vdav.push_back(j);
            }
        }
    }
    in.close();
}

//Reads N1, N2, N3, T, J from the requirementsFile
void parameters () {
    ifstream in(requirementsFile);
    in >> N1 >> N2 >> N3 >> T >> J;
    in.close();
}

//For each position in the team, finds the players in the greedy solution
void fill_position(const vector<Player>& Vpos, vector<Player>& pos_sol,
                   int& Price, int& Points, int Ni){
    int ni = 0;
    for(int i = 0; i < Vpos.size(); i++) {
        if(Price + Vpos[i].price <= T and ni++ < Ni) {
            pos_sol.push_back(Vpos[i]);
            Points += Vpos[i].points;
            Price += Vpos[i].price;
        }
    }
}

/*
We sort the database in decreasing order of productivity = points/log(price)
and we choose the first team that we can afford. This is done by taking
the players with higher productivity in the first place.
*/
void greedy(){
    sort(Vpor.begin(), Vpor.end());
    sort(Vdef.begin(), Vdef.end());
    sort(Vmig.begin(), Vmig.end());
    sort(Vdav.begin(), Vdav.end());
    //We initialize the solution to 4 empty vectors.
    por_sol = vector<Player> (0);
    def_sol = vector<Player> (0);
    mig_sol = vector<Player> (0);
    dav_sol = vector<Player> (0);
    /*Each variable "ni" stores the number of players of each position
    (Goalkeeper, Defense, Midfield and Forward) that we have filled
    during the execution of the algorithm.*/
    int Price = 0, Points = 0;
    fill_position(Vpor, por_sol, Price, Points, 1);
    fill_position(Vdef, def_sol, Price, Points, N1);
    fill_position(Vmig, mig_sol, Price, Points, N2);
    fill_position(Vdav, dav_sol, Price, Points, N3);
    overwrite_solution(Points, Price);
    max_Points = Points;
}

// Returns a random number in the interval [l, u].
int random(int l, int u) {return l + rand() % (u-l+1); }

void change_random(vector<Player>& pos_sol2, int pos){
    int i; //position of the player we are going to put into our solution.
    int j; //position of the player we want to change.
    if(pos == 1){
        i = random(0,Vpor.size()-1);
        j = 0;
        pos_sol2[j] = Vpor[i];
    } else if(pos == 2){
        i = random(0,Vdef.size()-1);
        j = random(0,N1-1);
        pos_sol2[j] = Vdef[i];
    } else if(pos == 3){
        i = random(0,Vmig.size()-1);
        j = random(0,N2-1);
        pos_sol2[j] = Vmig[i];
    } else {
        i = random(0,Vdav.size()-1);
        j = random(0,N3-1);
        pos_sol2[j] = Vdav[i];
    }
}

void find_Neighbor(vector<Player>& por_sol2, vector<Player>& def_sol2,
                   vector<Player>& mig_sol2, vector<Player>& dav_sol2){
    int i = random(0, 10);
    if(i == 0){
        change_random(por_sol,1);
    }
    else if(i >= 1 and i<=N1){
        change_random(def_sol,2);
    }
    else if(i >= 1+N1 and i<=N1+N2){
        change_random(mig_sol,3);
    }
    else{
        change_random(dav_sol,4);
    }
}

void count(int& Points, int& Price, const vector<Player>& por_sol,
           const vector<Player>& def_sol, const vector<Player>& mig_sol,
           const vector<Player>& dav_sol){
    Points = 0;
    Price = 0;
    for(Player x : por_sol){
        Points += x.points;
        Price += x.price;
    }
    for(Player x : def_sol){
        Points += x.points;
        Price += x.price;
    }
    for(Player x : mig_sol){
        Points += x.points;
        Price += x.price;
    }
    for(Player x : dav_sol){
        Points += x.points;
        Price += x.price;
    }
}

//prob
double probability(double T, int Points, int Points2) {
    return exp((Points2-Points)/T);
}

/*
After greedy, variables with val:
por_sol, def_sol, mig_sol, dav_sol
price
points
n0, n1, n2, n3
are full
*/
void improve(double& Temp){
    int Points, Price; //puntos de sint Price2, Points2;
    count(Points,Price,por_sol,def_sol,mig_sol,dav_sol);
    vector<Player> por_sol2 = por_sol, def_sol2 = def_sol,
                    mig_sol2 = mig_sol, dav_sol2 = dav_sol;
    //changes one player.
    find_Neighbor(por_sol2, def_sol2, mig_sol2, dav_sol2);
    int Points2, Price2;
    count(Points2,Price2,por_sol2,def_sol2,mig_sol2,dav_sol2);
    if(Price2 > T) return;

    bool found = false;
    if (Points < Points2) {
        found = true;
    } else if (probability(T, Points, Points2) > rand()/(RAND_MAX+1.)) {
        found = true;
    }
    //cout << probability(T, Points, Points2) << endl;
    if (found) {
        if (por_sol != por_sol2 or def_sol != def_sol2 or mig_sol != mig_sol2 or dav_sol != dav_sol2) {
            cout << por_sol2[0].name << endl;
            for(Player x : def_sol2)cout << x.name << endl;
            for(Player x : mig_sol2)cout << x.name << endl;
            for(Player x : dav_sol2)cout << x.name << endl;
        }
        por_sol=por_sol2; def_sol=def_sol2; mig_sol=mig_sol2; dav_sol=dav_sol2;
        overwrite_solution(Points2, Price2);

    }
    Temp *= 0.9;//probar valores e ir calibrando
}

void GRASP(){//DUPLICADOS
    //fase1
    greedy();
    double Temp = 10e9;
    //fase2
    long long int k = 100;
    while (k > 0) {
        improve(Temp);
        --k;
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

    GRASP();
}
