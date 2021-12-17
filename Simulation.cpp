#ifndef __SIMULATION_CPP__
#define __SIMULATION_CPP__

#include "Simulation.h"

using my_clock = chrono::high_resolution_clock;
using seconds = chrono::duration<float>;

std::mt19937 randomNumberGenerator; // Mersenne twister
std::uniform_real_distribution<double> rand_double(0,1);

Simulation::Simulation(string file, long seed){

    ifstream infile {file};

    randomNumberGenerator.seed(seed);

    vector<double> input;
    double i;

    string s;

    while (infile >> s >> i)
    {
        input.push_back(i);
    }

    maximum_simulated_time = input[0];
    num_sect = input[1];

    // the time in sections the light will stay green for the north/south light
    green_n_s = input[2];
    yellow_n_s = input[3];

    green_e_w = input[4];
    yellow_e_w = input[5];

    red_n_s = green_e_w + yellow_e_w;
    red_e_w = green_n_s + yellow_n_s;

    // probability a new vehicle will spawn on the north section
    prob_north = input[6];
    prob_south = input[7];
    prob_east = input[8];
    prob_west = input[9];

    prop_car = input[10];
    prop_suv = input[11];
    prop_trucks = 1 - (prop_car + prop_suv);

    //proportion for turns
    prop_right_car = input[12];
    prop_left_car = input[13];
    prop_right_suv = input[14];
    prop_left_suv = input[15];
    prop_right_truck = input[16];
    prop_left_truck = input[17];


    halfSize = (num_sect)/2;


    //! resize and fills the vectors
    southbound.assign(halfSize * 2 + 2, nullptr);
    northbound.assign(halfSize * 2 + 2, nullptr);
    eastbound.assign(halfSize * 2 + 2, nullptr);
    westbound.assign(halfSize * 2 + 2, nullptr);

    // stores the color of the light
    string NS_light = "red";
    string EW_light = "green";

}



void Simulation::run(){

    Animator::MAX_VEHICLE_COUNT = 999;
    halfSize = (num_sect)/2;
    Animator anim(halfSize);

    // sets the initial color of the traffic light
    anim.setLightNorthSouth(LightColor::red);
    anim.setLightEastWest(LightColor::green);

    int NS_count = 100;
    int EW_count = 0;

    NS_light = "red";
    EW_light = "green";

    for (int i = 0; i < halfSize; i++)
    {
        create_vehicle();
    }

    for (int i = 0; i < halfSize; i++)
    {
        spawn();
    }
    // main for loop
    for(int sim = 0; sim < maximum_simulated_time+1; sim++){
        //create, spawn, and redraw animation
        anim.setVehiclesNorthbound(northbound);
        anim.setVehiclesWestbound(westbound);
        anim.setVehiclesSouthbound(southbound);
        anim.setVehiclesEastbound(eastbound);

        anim.draw(sim);

        // sleep for a second so it seems like the intersection is being animated
        this_thread::sleep_for(chrono::seconds(1));

        //loops through all vehicles and moves them straight or makes a turn
        //based on vehicle type and turning/driving straight probability
        for (int i=0; i<allVehicleIDs.size();i++){
            double random = RandomNum();// generates a new random number

            int ID_index = 0;
            int tempID = allVehicleIDs.at(i);

            // prevents against accessing despawned vehicles
            for (size_t j = 0; j < vehicles.size(); j++){
               if (tempID == vehicles.at(j)->getVehicleID()){
                   //found the matching vehicles
                   ID_index = j;
                   break;
               }
            }


            //vehicles vector should correspond with allVehicleIDs vector
            VehicleBase &vehicle = *vehicles.at(ID_index); //reference to vehicle

            VehicleType type =  vehicle.getVehicleType();
            Direction direction = vehicle.getCurrDirection();
            vector<VehicleBase*>  designatedVector;

            if(direction == Direction::north){
                designatedVector = northbound;
            }
            else if(direction == Direction::south){
                designatedVector = southbound;
            }
            else if(direction == Direction::east){
                designatedVector = eastbound;
            }
            else{
                designatedVector = westbound;
            }


            // Makes the vehicles move
            if(designatedVector.at(halfSize-1)!=nullptr && designatedVector.at(halfSize-1)->getVehicleID() == vehicle.getVehicleID()){
                //check for vehicles before the intersection
                if(type == VehicleType::car){
                    if(random<prop_right_car){
                        right_turn(vehicle);
                    }
                    else if(random<prop_right_car+prop_left_car){
                        //left turn
                        left_turn(vehicle);
                    }
                    else{
                        //go straight
                        make_move(vehicle);
                    }
                } else if(type == VehicleType::suv){
                    if(random<prop_right_suv){
                        right_turn(vehicle);
                    }
                    else if(random<prop_right_suv+prop_left_suv){
                      //left turn
                      left_turn(vehicle);
                    }
                    else{
                      //go straight
                      make_move(vehicle);
                    }
                } else{
                  if(random<prop_right_truck){
                        right_turn(vehicle);
                  }
                  else if(random<prop_right_truck+prop_left_truck){
                    //left turn
                    left_turn(vehicle);
                  }
                  else{
                    //go straight
                    make_move(vehicle);
                  }
                }
            } else{
                // vehicles not at the intersection should try to move forward
                make_move(vehicle);
            }
        }



        // if the beginning of a road is empty, create a new vehicle
        if(northbound.at(0) == nullptr || southbound.at(0) == nullptr
        || eastbound.at(0) == nullptr || westbound.at(0) == nullptr){
            create_vehicle();
        }
        //  this will rolls a vehicle section by section, one vehicle at a time
        roll();

        // TRAFFIC LIGHT
        if(EW_count == red_n_s){
            anim.setLightEastWest(LightColor::red);
            anim.setLightNorthSouth(LightColor::green);

            NS_light = "green";
            EW_light = "red";

            EW_count = 100;
            NS_count = 0;
        } else if(EW_count == green_e_w){
            anim.setLightEastWest(LightColor::yellow);

            EW_light = "yellow";
        } else if(NS_count == green_n_s)
        {
            anim.setLightNorthSouth(LightColor::yellow);

            NS_light = "yellow";
        } else if(NS_count == red_e_w ){
            EW_count = 0;
            NS_count = 100;

            anim.setLightEastWest(LightColor::green);
            anim.setLightNorthSouth(LightColor::red);

            EW_light = "green";
            NS_light = "red";
        }
        EW_count++;
        NS_count++;


    }
}

