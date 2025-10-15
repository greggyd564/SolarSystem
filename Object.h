//
// Created by Stone Stephenson.
//

// Notes:
// - Needs function syntax (final step)

#ifndef OBJECT_H
#define OBJECT_H
struct pos { // X/Y Coordinates for Position & Velocity
    double x, y;
};


class Object {
    public:
        // Constructor
        Object(double mass, double veloX, double veloY, double x, double y);

        // Getters
        double getMass();
        pos getVelocity();4
        pos getLocation();

        // Setters
        void setMass(double newMass);
        void setVelocity(pos newVelo);
        void setLocation(pos newLocation);

        // Updates velocity
        void velocityUpdate(double accelerationX, double accelerationY, double deltaT);
        // updates Position
        void updatePosition(double deltaT);


    private:
      double mass;
      pos location;
      pos velocity;


};



#endif //OBJECT_H
