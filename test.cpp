#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Simulation.cpp"
#include "VehicleBase.cpp"

TEST_CASE("VehicleBase Constructor Initialization"){
  SECTION( "vehicleCount's and vehicleID's update as vehicle's initalized" ) {
    VehicleBase car1(VehicleType::car, Direction::east);
    CHECK ( car1.vehicleCount == 1 );
    CHECK ( car1.getVehicleID() == 0 );
    VehicleBase suv1(VehicleType::suv, Direction::south);
    CHECK ( suv1.vehicleCount == 2 );
    CHECK ( suv1.getVehicleID() == 1 );
    }

    SECTION( "setting vehicle to another vehicle" ) {
      VehicleBase truck1(VehicleType::truck, Direction::east);
      VehicleBase truck2 = VehicleBase(truck1);
      CHECK ( truck1.getVehicleID() == truck2.getVehicleID() );
      }
}

TEST_CASE("Simulation Testing"){
  //Enter testing input file and seed here
  Simulation sim1("input_file_format.txt", 345); //create a simulation

  SECTION( "RandomNum generates different number given a seed" ) {
    CHECK (sim1.RandomNum()!=sim1.RandomNum());
  }

}
