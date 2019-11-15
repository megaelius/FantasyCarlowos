#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <time.h>
#include <algorithm>
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
      value of points.
    */
    bool operator<(const Player x) const{
        return points > x.points;
    }
};

clock_t start = clock();
//Filenames:
string data_base;
string outputFile;
string requirementsFile;
//Conditions of the team:
int N1, N2, N3, T, J;
/*Each variable "ni" stores the number of players of each position
(Goalkeeper, Defense, Midfield and Forward) that we have filled
during the execution of the algorithm.*/
int n0 = 0, n1 = 0, n2 = 0, n3 = 0;
int max_Points = -1;
//The database is stored in this vector:
vector<Player> Vplayers;
//These vectors store the line up during the execution of the algorithm:
vector<Player> por_sol, def_sol, mig_sol, dav_sol;

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

/*Given the number of players left to fill in a position, the name
  of that position and the "iterator" of Vplayers, calculates the maximum
  amount of points we can get. This amount is calculated by adding up
  the maximum N points of that position.
*/
int cota_pos(int N, string pos,int i){
    int cota = 0;
    int j = 0;
    while(j < N and i < Vplayers.size()){
        if(Vplayers[i].position == pos){
            ++j;
            cota += Vplayers[i].points;
        }
        ++i;
    }
    return cota;
}

/*Calculates the maximum quantity of points we can get with the current
  (incompleted) line up and if this quantity is less than the points of the
  last solution, returns false.
*/
bool valid(int Points, int i){
    int cota = 0;
    cota += cota_pos(1-n0,"por",i);
    cota += cota_pos(N1-n1,"def",i);
    cota += cota_pos(N2-n2,"mig",i);
    cota += cota_pos(N3-n3,"dav",i);
    if(Points + cota < max_Points)return false;
    return true;
}

/*
Generates combinations of players.
i refers to the "iterator" in the Vplayers vector (all the players).
Points refers to the amount of points of the team we are generating.
Price refers to the price of the team we are generating.
Team refers to the number of players in the team being generated.
Also uses the global variables T, n0, n1, n2, n3, N1, N2, N3 and max_Points*/
void generate_exh (int i, int Points, int Price, int Team) {
    if (Team == 11) { //Once a team is complete...
        if (Points > max_Points) {
            max_Points = Points;
            overwrite_solution(Points, Price);
        }
    }
    /*
    Stop when there are no more players left or when the ones remaining won't
    yield a better solution.*/
    if (i >= Vplayers.size() or not valid(Points, i)) return;
    Player j = Vplayers[i];
    if (Price + j.price <= T) { //If we can afford the player...
        if (j.position == "por") {
            if (n0 < 1) { //If we need more goalkeepers...
                por_sol[n0] = j; ++n0; //Add it and look for more players
                generate_exh(i+1, Points+j.points, Price+j.price, Team+1);
                --n0; //Don't add it
            }
        } else if (j.position == "def") { //Ídem
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
    generate_exh(i+1, Points, Price, Team); //Try next
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

    sort(Vplayers.begin(), Vplayers.end()); //Sorts by points in decreasing order

    //We assign the vectors the size given in the requirements
    por_sol = vector<Player> (1);
    def_sol = vector<Player> (N1);
    mig_sol = vector<Player> (N2);
    dav_sol = vector<Player> (N3);

    generate_exh(0, 0, 0, 0);
}
