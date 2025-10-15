#include <iostream>
#include <SFML/Graphics.hpp>
#include "Object.h"
using namespace std;

// The goal of this simulation is to visually represent the orbit of a planet around the sun.

/** Current Formulas:
// Something to note is that in our simulation we use most of these equations twice to get a vector. This is because our
// Simulation is in 2D therefore we need a change in X & Y.
// VELOCITY: dx / dt = v "the rate of change of position with respect to time is velocity."
// GRAVITATIONAL FORCE: F = G ( ( m1 * m2 ) / r^2 ) cc: https://en.wikipedia.org/wiki/Newton's_law_of_universal_gravitation
// ACCELERATION: a = F / m "force divided by mass"
// NEWTON'S SECOND LAW: F = m * a
// DISTANCE BETWEEN TWO POINTS p = sqrt( ( (x2 - x1)^2 + (y2 - y1)^2 ) "length of the vector"**/

/** How It Works:
// Gather Initial parameters of two objects (Mass, Velocity, and Position)
// Use the Distance Between Two Points Formula to calculate the distance (r)
// Use the Masses of both objects & r to compute the Force Magnitude ( Then split into direction , ie a vector )
// Use that vector to calculate the acceleration of each Object ( inverse but equal due to N3L )
// Use that acceleration to update the velocity of both objects
// Use the velocity to update the position
// Uses Delta-t as time which essentially is multiplied by to represent the change over time.
// Then the visualization changes by basically plotting that change in position on a graph**/

/** NEXT STEPS:
// - Figure out parameters that don't slingshot planets into space
// - Create a 2D Motion Visualization ( CURRENTLY IN PROGRESS )
// - Figure out N-Body System
// - Make it 3D
// - Make it look good
// - Website / App integration
// - Allow adjusting parameters / adding planets from the site ( UX / UI )
// - Adding things other than planets ( ie multiple suns, black holes, asteroid belts )
// - Showing collisions ( Would be cool )  **/

// Computation Functions
double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2);


// Constants
// double G = 6.67430e-11; // Universal Gravitational Constant
double G = 1.0; // For testing
double dt = 0.1; // DeltaTime (Change in time)