Simulation::~Simulation(){}




/*
    Creates a vehicle based on probability of direction and vehicle type.
    Adds this vehicle to the vehicles vector.
*/
void Simulation::create_vehicle(){
    // creates a new vehicle
    VehicleType vehicle;
    Direction direction;

    // probability for vehicle type
    double random = RandomNum();// generates a new random number

    if (random < prop_car){
        vehicle = VehicleType::car;
    } else if(random < 1-(1-(prop_car+prop_suv))){
        vehicle = VehicleType::suv;
    } else {
        vehicle = VehicleType::truck;
    }

    // probability for initial direction
    random = RandomNum();

    if (random < prob_north){
        direction = Direction::north;
    } else if(random < (prob_north+prob_south)){
        direction = Direction::south;
    } else if(random < (prob_north+prob_south+prob_east)){
        direction = Direction::east;
    } else {
        direction = Direction::west;
    }

    VehicleBase* vb = new VehicleBase(vehicle, direction);// creates the new vehicle

    vehicles.push_back(vb);// add to the vehicles vector
}

/* Roll vehicle section by section onto the streets until it is fully displayed.
   call roll() on vehicle after creation and when not fully on screen yet.
*/
int order = 0;
int inOrder=order; //line for in order vehicle assignment
bool fullyRolled=false;
int pointerCountRoll=0; //count # pointers allocated to a vehicle as it rolls

void Simulation::roll(){

    if(vehicles.at(inOrder)->getDespawn()){
        inOrder++;
        return;
    }

    for (size_t i = 0; i < allVehicleIDs.size(); i++)
    {
        if(vehicles.at(inOrder)->getVehicleID() == allVehicleIDs.at(i)){
            if (fullyRolled == false){
                break;
            } else{
                inOrder++;
                pointerCountRoll=0;
                return;
            }
        }
    }

    VehicleBase &vehicle = *vehicles.at(inOrder);

    //if-block for in order vehicle assignment
    if (inOrder >= vehicles.size() - 1){
        return; //nothing to roll in
    } else{
        if(fullyRolled == true){
            fullyRolled=false;
        }
    }

    VehicleType type =  vehicle.getVehicleType();
    Direction direction = vehicle.getCurrDirection();

    vector<VehicleBase*>  designatedVector;

    if(direction == Direction::north){
        designatedVector = northbound;
    }
    else if(direction == Direction::south){
        designatedVector = southbound;
    }
    else if(direction == Direction::east){
        designatedVector = eastbound;
    }
    else{
        designatedVector = westbound;
    }

    if(type == VehicleType::car){
        //roll car
        if(designatedVector.size() >= 2){ //checks for road sections space to roll
            if (designatedVector.at(0) == NULL && pointerCountRoll==0){
                allVehicleIDs.push_back(vehicle.getVehicleID());
                designatedVector.at(0) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;

            }
            else if (designatedVector.at(1) == NULL && pointerCountRoll==1
            && designatedVector.at(0)==&vehicle){
                designatedVector.at(1) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                if (pointerCountRoll==2){
                    fullyRolled=true;
                    pointerCountRoll=0;
                    inOrder++; //able to assign next vehicle
                    pointerCountRoll=0;
                }
            }
        }
    } else if(type == VehicleType::suv){
      //roll SUV
        if(designatedVector.size() >= 3){ //checks for road sections space to roll
            if (designatedVector.at(0) == NULL && pointerCountRoll==0){
                allVehicleIDs.push_back(vehicle.getVehicleID());
                designatedVector.at(0) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;
            }
            else if (designatedVector.at(1) == NULL && pointerCountRoll==1
            && designatedVector.at(0)==&vehicle){
                designatedVector.at(1) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;
            }
            else if (designatedVector.at(2) == NULL && pointerCountRoll==2
            && designatedVector.at(0)==&vehicle
            && designatedVector.at(1)==&vehicle){
                designatedVector.at(2) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                if (pointerCountRoll==3){
                    fullyRolled=true;
                    pointerCountRoll=0;
                    inOrder++; //able to assign next vehicle
                    pointerCountRoll=0;
                }
            }
        }
    } else {
        //roll truck
        if(designatedVector.size() >= 4){ //checks for road sections space to roll
            if (designatedVector.at(0) == NULL && pointerCountRoll==0){
                allVehicleIDs.push_back(vehicle.getVehicleID());
                designatedVector.at(0) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;
            } else if (designatedVector.at(1) == NULL && pointerCountRoll==1
              && designatedVector.at(0)==&vehicle){
                designatedVector.at(1) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;
            } else if (designatedVector.at(2) == NULL && pointerCountRoll==2
              && designatedVector.at(0)==&vehicle
              && designatedVector.at(1)==&vehicle){
                designatedVector.at(2) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                fullyRolled=false;
            } else if (designatedVector.at(3) == NULL && pointerCountRoll==3
            && designatedVector.at(0)==&vehicle
            && designatedVector.at(1)==&vehicle
            && designatedVector.at(2)==&vehicle){
                designatedVector.at(3) = &vehicle;
                pointerCountRoll = pointerCountRoll + 1;
                if (pointerCountRoll==4){
                    fullyRolled=true;
                    pointerCountRoll=0;
                    inOrder++;
                    pointerCountRoll=0;
                }
            }
        }
    }

    //sets the elements of designatedVectors into the corresponding direction bound vector
    if(direction == Direction::north){
        northbound = designatedVector;
    }
    else if(direction == Direction::south){
        southbound = designatedVector;
    }
    else if(direction == Direction::east){
        eastbound = designatedVector;
    }
    else{
        westbound = designatedVector;
    }

}


