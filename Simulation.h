#ifndef __SIMULATION_H_
#define __SIMULATION_H_

#include <iostream>
#include "Animator.h"
#include "VehicleBase.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <random>

#include <chrono>
#include <thread>

using namespace std;

class Simulation
{
    private:
        int maximum_simulated_time;
        int num_sect;

        // the time in sections the light will stay green for the north/south light
        int green_n_s;
        int yellow_n_s;

        int green_e_w;
        int yellow_e_w;

        int red_n_s;
        int red_e_w;

        // probability a new vehicle will spawn on the north section
        double prob_north;
        double prob_south;
        double prob_east;
        double prob_west;

        double prop_car;
        double prop_suv;
        double prop_trucks;

        //proportion for turns
        double prop_right_car;
        double prop_left_car;
        double prop_right_suv;
        double prop_left_suv;
        double prop_right_truck;
        double prop_left_truck;

        //vector to store all the vehicles
        vector<VehicleBase*> vehicles;

        //road vectors
        std::vector<VehicleBase*> westbound;
        std::vector<VehicleBase*> eastbound;
        std::vector<VehicleBase*> southbound;
        std::vector<VehicleBase*> northbound;

        int halfSize;

        string NS_light;
        string EW_light;

        std::vector<int>  allVehicleIDs; //keep track of all vehicle IDs spawned


    public:
        Simulation(string file, long seed);
        ~Simulation();

        void run();// this will run the simulation

        void spawn();
        void despawn(VehicleBase vehicle);
        void roll();

        void right_turn(VehicleBase &vehicle);
        void make_move(VehicleBase &vehicle);
        void left_turn(VehicleBase &vehicle);

        bool can_turn(VehicleBase &vehicle);
        bool can_turnLeft(VehicleBase &vehicle);
        bool can_move(VehicleBase &vehicle);
        bool can_cross_intersection(VehicleBase vehicle, int index, string designatedLight);
        bool is_on_intersection(VehicleBase vehicle, int index);

        double RandomNum();

        void create_vehicle();
        void reset();// resets the vectors
};


#endif