int main() {
    cout << "Welcome to Planet Orbit Simulator" << endl; // Intro

    // Using SF, create an instance of the RenderWindow class called 'window' that is 800 x 600 called 'Solar System Simulation'
    /**sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Solar System Simulation");

    // Set the frame limit to 60FPS for now.
    window.setFramerateLimit(60);


    while (window.isOpen()) {
        // pollEvent now returns std::optional<sf::Event>
        while (auto event = window.pollEvent()) { // Awaits input
            if (event->is<sf::Event::Closed>()) { // If the window gets closed, exit the loop
                window.close();
            }
        }

        window.clear(); // Clears screen (Makes it black)
        window.display(); // Sends drawings to screen
        sf::Text text("hello");
    }
**/


    // VARIABLES -- Currently arbitrary values.

    // First object - Sun
    double sMass = 1.0; // Mass
    double sX = 0.0; // X Coordinate
    double sY = 0.0; // Y Coordinate

    // Second object - Planet A
    double pMass = 0.000003003; // Mass
    double pVeloX = 0.0; // Velocity X Coord
    double pVeloY = 1.0; // Velocity y Coord
    double pX = 1.0; // X Coordinate
    double pY = 0.0; // Y Coordinate

    // Initialize the Sun & the Earth ( 2 Body System )
    Object sun( sMass,  0,  0, sX,  sY);
    Object earth(pMass, pVeloX, pVeloY, pX, pY);

    // In a loop of incrementing time ( Delta - T ): (Commented out the output lines)
    for (int step = 0; step < 100; step++) {
        double t = step * dt;
        cout << "t = " << t << endl;

        //cout << "Sun's current data: " << endl
        //<< "Mass: " << sun.getMass() << endl
        //<< "Velocity: (" << sun.getVelocity().x << ", " << sun.getVelocity().y << ")" << endl
        //<< "Position: (" << sun.getLocation().x << ", " << sun.getLocation().y << ")" << endl;

        //cout << "Earth's current data: " << endl
        //<< "Mass: " << earth.getMass() << endl
        //<< "Velocity: (" << earth.getVelocity().x << ", " << earth.getVelocity().y << ")" << endl
        //<< "Position: (" << earth.getLocation().x << ", " << earth.getLocation().y << ")" << endl;



        // Calculate the distance between the two planets ( Gives us r for the force formula)
        double r = distanceBetweenTwoObjects(sun.getLocation().x, sun.getLocation().y, earth.getLocation().x, earth.getLocation().y);

        //cout << "The distance between the two objects is : " << r << endl;

        // Use the Mass of Both Objects  & R to compute the magnitude of the force
        double force = computeGForce(sun.getMass(), earth.getMass(), r);

        // Take the Force Magnitude and split it by direction, giving us a vector:
        // ( Gives us both parameters to then use to find the acceleration of both objects )
        double dx = earth.getLocation().x - sun.getLocation().x;
        double dy = earth.getLocation().y - sun.getLocation().y;
        double forceVectorX = force * ( dx / r );
        double forceVectorY = force * ( dy / r );

        //cout << "The force vector is (" << forceVectorX << ", " << forceVectorY << ")" << endl;

        // Use the that G-Force Vector in the Acceleration Formula for both objects to get the Acceleration Vector
        // Earth
        double accelerationEarthX = computeAcceleration(forceVectorX, earth.getMass());
        double accelerationEarthY = computeAcceleration(forceVectorY, earth.getMass());

        //cout << "The acceleration vector of the Earth is (" << accelerationEarthX << ", " << accelerationEarthY << ")" << endl;

        // Sun , using negative force values because we want equal but opposite values (Newton's 3rd Law)
        double accelerationSunX = computeAcceleration(-forceVectorX, sun.getMass());
        double accelerationSunY = computeAcceleration(-forceVectorY, sun.getMass());

        //cout << "The acceleration vector of the Sun is (" << accelerationSunX << ", " << accelerationSunY << ")" << endl;

        // Update Velocity given the acceleration
        earth.velocityUpdate(accelerationEarthX, accelerationEarthY, dt);
        sun.velocityUpdate(accelerationSunX, accelerationSunY, dt);

        //cout << "The updated velocity of the Earth is (" << earth.getVelocity().x << ", " << earth.getVelocity().y << ")" << endl;
        //cout << "The updated velocity of the Sun is (" << sun.getVelocity().x << ", " << sun.getVelocity().y << ")" << endl;

        // Update position given the velocity
        earth.updatePosition(dt);
        sun.updatePosition(dt);

        //cout << "The updated position of the Earth is (" << earth.getLocation().x << ", " << earth.getLocation().y << ")" << endl;
        //cout << "The Updated position of the Sun is (" << sun.getLocation().x << ", " << sun.getLocation().y << ")" << endl << endl << endl;

        // Updates graph / Visualizer ( not implemented, math only right now )

        // Example: draw a 40x20 grid , currently throws Earth out of Orbit
        for (int y = -10; y < 10; y++) {
            for (int x = -20; x < 20; x++) {
                if ((int)earth.getLocation().x == x && (int)earth.getLocation().y == y)
                    cout << "E";
                else if ((int)sun.getLocation().x == x && (int)sun.getLocation().y == y)
                    cout << "S";
                else
                    cout << ".";
            }
            cout << "\n";
        }
    }

    return 0;
}

// Computes g force. May turn into setter/getter in class ?
double computeGForce(double m1, double m2, double r) {
    return G * ( ( m1 * m2 ) / (r * r) );
}

double computeAcceleration(double F, double m) {
    return F / m;
}

// Computes and returns the distance between two objects
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