/* Places vehicles on the streets.
  Call spawn after creation and before altering currDirection of vehicle
*/
bool spawned = false;
void Simulation::spawn(){
    if ((vehicles.size())<1){ //catch out of range vector
        spawned=false;
        return;
    }
    else if(order > vehicles.size()-1){
        spawned=true;
        return;
    }
    VehicleBase &vehicle = *vehicles.at(order); //reference to a vehicle

    //if-block for in order vehicle assignment
    if (order == vehicles.size()-1){
        allVehicleIDs.push_back(vehicle.getVehicleID());
        spawned=true;
        // Once all vehicles have been spawned, this boolean is set to true
        // This prevents the method from spawning vehicles it have already spawned before
    } else{
        allVehicleIDs.push_back(vehicle.getVehicleID()); // used for make_move and can_move()
        spawned = false;
    }

    VehicleType type =  vehicle.getVehicleType();
    Direction direction = vehicle.getCurrDirection();

    // check which direction's vector to use when pointing sections to vehicles
    vector<VehicleBase*>  designatedVector;

    if(direction == Direction::north){
        designatedVector = northbound;
    }
    else if(direction == Direction::south){
        designatedVector = southbound;
    }
    else if(direction == Direction::east){
        designatedVector = eastbound;
    }
    else if (direction == Direction::west){
        designatedVector = westbound;
    }

    int pointerCount=0; //count # pointers allocated to a vehicle as it spawns

    // spawn vehicle based on vehicle type
    if(type == VehicleType::car){
        //spawn car
        for (int i=0; i<designatedVector.size()-2; i++){
            if((i !=halfSize && i != halfSize+1) && (i+1 !=halfSize && i+1 != halfSize+1)){
                if (designatedVector.at(i) == NULL && designatedVector.at(i+1) == NULL &&  pointerCount!=2){
                    designatedVector.at(i) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+1) = &vehicle;
                    pointerCount = pointerCount + 1;

                    order++; //able to assign next vehicle
                    break;
                }
            } else{
                continue;
            }
        }
    } else if(type == VehicleType::suv){
        //spawn SUV
        for (int i=0; i<designatedVector.size()-3; i++){
            if((i !=halfSize && i != halfSize+1) && (i+1 !=halfSize && i+1 != halfSize+1)
            && (i+2 !=halfSize && i+2 != halfSize+1)){
                if (designatedVector.at(i) == NULL &&  designatedVector.at(i+1) == NULL && designatedVector.at(i+2) == NULL && pointerCount!=3){
                    designatedVector.at(i) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+1) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+2) = &vehicle;
                    pointerCount = pointerCount + 1;

                    order++; //able to assign next vehicle
                    break;
                }
            } else{
              continue;
            }
        }
    } else {
        //spawn truck
        for (int i=0; i<designatedVector.size()-4; i++){
            if((i !=halfSize && i != halfSize+1) && (i+1 !=halfSize && i+1 != halfSize+1)
            && (i+2 !=halfSize && i+2 != halfSize+1) && (i+3 !=halfSize && i+3 != halfSize+1)){
                if (designatedVector.at(i) == NULL &&  designatedVector.at(i+1) == NULL
                && designatedVector.at(i+2) == NULL && designatedVector.at(i+3) == NULL
                && pointerCount!=4){
                    designatedVector.at(i) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+1) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+2) = &vehicle;
                    pointerCount = pointerCount + 1;
                    designatedVector.at(i+3) = &vehicle;
                    pointerCount = pointerCount + 1;
                    order++; //able to assign next vehicle
                    break;
                }
            } else{
              continue;
            }
        }
    }

    //sets the elements of designatedVectors into the corresponding direction bound vector
    if(direction == Direction::north){
        northbound = designatedVector;
    }
    else if(direction == Direction::south){
        southbound = designatedVector;
    }
    else if(direction == Direction::east){
        eastbound = designatedVector;
    }
    else{
        westbound = designatedVector;
    }
    inOrder = order;
}

/*
  Moves a vehicle forward if it's able to move forward
 */
