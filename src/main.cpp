#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "Object.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "cirQueue.h"
//#include <cassert>
using namespace std;

double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2, double z1, double z2);
sf::Vector3f computeForce(Object objectOne, Object objectTwo);

std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies);


double G = 10; // For testing
double dt = 0.25; // DeltaTime (Change in time) -- increase to make sim go faster , but too much causes it to break (math stuff)

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1100, 600}), "CMake SFML Project");
    sf::View worldView = sf::View();
    worldView.setSize({770, 600});
    worldView.setCenter({385, 300});
    worldView.setViewport(sf::FloatRect({ 0, 0 }, { .7f, 1.f }));
    sf::View uiView = sf::View();
    uiView.setSize({330, 600});
    uiView.setCenter({165, 300});
    uiView.setViewport(sf::FloatRect({.7, 0}, {.3, 1}));
    sf::Font font(std::string(PROJECT_ROOT) + "/arial.ttf");
    sf::Text title(font);
    title.setString("Planet Properties");
    title.setCharacterSize(24);
    title.setOrigin({title.getLocalBounds().size.x/2,title.getLocalBounds().size.y/2});
    title.setPosition({165, 20});

    sf::Text massLabel(font);
    massLabel.setString("Mass: 50kg");
    massLabel.setCharacterSize(24);
    massLabel.setOrigin({ massLabel.getLocalBounds().size.x / 2,massLabel.getLocalBounds().size.y / 2 });
    massLabel.setPosition({ 140, 60 });

    sf::Text posLabel(font);
    posLabel.setString("Distance from Sun: 50AU");
    posLabel.setCharacterSize(24);
    posLabel.setOrigin({ posLabel.getLocalBounds().size.x / 2,posLabel.getLocalBounds().size.y / 2 });
    posLabel.setPosition({ 140, 150 });

    struct Slider {
        sf::RectangleShape track;
        sf::CircleShape    thumb;
        float min = 0.f, max = 100.f, value = 50.f;
        float left = 16.f, top = 80.f, width = 288.f;
    };

    Slider sliders[2];
    Slider& massSlider = sliders[0];
    Slider& posSlider = sliders[1];

    massSlider.track.setSize({ massSlider.width, 6.f });
    massSlider.top = 100;
    massSlider.track.setPosition({ massSlider.left, massSlider.top });
    massSlider.track.setFillColor(sf::Color(80, 80, 90));

    massSlider.thumb.setRadius(10.f);
    massSlider.thumb.setOrigin({ 10.f, 10.f });
    massSlider.thumb.setFillColor(sf::Color(200, 200, 220));

    posSlider.track.setSize({posSlider.width, 6.f});
    posSlider.top = 180;
    posSlider.track.setPosition({ posSlider.left, posSlider.top });
    posSlider.track.setFillColor(sf::Color(80, 80, 90));

    posSlider.thumb.setRadius(10.f);
    posSlider.thumb.setOrigin({ 10.f, 10.f });
    posSlider.thumb.setFillColor(sf::Color(200, 200, 220));

    int sliderBeingDragged = 0;

    auto updateThumb = [&] {
        float t = (sliders[sliderBeingDragged].value - sliders[sliderBeingDragged].min) / (sliders[sliderBeingDragged].max - sliders[sliderBeingDragged].min);
        float x = sliders[sliderBeingDragged].left + t * sliders[sliderBeingDragged].width;
        sliders[sliderBeingDragged].thumb.setPosition({x, sliders[sliderBeingDragged].top + 3.f});
    };

    for (int i = 0; i < std::size(sliders); i++) {
        sliderBeingDragged = i;
        updateThumb();
    }

    bool draggingSlider = false;

    window.setFramerateLimit(1000);

    // changed to 1000 from 20 idk why looks cool
    cirQueue trails = cirQueue(1000);

    // First object - Sun
    double sMass = 1.0; // Mass
    double sX = 385.0; // X Coordinate
    double sY = 300.0; // Y Coordinate
    double sZ = 300.0; // Z Coordinate

    // Second object - Planet A
    double pMass = 0.000003003; // Mass
    double pVeloX = 0.0; // Velocity X Coord
    double pVeloY = 0.223; // Velocity y Coord
    double pVeloZ = 0.225; // Velocity Z Coord
    double pX = 200.0; // X Coordinate
    double pY = 300.0; // Y Coordinate
    double pZ = 300.0;

    // Third Object - Planet B
    double pMass1 = 0.000003003; // Mass
    double pVeloX1 = 0.0; // Velocity X Coord
    double pVeloY1 = 0.223; // Velocity y Coord
    double pVeloZ1 = 0.225;
    double pX1 = 200.0; // X Coordinate
    double pY1 = 200.0; // Y Coordinate
    double pZ1 = 0.225; // Z Coordinate

    // Initialize as many objects as you want! ( N Body System )
    Object sun(sMass, 0, 0, 0, 0, sX, sY, sZ);
    Object earth(pMass, 1, pVeloX, pVeloY, pVeloZ, pX, pY, pZ);
    Object mars(pMass1, 1, pVeloX1, pVeloY1, pVeloZ1, pX1, pY1, pZ1);




    // array of of objects
    std::vector<Object> bodies = {sun, earth, mars};

    // What if we made a loop of objects for shits and giggles ? gpt code cus lazy
    // Add multiple planets
    /**for (int i = 0; i < 10; i++) {
        double angle = i * 36.0 * (M_PI / 180.0); // Spread planets evenly around a circle
        double radius = 150.0 + i * 20.0;         // distance from the sun
        double x = sX + radius * cos(angle);
        double y = sY + radius * sin(angle);

        // Give each planet a tangential velocity for orbit
        double v = 0.15 + i * 0.005;
        double vx = -v * sin(angle);
        double vy = v * cos(angle);

        bodies.push_back(Object(pMass, 1, vx, vy, x, y));
    } // end of gpt slop **/


    // Copy of array of objects (Used for N-body system)
    std::vector<Object> nextBodies = bodies;

    // should this be changed to bodies inside {} ? idk i tried and nothing was different lmk
    std::vector<sf::CircleShape> graphicsBodies = convertBodies({sun, earth});
    int drawNum = 0;

    while (window.isOpen())
    {
        //sf::Event e;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (event->is<sf::Event::MouseButtonPressed>() ) { 
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (int i = 0; i < std::size(sliders); i++) {
                    if (sliders[i].thumb.getGlobalBounds().contains(mousePos)) {
                        sliderBeingDragged = i;
                        draggingSlider = true;
                        break;
                    }
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) { 
                draggingSlider = false;
            }
            if (event->is<sf::Event::MouseMoved>() && draggingSlider) {
                float t = std::clamp((window.mapPixelToCoords(sf::Mouse::getPosition(window)).x - sliders[sliderBeingDragged].left)/sliders[sliderBeingDragged].width, 0.f, 1.f);
                sliders[sliderBeingDragged].value = sliders[sliderBeingDragged].min + t * (sliders[sliderBeingDragged].max - sliders[sliderBeingDragged].min);
                updateThumb();
            }
        }

        drawNum++;
        

        window.clear();
        window.setView(worldView);
        trails.printToScreen(window);
        for (sf::CircleShape body : graphicsBodies) {
            window.draw(body);
        }

        //assert(graphicsBodies[0].getPosition().x == (float)sun.getLocation().x);

        window.setView(uiView);
        // Render UI Here!!!
        sf::RectangleShape uiBg;
        uiBg.setSize({ 330, static_cast<float>(window.getSize().y) });
        uiBg.setPosition({ 0.f, 0.f }); 
        uiBg.setFillColor(sf::Color(30, 30, 35));
        window.draw(uiBg);
        window.draw(title);
        window.draw(massLabel);
        window.draw(posLabel);
        for (Slider i : sliders) {
            window.draw(i.track);
            window.draw(i.thumb);
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
            sf::Vector3f totalForce(0, 0, 0);

            for (int j = 0; j < bodies.size(); j++) {
                if (i == j) {
                    continue;
                }
                // Add the force to total force
                totalForce += computeForce(bodies[i], bodies[j]);
            }

            // Update velocity & position
            nextBodies[i].velocityUpdate(totalForce.x, totalForce.y, totalForce.z, dt);
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
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2, double z1, double z2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}

// Computes the force between two objects
// was originally my code but was like 90% there and the final 10% of the math was confusing so gpt did rest
sf::Vector3f computeForce(Object objectOne, Object objectTwo) {
    double dx = objectTwo.getLocation().x - objectOne.getLocation().x;
    double dy = objectTwo.getLocation().y - objectOne.getLocation().y;
    double dz = objectTwo.getLocation().z - objectOne.getLocation().z;
    double r = distanceBetweenTwoObjects(objectOne.getLocation().x, objectOne.getLocation().y,
                                         objectTwo.getLocation().x, objectTwo.getLocation().y, objectOne.getLocation().z, objectTwo.getLocation().z);
    if (r == 0) return {0, 0, 0}; // avoid division by zero

    double force = computeGForce(objectOne.getMass(), objectTwo.getMass(), r);
    // direction from one to two
    double fx = force * (dx / r);
    double fy = force * (dy / r);
    double fz = force * (dz / r);

    // acceleration of objectOne = F / m1
    double ax = fx / objectOne.getMass();
    double ay = fy / objectOne.getMass();
    double az = fz / objectOne.getMass();

    return sf::Vector3f(ax, ay, az);
}


// Makes vector of planet graphics objects
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
