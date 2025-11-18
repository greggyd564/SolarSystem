#include "Object.h"

// constructor
Object::Object(double mass, int type, double veloX, double veloY, double veloZ, double x, double y, double z) {
    this->mass = mass;
    this->type = type;
    velocity = { veloX, veloY, veloZ};
    location = { x, y, z };
}

// Getter for location
pos Object::getLocation() {
    return location;
}

// Getter for mass
double Object::getMass() {
    return mass;
}

//Getter for velocity
pos Object::getVelocity() {
    return velocity;
}

// Getter for type
int Object::getType() {
    return type;
}

// Setter for location
void Object::setLocation(pos newLocation) {
    location = newLocation;
}

// Setter for mass
void Object::setMass(double newMass) {
    mass = newMass;
}

// Setter for Velocity
void Object::setVelocity(pos newVelo) {
    velocity = newVelo;
}

// Updates the Velocity
void Object::velocityUpdate(double accelerationX, double accelerationY, double accelerationZ, double deltaT) {
    velocity.x += accelerationX * deltaT;
    velocity.y += accelerationY * deltaT;
    velocity.z += accelerationZ * deltaT;
}

void Object::updatePosition(double deltaT) {
    location.x += velocity.x * deltaT;
    location.y += velocity.y * deltaT;
    location.z += velocity.z * deltaT;
}