void Simulation::make_move(VehicleBase &vehicle){
    if(allVehicleIDs.empty()){
        return; // no vehicle to move, road is empty
    }

    std::vector<VehicleBase*>  designatedVector;
    Direction direction;
    VehicleType type;

    bool move = false;
    int index = 0;

    direction = vehicle.getCurrDirection();
    type = vehicle.getVehicleType();

    if(direction == Direction::north){
        designatedVector = northbound;
    }
    else if(direction == Direction::south){
        designatedVector = southbound;
    }
    else if(direction == Direction::east){
        designatedVector = eastbound;
    }
    else{
        designatedVector = westbound;
    }

    // loop through the vehicles vector to get type and curr direction of the vehicle
    for(int i = 0; i < designatedVector.size(); i++){
        if(designatedVector.at(i) != nullptr){
            // check if the selected vehicle and the vehicle found are equal
            if (vehicle.getVehicleID() == designatedVector.at(i)->getVehicleID()){
                // send the vehicle to can_move to check if it can be moved
                index = i;
                move = can_move(vehicle);
                direction = designatedVector.at(i)->getCurrDirection();
                type = designatedVector.at(i)->getVehicleType();
                break; // break the loop, when vehicle is found
            }
        }
    }

    if (move == true){
        if(type == VehicleType::car){
            if (index+2<designatedVector.size()){
                designatedVector.at(index) = nullptr;
                designatedVector.at(index+2) = &vehicle;
            }
            else{
                designatedVector.at(index) = nullptr;
                if (index == designatedVector.size() - 1)
                {
                    //vehicle fully rolled out
                    despawn(vehicle);
                }
            }
        } else if(type == VehicleType::suv){
            if (index+3<designatedVector.size()){
                designatedVector.at(index) = nullptr;
                designatedVector.at(index+3) = &vehicle;
            }
            else{
                designatedVector.at(index) = nullptr;
                if (index == designatedVector.size() - 1)
                {
                    //vehicle fully rolled out
                    despawn(vehicle);
                }
            }
        } else if(type == VehicleType::truck){
            if (index+4<designatedVector.size()){
                designatedVector.at(index) = nullptr;
                designatedVector.at(index+4) = &vehicle;
            }
            else{
                designatedVector.at(index) = nullptr;
                if(index == designatedVector.size()-1){
                    despawn(vehicle);
                }
            }
        }
    } else {
        return; // can't move the vehicle, exits the method
    }

    // sets the elements of designatedVectors into the corresponding direction bound vector
    if(direction == Direction::north){
        northbound = designatedVector;
    }
    else if(direction == Direction::south){
        southbound = designatedVector;
    }
    else if(direction == Direction::east){
        eastbound = designatedVector;
    }
    else{
        westbound = designatedVector;
    }
}

// resets the vector before moving cars
void Simulation::reset(){
        southbound.assign(halfSize * 2 + 2, nullptr);
        northbound.assign(halfSize * 2 + 2, nullptr);
        eastbound.assign(halfSize * 2 + 2, nullptr);
        westbound.assign(halfSize * 2 + 2, nullptr);
}

//Turns the given vehicle right
void Simulation::right_turn(VehicleBase &vehicle){
    if(allVehicleIDs.empty()){
        return;
    }
    std::vector<VehicleBase *> designatedVector;
    std::vector<VehicleBase *> turningLane;
    std::vector<VehicleBase *> turnedFrom;
    Direction direction;
    Direction newDirection;
    VehicleType type;
    bool turn = false;
    int index = 0;

    direction = vehicle.getCurrDirection();
    type = vehicle.getVehicleType();

    if (direction == Direction::north){
        designatedVector = northbound;
        turningLane = eastbound;
        turnedFrom = westbound;
        newDirection = Direction::east;
    }
    else if (direction == Direction::south){
        designatedVector = southbound;
        turningLane = westbound;
        turnedFrom = eastbound;
        newDirection = Direction::west;
    }
    else if (direction == Direction::east){
        designatedVector = eastbound;
        turningLane = southbound;
        turnedFrom = northbound;
        newDirection = Direction::south;
    }
    else if (direction == Direction::west)
    {
        designatedVector = westbound;
        turningLane = northbound;
        turnedFrom = southbound;
        newDirection = Direction::north;
    }


    // loop through the vehicles vector to get type and curr direction of the vehicle
    for (int i = 0; i < designatedVector.size(); i++){
        if (designatedVector.at(i) != nullptr){
            // check if the selected vehicle and the vehicle found are equal
            if (vehicle.getVehicleID() == designatedVector.at(i)->getVehicleID()){
                // send the vehicle to can_turn to check if it can turn
                index = i;
                direction = designatedVector.at(i)->getCurrDirection();
                type = designatedVector.at(i)->getVehicleType();
                if (vehicle.isTurning()){
                    // already turning so no need to call can_turn
                    break;
                } else{
                    turn = can_turn(vehicle);
                }
                break; // break the loop when vehicle is found
            }
        }
    }

    if(designatedVector.at(halfSize+1) == &vehicle && designatedVector.at(halfSize+2) == &vehicle && vehicle.isTurning() == false){
        make_move(vehicle);
        return;
    }

    if(turn == false && vehicle.isTurning() == false){
        make_move(vehicle);
        return;
    }

    // code to make the turn
    if(vehicle.isTurning()){
        //when vehicle is in the middle of a turn
        if(type == VehicleType::car){
            if(designatedVector.at(index + 1) == nullptr){
                designatedVector.at(index + 1) = &vehicle;
                turnedFrom.at(halfSize-1) = nullptr;
            } else if(designatedVector.at(halfSize+1) != nullptr
            && designatedVector.at(halfSize+1)->getVehicleID() == vehicle.getVehicleID()){
                //on the intersection
                designatedVector.at(index+2) = &vehicle;
                designatedVector.at(index) = nullptr;
                vehicle.setTurn(false);
            }
        }
        else if (type == VehicleType::suv){

            designatedVector.at((halfSize+2) + vehicle.getIndex()) = &vehicle;
            turnedFrom.at((halfSize-2) + vehicle.getIndex()) = nullptr;
            vehicle.addIndex();

            if(vehicle.getIndex() == 2){
                vehicle.resetIndex();
                vehicle.setTurn(false);
            }
        }
        else if (type == VehicleType::truck){
            designatedVector.at((halfSize + 2) + vehicle.getIndex()) = &vehicle;
            turnedFrom.at((halfSize - 3) + vehicle.getIndex()) = nullptr;
            vehicle.addIndex();

            if (vehicle.getIndex() == 3){
                vehicle.resetIndex();
                vehicle.setTurn(false);
            }
        }
    } else if(turn == true){
        if (type == VehicleType::car){
            designatedVector.at(index) = nullptr;
            turningLane.at(halfSize + 1) = &vehicle;
            vehicle.setTurn(true);
            vehicle.setDirection(newDirection);
        }
        else if (type == VehicleType::suv){
            designatedVector.at(index) = nullptr;
            turningLane.at(halfSize + 1) = &vehicle;
            vehicle.setTurn(true);
            vehicle.setDirection(newDirection);
        }
        else if (type == VehicleType::truck){
            designatedVector.at(index) = nullptr; // points to the back of the car
            turningLane.at(halfSize + 1) = &vehicle;
            vehicle.setTurn(true);
            vehicle.setDirection(newDirection);
        }
    } else{
        return;
    }

    if (direction == Direction::north){
        northbound = designatedVector;
        eastbound = turningLane;
        westbound = turnedFrom;
    }
    else if (direction == Direction::south){
        southbound = designatedVector;
        westbound = turningLane;
        eastbound = turnedFrom;
    }
    else if (direction == Direction::east){
        eastbound = designatedVector;
        southbound = turningLane;
        northbound = turnedFrom;
    }
    else{
        westbound = designatedVector;
        northbound = turningLane;
        southbound = turnedFrom;
    }
}

