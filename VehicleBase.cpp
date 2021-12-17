#ifndef __VEHICLE_BASE_CPP__
#define __VEHICLE_BASE_CPP__

#include "VehicleBase.h"
#include <utility>

using namespace std;

int VehicleBase::vehicleCount = 0;

VehicleBase::VehicleBase(VehicleType type, Direction direction)
    : vehicleID(VehicleBase::vehicleCount++),
      vehicleType(type),
      vehicleDirection(direction),
      currDirection(direction),
      turn(false),
      turnIndex(0),
      despawn(false),
      leftTurn(false),
      leftIndex(0)
{}

VehicleBase::VehicleBase(const VehicleBase& other)
    : vehicleID(other.vehicleID),
      vehicleType(other.vehicleType),
      vehicleDirection(other.vehicleDirection),
      currDirection(other.currDirection),
      turn(other.turn),
      turnIndex(other.turnIndex),
      despawn(other.despawn),
      leftTurn(other.leftTurn),
      leftIndex(other.leftIndex)
{}

VehicleBase::~VehicleBase(){}

VehicleBase& VehicleBase::operator=(const VehicleBase& other){
  if(this == &other){
    return *this;
  }
  vehicleID = other.vehicleID;
  vehicleType = other.vehicleType;
  vehicleDirection = other.vehicleDirection;
  currDirection = other.currDirection;
  turn = other.turn;
  turnIndex = other.turnIndex;
  despawn = other.despawn;
  leftTurn = other.leftTurn;
  leftIndex = other.leftIndex;
  return *this;
}

VehicleBase::VehicleBase(VehicleBase&& other){
  vehicleID = move(other.vehicleID);
  vehicleType = move(other.vehicleType);
  vehicleDirection = move(other.vehicleDirection);
  currDirection = move(other.currDirection);
  turn = move(other.turn);
  turnIndex = move(other.turnIndex);
  despawn = move(other.despawn);
  leftTurn = move(other.leftTurn);
  leftIndex = move(other.leftIndex);
}

VehicleBase& VehicleBase::operator=(VehicleBase&& other) noexcept{
  if(this == &other){
    return *this;
  }
  vehicleID = move(other.vehicleID);
  vehicleType = move(other.vehicleType);
  vehicleDirection = move(other.vehicleDirection);
  currDirection = move(other.currDirection);
  turn = move(other.turn);
  turnIndex = move(other.turnIndex);
  despawn = move(other.despawn);
  leftTurn = move(other.leftTurn);
  leftIndex = move(other.leftIndex);
  return *this;
}

void  VehicleBase::setTurn(bool val){
  turn = val;
}

void VehicleBase::setDirection(Direction direction){
  currDirection = direction;
}

void VehicleBase::setDespawn(bool val){
  despawn = val;
}


#endif
