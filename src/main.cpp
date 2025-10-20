#include <SFML/Graphics.hpp>
#include "Object.h"
#include <vector>

double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2);
std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies);


double G = 1.0; // For testing
double dt = 0.1; // DeltaTime (Change in time)

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({800, 600}), "CMake SFML Project");
    window.setFramerateLimit(100);


    // First object - Sun
    double sMass = 1.0; // Mass
    double sX = 400.0; // X Coordinate
    double sY = 300.0; // Y Coordinate

    // Second object - Planet A
    double pMass = 0.000003003; // Mass
    double pVeloX = 0.0; // Velocity X Coord
    double pVeloY = 1.0; // Velocity y Coord
    double pX = 200.0; // X Coordinate
    double pY = 300.0; // Y Coordinate

    // Initialize the Sun & the Earth ( 2 Body System )
    Object sun(sMass, 0, 0, 0, sX, sY);
    Object earth(pMass, 1, pVeloX, pVeloY, pX, pY);

    std::vector<Object> bodies = {sun, earth};

    std::vector<sf::CircleShape> graphicsBodies = convertBodies({sun, earth});

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        for (sf::CircleShape body : graphicsBodies) {
            window.draw(body);
        }
        window.display();

        double r = distanceBetweenTwoObjects(sun.getLocation().x, sun.getLocation().y, earth.getLocation().x, earth.getLocation().y);
        double force = computeGForce(sun.getMass(), earth.getMass(), r);
        double dx = earth.getLocation().x - sun.getLocation().x;
        double dy = earth.getLocation().y - sun.getLocation().y;
        double forceVectorX = force * (dx / r);
        double forceVectorY = force * (dy / r);
        double accelerationEarthX = computeAcceleration(forceVectorX, earth.getMass());
        double accelerationEarthY = computeAcceleration(forceVectorY, earth.getMass());
        double accelerationSunX = computeAcceleration(-forceVectorX, sun.getMass());
        double accelerationSunY = computeAcceleration(-forceVectorY, sun.getMass());
        earth.velocityUpdate(accelerationEarthX, accelerationEarthY, dt);
        sun.velocityUpdate(accelerationSunX, accelerationSunY, dt);
        earth.updatePosition(dt);
        sun.updatePosition(dt);
        graphicsBodies = convertBodies({ sun, earth });
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