//Checks if a vehicle can make a right turn
bool Simulation::can_turn(VehicleBase &vehicle){

    VehicleType type = vehicle.getVehicleType();
    Direction direction = vehicle.getCurrDirection();

    string designatedLight;
    if (direction == Direction::north || direction == Direction::south)
    {
        designatedLight = NS_light;
    }
    else if (direction == Direction::east || direction == Direction::west)
    {
        designatedLight = EW_light;
    }

    if(designatedLight == ("red")){
        return false;
    }

    int index;
    std::vector<VehicleBase *> designatedVector;
    std::vector<VehicleBase *> turningLane;

    if (direction == Direction::north){
        designatedVector = northbound;
        turningLane = eastbound;
    }
    else if (direction == Direction::south){
        designatedVector = southbound;
        turningLane = westbound;
    }
    else if (direction == Direction::east){
        designatedVector = eastbound;
        turningLane = southbound;
    }
    else if (direction == Direction::west){
        designatedVector = westbound;
        turningLane = northbound;
    }

    // loop through the vector to see where the vehicle is
    for (size_t i = 0; i < designatedVector.size(); i++){
        if (designatedVector.at(i) == nullptr){
            continue;
        }
        else if (designatedVector.at(i)->getVehicleID() == vehicle.getVehicleID()){
            index = i;
            break;
        }
    }

    if (index>halfSize){
      //checks if vehicle has passed the first half of the intersection
      return false;
    }
    if (designatedLight == ("yellow") && index < halfSize-1 && halfSize-1>=0){
      //checks if vehicle is far before the intersection and has a yellow light,
      //then vehicle should not turn on yellow
      return false;
    }



    if(designatedVector.at(halfSize-1) == nullptr){ // no car at the intersection
        return false;
    }

    if(designatedVector.at(halfSize-1)->getVehicleID() == vehicle.getVehicleID() && designatedVector.at(halfSize) == nullptr){
        // the section in front of the vehicle is an intersection and is empty
        if (halfSize + 2<turningLane.size()){
          if(turningLane.at(halfSize + 2) == nullptr && turningLane.at(halfSize + 1)==nullptr){
              //both the intersection and the section on the other road is empty
              return true;
          }
        }
    } else{
        return false;
    }

    return false;
}

/*
Method checks if a vehicle can move.
Returns true if vehicle is in in intersection regardless of red/yellow/green light
Returns true if light is green.
Returns false if vehicle is currently turning.
Returns true if there's space to line up before an intersection on a red/yellow light
Returns false if vehicle is beyond the road
 */
