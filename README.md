# About
------------------
This project is a traffic simulation which uses an animator to
display vehicles, traffic lights, an intersection, and lanes of traffic.
The vehicles include cars, SUVs, and trucks.

This was implemented by Zohaib and Rinki.
This is version 1 of the project.

# Getting Started!
------------------
Untar the tar file. This tar file includes a makefile, therefore
enter "make" into the command line to compile the necessary code.

Next, to execute the code in the command line you must follow the following format:
"./simulate input_file_format.txt 45"

The first input is the executable file created in our make command,
the second input is a file which includes input parameters,
and the third input is any long value.

# Project Design
------------------
## Main, Simulation, and Vehicle Creation
Our Main file checks for the correct command line inputs for execution.
The parameter file and the long are passed to our Simulation file.

The simulation file consists of many different cohesive methods. We use the
parameter file to set the traffic lights to their according location.

We create and spawn vehicles based on their probability of vehicle types,
their initial directions, and keep track of these vehicles in a vector of
vehicleBase pointers. Additionally, vehicle IDs are also kept track of
in a vector of integers.

Vehicles have indices of where they exist on the road, their original direction,
and their current direction as attributes, among other attributes.


## Spawning and Rolling
The spawn method and roll method help insert a car into sections of the road.
Both work with integers keeping track of the number of vehicles fully spawned
or fully rolled in order to add vehicles in the order of creation and in
the increasing numerical order of their vehicle IDs. They also use an integer
pointerCounter to check how many pointers are assigned to a vehicle as it's
being spawned/rolled.

Throughout the project, we make sure that the vehicle's current direction
is considered by a temporary vehicleBase pointer vector that is assigned
to the road that corresponds to the vehicle's current direction to assign pointers.
After road section pointers of the vehicle's direction are manipulated, we reassign
the road vehicleBase pointer vectors to the value's in the temporary vector.
This way the changes in pointers persists beyond the scope of the methods.

### Differences Between Spawning and Rolling
The spawn method specifically places vehicles on empty road sections without
placing vehicles on the intersection. The roll method places vehicles
section by section on the end of the road if the end of the road is empty for
each section to roll in. We have multiple vehicles spawn before vehicles roll in.

Rolling works by checking the number of pointers are to a vehicle by a temporary
integer and making sure front sections rolled before the back sections. If a
vehicle was able to fully roll, we increment an integer representing the
indices of our vector to move on the next vehicle to fully roll.
This prevents vehicles from remaining partially rolled.
We roll each vehicle in order.

### Despawning After Spawning / Fully Rolling
We added a despawn attribute to vehicleBases to make sure cars that have
spawned/fully rolled into the road then sucessfully rolled off the road
can be kept track of. This prevents the code from accessing any vehicle that
have despawned and are no longer needed to preform any changes to.



## Moving and Turning
Both moving and turning keep track of the traffic light corresponding to the
direction it's currently traveling in by comparing the variables of string lights
that indicate the color of a particular light at a given time. We use this string
and the vehicle's direction to compare if a move or turn is possible.

We use a temporary vehicleBase pointer vector when manipulating pointers of the
road sections to vehicles as we did in spawning/rolling methods.

We keep track of where a given vehicle is by looping through the road sections
to see where the first index of a vehicle is on the road.

Moving / turning only occurs when a given vehicle exists on the road to move.

### Moving
Our method that makes a forward move calls on a method to check if a
forward move is possible.
A move is deemed possible if the following cases are satisfied:
- the vehicle is beyond the intersection and there is no vehicle ahead of it
- the vehicle has a green light and there is no vehicle ahead of it
- the vehicle is before the intersection on a red or yellow light and there is no
vehicle ahead of it
- any section of the vehicle is on the intersection regardless of the light color
- the vehicle is not in the process of turning
- the vehicle is not before the intersection on a red light

If a forward move is possible, we point the last section of the vehicle to nothing
and the next section to the vehicle to simulate a forward move. If the vehicle
has moved off of the screen fully, we despawn the vehicle.

### Turning Right
Our method that makes a right turn calls on a method to check if a
right turn is possible.

A turn if deemed possible if the following cases are satisfied:
- a turn is not already in progress for the vehicle
- the vehicle has a green or yellow light
- the vehicle is in the section right before the intersection
- there is no vehicle on the intersection ahead of the vehicle
- the lane the vehicle is turning onto has space after the intersection
  to allow the vehicle to turn onto the new road direction

If a right turn is possible, we keep track of the vehicle's direction, index,
if a turn is in progress, manipulating the index of a vehicle when turning from
one direction to another direction. When a vehicle turns we have additional temporary
vehicleBase pointer vectors corresponding to the road direction that the vehicle
turned onto, and the road direction that the vehicle turned from.

### Turning Left
Similarly, our method that makes a left turn calls on a method to check if a
left turn is possible.

A turn if deemed possible if the following cases are satisfied:
- a turn is not already in progress for the vehicle
- the vehicle has a green light
- the vehicle is in the section right before the intersection
- there is no vehicle on the intersection ahead of the vehicle
- the lane the vehicle is turning onto has space after and at the intersection
  to allow the vehicle to turn onto the new road direction

If a left turn is possible, we keep track of the vehicle's direction, index,
if a turn is in progress, manipulating the index of a vehicle when turning from
one direction to another direction. When a vehicle turns we have additional temporary
vehicleBase pointer vectors corresponding to the road direction that the vehicle
turned onto, and the road direction that the vehicle turned from.

## Copy Semantics, Move Semantics, Additional Pointers
VehicleBase objects implement copy constructor, copy assignment operator, move constructor,
move assignment operator, passing by rvalue and lvalue. This helps the correct
information be copied/moved as needed when manipulating vehicles. Pointers were heavily
used throughout this project. We use many vectors of vehicleBase pointers, and
we temporarily store the address of a vehicle which gets assigned the
dereferenced vehicle in our vehicleBase pointer vector keeping track of vehicles.
This makes sure changes to the vehicle persists outside the scope of methods,
within our vectors keeping track of vehicles, and the road sections pointer manipulations
persist beyond the scope of our methods.

## Testing
To run a few basic tests run the following commands:
"g++ -std=c++17 test.cpp -o test Animator.cpp"
"./test"
