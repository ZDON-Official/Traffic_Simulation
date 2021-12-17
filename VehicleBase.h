#ifndef __VEHICLE_BASE_H__
#define __VEHICLE_BASE_H__

// enum: see http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-enum
enum class Direction   {north, south, east, west};
enum class VehicleType {car, suv, truck};
enum class LightColor  {green, yellow, red};

class VehicleBase
{
   public:
      static int vehicleCount;

   private:
      int         vehicleID;
      VehicleType vehicleType;
      Direction   vehicleDirection;
      Direction   currDirection;
      bool turn;
      int turnIndex;
      bool despawn;
      bool leftTurn;
      int leftIndex = 0;

   public:
      VehicleBase(VehicleType type, Direction originalDirection);
      VehicleBase(const VehicleBase& other);
      ~VehicleBase();
      VehicleBase& operator=(const VehicleBase& other);
      VehicleBase(VehicleBase&& other);
      VehicleBase& operator=(VehicleBase&& other) noexcept;


      inline int getVehicleID() const { return this->vehicleID; }

      inline VehicleType getVehicleType() const { return this->vehicleType; }
      inline Direction   getVehicleOriginalDirection() const { return this->vehicleDirection; }

      Direction getCurrDirection(){ return this->currDirection; }
      int getIndex(){ return this ->turnIndex;}

      // for making a turn
      bool isTurning(){ return this->turn;}
      void setTurn(bool val);
      void setDirection(Direction direction);
      void resetIndex(){turnIndex = 0;}
      void addIndex(){turnIndex++;}

      // for despawn
      bool getDespawn(){ return this->despawn;}
      void setDespawn(bool val);

      //for left turn
      bool getLeft(){return this->leftTurn;}
      void switchLeft(){leftTurn = !leftTurn;}
      void resetLeftIndex() { leftIndex = 0; }
      void addLeftIndex() { leftIndex++; }
      int getLeftIndex(){return this->leftIndex;}
};

#endif