bool Simulation::can_move(VehicleBase &vehicle){
    if(vehicle.isTurning()){
        //when true, return false as vehicle is turning
        right_turn(vehicle);
        return false;
    } else if(vehicle.getLeft()){
        left_turn(vehicle);
        return false;
    }

    VehicleType type = vehicle.getVehicleType();
    Direction direction = vehicle.getCurrDirection();

    //store the light corresponding to the car's direction
    string designatedLight;
    if(direction == Direction::north || direction == Direction::south){
      designatedLight=NS_light;
    }
    else if(direction == Direction::east || direction == Direction::west){
      designatedLight=EW_light;
    }

    int index;
    std::vector<VehicleBase*>  designatedVector;
    std::vector<VehicleBase*>  rightSide;// right side intersection
    std::vector<VehicleBase*>  leftSide;// left side intersection

    if(direction == Direction::north){
        designatedVector = northbound;
        rightSide = westbound;
        leftSide = eastbound;
    }
    else if(direction == Direction::south){
        designatedVector = southbound;
        rightSide = eastbound;
        leftSide = westbound;
    }
    else if(direction == Direction::east){
        designatedVector = eastbound;
        rightSide = northbound;
        leftSide = southbound;
    }
    else{
        designatedVector = westbound;
        rightSide = southbound;
        leftSide = northbound;
    }

    // loop through the vector to see where the vehicle is
    for (size_t i = 0; i < designatedVector.size(); i++)
    {
        if (designatedVector.at(i) == nullptr){
            continue;
        } else if(designatedVector.at(i)->getVehicleID() == vehicle.getVehicleID()){
            index = i;
            break;
        }
    }

    // collision prevention
    if(designatedVector.at(halfSize-1) == &vehicle && leftSide.at(halfSize+1) != nullptr){
        return false;
    } else if(designatedVector.at(halfSize) == &vehicle && rightSide.at(halfSize) != nullptr){
        return false;
    }

    if(designatedVector.at(halfSize-1)!= nullptr && designatedLight == "red"
    && designatedVector.at(halfSize-1) == &vehicle){
        // vehicle is at the intersection and the light is red
        if(designatedVector.at(halfSize) != &vehicle){
            // already not on the intersection
            return false;
        }
    }

    // Check to see if space in front is empty
    // Check if the space ahead is the intersection or not
    if (type == VehicleType::car){
        if(index + 2 >= designatedVector.size()){
            return true;
        } else if (designatedVector.at(index) != designatedVector.at(index + 1)){
            // returns false because car has not fully rolled in
            return false;
        }

        if (designatedLight==("red") || designatedLight==("yellow")){
            if ((designatedVector.at(index + 2) == nullptr) && index + 2 < halfSize){
                return true; //line up before the intersection on red/yellow
            }
            if (halfSize+1 <designatedVector.size() && index + 2 > halfSize+1 && designatedVector.at(index + 2) == nullptr){
                //on red/yellow if vehicle is beyond intersection and the space infront
                //is empty, then continue moving
                return true;
            }
        }

        if (is_on_intersection(vehicle, index) && designatedVector.at(index + 2) == nullptr && (designatedLight==("red") || designatedLight==("yellow"))){
            return true;
        }
        if(designatedVector.at(index + 2) == nullptr && designatedLight==("green")){
            //on green if space infront of vehicle is empty, then continue moving
            return true;
        }
        return false;
    } else if(type == VehicleType::suv){
        if(index + 3 >= designatedVector.size()){
            return true;
        } else if (designatedVector.at(index) != designatedVector.at(index + 2)){
            return false;
        }

        if (designatedLight==("red") || designatedLight==("yellow")){
            if ((designatedVector.at(index + 3) == nullptr) && index + 3 < halfSize){
                return true; //line up before the intersection on red/yellow
            }

            if (halfSize+1 <designatedVector.size() && index + 3 > halfSize+1 && designatedVector.at(index + 3) == nullptr){
                //on red/yellow if vehicle is beyond intersection and the space infront
                //is empty, then continue moving
                return true;
            }

        }

        if ( is_on_intersection(vehicle, index)){
            return true;
        }

        if(designatedVector.at(index + 3) == nullptr && designatedLight==("green")){
            //on green if space infront of vehicle is empty, then continue moving
            return true;

        }
        return false;
    } else if(type == VehicleType::truck){
        if(index + 4 >= designatedVector.size()){
            return true;
        } else if (designatedVector.at(index) != designatedVector.at(index + 3)){
            return false;
        }

        if (designatedLight==("red") || designatedLight==("yellow")){
            if ((designatedVector.at(index + 4) == nullptr) && index + 4 < halfSize){
                return true; //line up before the intersection on red/yellow
            }
            if (halfSize+1 <designatedVector.size() && index + 4 > halfSize+1 && designatedVector.at(index + 4) == nullptr){
                //on red/yellow if vehicle is beyond intersection and the space infront
                //is empty, then continue moving
                return true;
            }
        }

        if (is_on_intersection(vehicle, index) && designatedVector.at(index + 4) == nullptr && (designatedLight==("red") || designatedLight==("yellow"))){
            return true;
        }

        if(designatedVector.at(index + 4) == nullptr && designatedLight==("green")){
            //on green if space infront of vehicle is empty, then continue moving
            return true;
        }
        return false;
    }
    return false; //surpresses compiler warning, should never be reached
}


/*
Method checks if vehicle is on intersection on a yellow/red light
Returns true if is on intersection on a yellow/red light
*/
bool Simulation::can_cross_intersection(VehicleBase vehicle, int index, string designatedLight){
  VehicleType type = vehicle.getVehicleType();

  if (type == VehicleType::car){
    if ( designatedLight==("yellow") || designatedLight==("red")  ){
      //check if already in intersection
      if(is_on_intersection(vehicle, index)){
        return true;
      }
      return false;
    }
  }
  else if (type == VehicleType::suv){
    if (designatedLight==("yellow") || designatedLight==("red")){
      //check if already in intersection
      if(is_on_intersection(vehicle, index)){
        return true;
      }
      return false;
    }
  }
  else if (type == VehicleType::truck){
    if (designatedLight==("yellow") || designatedLight==("red")){
      //check if already in intersection
      if(is_on_intersection(vehicle, index)){
        return true;
      }
      return false;
    }
  }
    return false;
}

