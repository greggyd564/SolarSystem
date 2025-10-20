#ifndef OBJECT_H
#define OBJECT_H
struct pos { // X/Y Coordinates for Position & Velocity
    double x, y;
};


class Object {
public:
    // Constructor
    Object(double mass, int type, double veloX, double veloY, double x, double y);

    // Getters
    double getMass();
    pos getVelocity(); 
    pos getLocation();
    int getType();

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
    int type; // 0 is sun, 1 is planet

};



#endif //OBJECT_H
