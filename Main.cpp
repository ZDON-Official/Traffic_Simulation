#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include "Simulation.h"

int main(int argc, char const *argv[])
{

    ifstream infile {argv[1]};
    string file = argv[1];

    //throws an error if no input file provided and exits the program
    if(!infile){
        cerr << "could not open file: " << argv[1] << endl;
        exit(1);
    } else if(argc !=3){
      //checks for CL prompt: ./simulation [input file] [initial seed]
        cerr << "Usage: " << argv[0] << " [input file]" << " [(long) initial seed]" <<"\n";
        exit(1);
    }

    //store seed from CL to use in random number generator
    long seed = stol(argv[2]);

    Simulation sim(argv[1], seed); //create a simulation

    sim.run(); // run the simulations
    infile.close(); //close input file

    exit(0);
}