/*
    Method checks if any section of a vehicle is on the intersection
*/
bool Simulation::is_on_intersection(VehicleBase vehicle, int index){
  VehicleType type = vehicle.getVehicleType();
  if (type == VehicleType::car){
    if ( index == halfSize+1 || index+1 == halfSize  ){
      //check if already in intersection
      return true;
    }
      return false;
  }
  if (type == VehicleType::suv){
    if ( index == halfSize || index+1 == halfSize+1
    || index+1 == halfSize || index+2 == halfSize+1
    || index+2 == halfSize  ){
        //check if already in intersection
        return true;
    }
        return false;
    }
    else if (type == VehicleType::truck){
        if ( index+3==halfSize || index+3==halfSize+1
        || index+2==halfSize || index+2==halfSize+1
        || index+1==halfSize || index+1==halfSize+1
        || index == halfSize || index==halfSize+1  ){
            //check if already in intersection
            return true;
        }
        return false;
    }
    return false;
}


double Simulation::RandomNum(){
    double randNum = rand_double(randomNumberGenerator);

    return randNum;
}

void Simulation::despawn(VehicleBase vehicle){
    int ID = vehicle.getVehicleID();
    int index = 0;

    for (int i=0; i< vehicles.size(); i++)
    {
        if(vehicles.at(i)->getVehicleID() == ID){
            //found the vehicles
            index = i;
            vehicles.at(index)->setDespawn(true);
            break;
        }
    }

    for (int i = 0; i < allVehicleIDs.size(); i++){
        if(allVehicleIDs.at(i) == ID){
            //found the ID
            allVehicleIDs.erase(allVehicleIDs.begin() + i); //Removes from the ID
            break;
        }
    }
}

void Simulation::left_turn(VehicleBase &vehicle){
    if (allVehicleIDs.empty()){
        return;
    }

    std::vector<VehicleBase*> designatedVector;
    std::vector<VehicleBase*> turningLane_mid;
    std::vector<VehicleBase*> turningLane_final;
    std::vector<VehicleBase*> turnedFrom;
    Direction direction;
    Direction newDirection;
    VehicleType type;

    bool turn = false;
    int index = 0;

    direction = vehicle.getCurrDirection();
    type = vehicle.getVehicleType();

    if (direction == Direction::north)
    {
        designatedVector = northbound;
        turningLane_mid = eastbound;
        turningLane_final = westbound;
        turnedFrom = eastbound;
        newDirection = Direction::west;
    }
    else if (direction == Direction::south)
    {
        designatedVector = southbound;
        turningLane_mid = westbound;
        turningLane_final = eastbound;
        turnedFrom = westbound;
        newDirection = Direction::east;
    }
    else if (direction == Direction::east)
    {
        designatedVector = eastbound;
        turningLane_mid = southbound;
        turningLane_final = northbound;
        turnedFrom = southbound;
        newDirection = Direction::north;
    }
    else if (direction == Direction::west){
        designatedVector = westbound;
        turningLane_mid = northbound; // lane in mid
        turningLane_final = southbound; // destination
        turnedFrom = northbound;
        newDirection = Direction::south;
    }



    for (int i = 0; i < designatedVector.size(); i++)
    {
        if (designatedVector.at(i) != nullptr)
        {
            // check if the selected vehicle and the vehicle found are equal
            if (vehicle.getVehicleID() == designatedVector.at(i)->getVehicleID())
            {
                // send the vehicle to can_turn to check if it can turn
                index = i;
                direction = designatedVector.at(i)->getCurrDirection();
                type = designatedVector.at(i)->getVehicleType();
                if (vehicle.isTurning())
                {
                    // already turning so no need to call can_turn
                    break;
                }
                else
                {
                    turn = can_turnLeft(vehicle);
                }
                break; // break the loop, when vehicle is found
            }
        }
    }


    if(turn == false && vehicle.getLeft() == false){
        make_move(vehicle);
    }

    // in the proccess of turning left
    if(vehicle.getLeft()){
        if (VehicleType::car ==  type){
            if(turningLane_final.at(halfSize+1) == nullptr && vehicle.getCurrDirection() == vehicle.getVehicleOriginalDirection()){
                turningLane_final.at(halfSize+1) = &vehicle;
                designatedVector.at(index) = nullptr;
                vehicle.setDirection(newDirection);
            }
            else if (designatedVector.at(halfSize + 2) == nullptr && designatedVector.at(halfSize + 1)->getVehicleID() == vehicle.getVehicleID()){
                turnedFrom.at(halfSize) = nullptr;
                designatedVector.at(halfSize+2) = &vehicle;
                vehicle.switchLeft();
            }
        } else if(VehicleType::suv == type){
            if(turningLane_final.at(halfSize+1) == nullptr && vehicle.getCurrDirection() == vehicle.getVehicleOriginalDirection()){
                turningLane_final.at(halfSize+1) = &vehicle;
                designatedVector.at(index) = nullptr;
                vehicle.setDirection(newDirection);
                vehicle.addLeftIndex();
            } else if(designatedVector.at(halfSize+2) == nullptr && designatedVector.at(halfSize+1) == &vehicle){
                turnedFrom.at(halfSize-vehicle.getLeftIndex()) = nullptr;
                designatedVector.at((halfSize+1) + vehicle.getLeftIndex()) = &vehicle;
                vehicle.addLeftIndex();
            } else if(turnedFrom.at(halfSize) == &vehicle && turnedFrom.at(halfSize-1) == nullptr){
                turnedFrom.at(halfSize) = nullptr;
                designatedVector.at((halfSize+1) + vehicle.getLeftIndex()) = &vehicle;
                vehicle.resetLeftIndex();
                vehicle.switchLeft();
            }
        } else if(VehicleType::truck == type){
            designatedVector.at((halfSize+1) + vehicle.getLeftIndex()) = &vehicle;
            turnedFrom.at((halfSize-3)+vehicle.getLeftIndex()) = nullptr;
            vehicle.addLeftIndex();

            if(vehicle.getLeftIndex() == 4){
                vehicle.resetLeftIndex();
                vehicle.switchLeft();
            }
        }

    } else if(turn == true){
        if (VehicleType::car == type){
            designatedVector.at(index) = nullptr;
            designatedVector.at(halfSize) = &vehicle;
            vehicle.switchLeft();
        } else if(VehicleType::suv == type){
            designatedVector.at(index) = nullptr;
            designatedVector.at(halfSize) = &vehicle;
            vehicle.switchLeft();
        } else if(VehicleType::truck == type){
            designatedVector.at(index) = nullptr;
            designatedVector.at(halfSize) = &vehicle;
            vehicle.switchLeft();
            vehicle.setDirection(newDirection);
        }
    }

    if (direction == Direction::north)
    {
        northbound = designatedVector;
        westbound = turningLane_final;
        eastbound = turnedFrom;
    }
    else if (direction == Direction::south)
    {

        southbound = designatedVector;
        westbound = turningLane_mid;
        eastbound = turningLane_final;
        westbound = turnedFrom;
    }
    else if (direction == Direction::east)
    {
        eastbound = designatedVector;
        northbound = turningLane_final;
        southbound = turnedFrom;
    }
    else if (direction == Direction::west)
    {
        westbound = designatedVector;
        northbound = turningLane_mid;
        southbound = turningLane_final;
        northbound = turnedFrom;
    }
}



