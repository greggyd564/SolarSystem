#include <SFML/Graphics.hpp>
#include "Object.h"
#include <vector>
#include "cirQueue.h"
#include <iostream>

using namespace std;

double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2);
sf::Vector2f computeForce(Object objectOne, Object objectTwo);

std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies);


double G = 10; // For testing
double dt = 0.25; // DeltaTime (Change in time) -- increase to make sim go faster , but too much causes it to break (math stuff)

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({800, 600}), "CMake SFML Project");
    window.setFramerateLimit(1000);

    // changed to 1000 from 20 idk why looks cool
    cirQueue trails = cirQueue(1000);

    // First object - Sun
    double sMass = 1.0; // Mass
    double sX = 400.0; // X Coordinate
    double sY = 300.0; // Y Coordinate

    // Second object - Planet A
    double pMass = 0.000003003; // Mass
    double pVeloX = 0.0; // Velocity X Coord
    double pVeloY = 0.223; // Velocity y Coord
    double pX = 200.0; // X Coordinate
    double pY = 300.0; // Y Coordinate

    // Third Object - Planet B
    double pMass1 = 0.000003003; // Mass
    double pVeloX1 = 0.0; // Velocity X Coord
    double pVeloY1 = 0.223; // Velocity y Coord
    double pX1 = 200.0; // X Coordinate
    double pY1 = 200.0; // Y Coordinate

    // Initialize as many objects as you want! ( N Body System )
    Object sun(sMass, 0, 0, 0, sX, sY);
    Object earth(pMass, 1, pVeloX, pVeloY, pX, pY);
    Object mars(pMass1, 1, pVeloX1, pVeloY1, pX1, pY1);




    // array of of objects
    std::vector<Object> bodies = {sun, earth, mars};

    // What if we made a loop of objects for shits and giggles ? gpt code cus lazy
    // Add multiple planets
    for (int i = 0; i < 10; i++) {
        double angle = i * 36.0 * (M_PI / 180.0); // Spread planets evenly around a circle
        double radius = 150.0 + i * 20.0;         // distance from the sun
        double x = sX + radius * cos(angle);
        double y = sY + radius * sin(angle);

        // Give each planet a tangential velocity for orbit
        double v = 0.15 + i * 0.005;
        double vx = -v * sin(angle);
        double vy = v * cos(angle);

        bodies.push_back(Object(pMass, 1, vx, vy, x, y));
    } // end of gpt slop


    // Copy of array of objects (Used for N-body system)
    std::vector<Object> nextBodies = bodies;

    // should this be changed to bodies inside {} ? idk i tried and nothing was different lmk
    std::vector<sf::CircleShape> graphicsBodies = convertBodies({sun, earth});
    int drawNum = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        drawNum++;
        

        window.clear();
        trails.printToScreen(window);
        for (sf::CircleShape body : graphicsBodies) {
            window.draw(body);
        }
        
        window.display();

        // changed to 100 looks cool idk
        if (drawNum == 100) {
            drawNum = 0;
            for (int l = 0; l < bodies.size(); l++) {
                graphicsBodies[l].setFillColor(sf::Color::White);
                trails.push(graphicsBodies[l]);
            }
        }

        // For each object in the array, compute the force and add it to the total.
        for (int i = 0; i < bodies.size(); i++) {
            // Initialize total force to 0.
            sf::Vector2f totalForce(0, 0);

            for (int j = 0; j < bodies.size(); j++) {
                if (i == j) {
                    continue;
                }
                // Add the force to total force
                totalForce += computeForce(bodies[i], bodies[j]);
            }

            // Update velocity & position
            nextBodies[i].velocityUpdate(totalForce.x, totalForce.y, dt);
            nextBodies[i].updatePosition(dt);
        }
        // set old to new bodies
        bodies = nextBodies;

        // Probably can delete, saving until it works for n body system
        // update: ok it works but i dont actually know if the math is accurate , gpt says yes but waiting till i write actual test cases to confirm
        /**double r = distanceBetweenTwoObjects(sun.getLocation().x, sun.getLocation().y, earth.getLocation().x, earth.getLocation().y);
        double force = computeGForce(sun.getMass(), earth.getMass(), r);
        double dx = earth.getLocation().x - sun.getLocation().x;
        double dy = earth.getLocation().y - sun.getLocation().y;
        double forceVectorX = -force * (dx / r);
        double forceVectorY = -force * (dy / r);
        double accelerationEarthX = computeAcceleration(forceVectorX, earth.getMass());
        double accelerationEarthY = computeAcceleration(forceVectorY, earth.getMass());
        double accelerationSunX = computeAcceleration(-forceVectorX, sun.getMass());
        double accelerationSunY = computeAcceleration(-forceVectorY, sun.getMass());
        earth.velocityUpdate(accelerationEarthX, accelerationEarthY, dt);
        sun.velocityUpdate(accelerationSunX, accelerationSunY, dt);
        earth.updatePosition(dt);
        sun.updatePosition(dt);**/


        graphicsBodies = convertBodies(bodies);

    }
}


// Computes g force. May turn into setter/getter in class ?
double computeGForce(double m1, double m2, double r) {
    return G * ((m1 * m2) / (r * r));
}

double computeAcceleration(double F, double m) {
    return F / m;
}

// Computes and returns the distance between two objects
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Computes the force between two objects
// was originally my code but was like 90% there and the final 10% of the math was confusing so gpt did rest
sf::Vector2f computeForce(Object objectOne, Object objectTwo) {
    double dx = objectTwo.getLocation().x - objectOne.getLocation().x;
    double dy = objectTwo.getLocation().y - objectOne.getLocation().y;
    double r = distanceBetweenTwoObjects(objectOne.getLocation().x, objectOne.getLocation().y,
                                         objectTwo.getLocation().x, objectTwo.getLocation().y);
    if (r == 0) return {0, 0}; // avoid division by zero

    double force = computeGForce(objectOne.getMass(), objectTwo.getMass(), r);
    // direction from one to two
    double fx = force * (dx / r);
    double fy = force * (dy / r);

    // acceleration of objectOne = F / m1
    double ax = fx / objectOne.getMass();
    double ay = fy / objectOne.getMass();

    return sf::Vector2f(ax, ay);
}


// Greg's algorithm for implementation assignment
std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies) {
    std::vector<sf::CircleShape> graphicsBodies = {};
    for (Object body : bodies) {
        sf::CircleShape graphicsBody;
        int bodyType = body.getType(); // type: 0 = sun, 1 = planet
        if (bodyType == 0) {
            graphicsBody.setRadius(20);
            graphicsBody.setFillColor(sf::Color::Yellow);
        }
        else if (bodyType == 1) {
            graphicsBody.setRadius(10);
            graphicsBody.setFillColor(sf::Color::Green);
        }
        pos bodyLoc = body.getLocation();
        graphicsBody.setPosition({ (float)bodyLoc.x,(float)bodyLoc.y });
        graphicsBodies.push_back(graphicsBody);
    }
    return graphicsBodies;
}