bool Simulation::can_turnLeft(VehicleBase &vehicle){

    VehicleType type = vehicle.getVehicleType();
    Direction direction = vehicle.getCurrDirection();

    string designatedLight;
    if (direction == Direction::north || direction == Direction::south){
        designatedLight = NS_light;
    }
    else if (direction == Direction::east || direction == Direction::west){
        designatedLight = EW_light;
    }

    // vehicles will only make left turn on green
    if (designatedLight == ("red") || designatedLight == ("yellow")){
        return false;
    }

    int index;
    std::vector<VehicleBase*> designatedVector;
    std::vector<VehicleBase*> oncoming;
    std::vector<VehicleBase*> turningLane_mid;
    std::vector<VehicleBase*> turningLane_final;

    if (direction == Direction::north)
    {
        designatedVector = northbound;
        oncoming = southbound;
        turningLane_mid = eastbound;
        turningLane_final = westbound;
    }
    else if (direction == Direction::south)
    {
        designatedVector = southbound;
        oncoming = northbound;
        turningLane_mid = westbound;
        turningLane_final = eastbound;
    }
    else if (direction == Direction::east)
    {
        designatedVector = eastbound;
        oncoming = westbound;
        turningLane_mid = southbound;
        turningLane_final = northbound;
    }
    else if (direction == Direction::west)
    {
        designatedVector = westbound;
        oncoming = eastbound;
        turningLane_mid = northbound;
        turningLane_final = southbound;
    }

    for (size_t i = 0; i < designatedVector.size(); i++)
    {
        if (designatedVector.at(i) == nullptr)
        {
            continue;
        }
        else if (designatedVector.at(i)->getVehicleID() == vehicle.getVehicleID())
        {
            index = i;
            break;
        }
    }


    // not at the interesection
    if(index > halfSize){
        return false;
    }
    else if (designatedVector.at(halfSize - 1) == nullptr){
        // no car at the intersection
        return false;
    }

    //check if can cross
    if (designatedVector.at(halfSize - 1)->getVehicleID() == vehicle.getVehicleID() && designatedVector.at(halfSize) == nullptr)
    {
        // the section in front of the vehicle is an intersection and is empty
        if (halfSize + 2 < turningLane_final.size())
        {
            if (turningLane_final.at(halfSize + 2) == nullptr && turningLane_final.at(halfSize+1) == nullptr)
            {
                // both the intersection and the section on the other road is empty
                if(type == VehicleType::car){
                    if (oncoming.at(halfSize-1) == nullptr && oncoming.at(halfSize-2) == nullptr && oncoming.at(halfSize) == nullptr){
                        return true;
                    }
                } else if(VehicleType::suv == type){
                    if (oncoming.at(halfSize-1) == nullptr && oncoming.at(halfSize-2) == nullptr && oncoming.at(halfSize-3) == nullptr
                    && oncoming.at(halfSize) == nullptr){
                        return true;
                    }
                } else if(VehicleType::truck == type){
                    if (oncoming.at(halfSize - 1) == nullptr && oncoming.at(halfSize - 2) == nullptr
                    && oncoming.at(halfSize - 3) == nullptr && oncoming.at(halfSize-4) == nullptr
                    && oncoming.at(halfSize) == nullptr){
                        if(turningLane_final.at(halfSize+1) == nullptr && turningLane_final.at(halfSize+2) == nullptr){
                            return true;
                        }
                    }
                }
            }
        }
    }
    else
    {
        // should never enter this else statement, but just to be safe
        return false;
    }

    return false;
}

#endif
