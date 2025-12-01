#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "Object.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "cirQueue.h"
//#include <cassert>
using namespace std;

// Helper: normalize a vector
sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0.f)
        return sf::Vector2f(0.f, 0.f);
    return sf::Vector2f(v.x / len, v.y / len);
}

double get_speed(Object obj) {
    float vx = (float)obj.getVelocity().x;
    float vy = (float)obj.getVelocity().y;
    return (double)std::sqrt(vx * vx + vy * vy);
}

double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2);
sf::Vector2f computeForce(Object objectOne, Object objectTwo);

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
    sf::Texture texPlanet;
    texPlanet.loadFromFile(std::string(PROJECT_ROOT) + "/resources/planet_64.png");
    texPlanet.setSmooth(true);
    sf::Texture texSun;
    texSun.loadFromFile(std::string(PROJECT_ROOT) + "/resources/sun_64.png");
    texSun.setSmooth(true);
    sf::Font font(std::string(PROJECT_ROOT) + "/resources/arial.ttf");
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

    sf::Text velLabel(font);
    velLabel.setString("Speed: 0.1 km/s");
    velLabel.setCharacterSize(24);
    velLabel.setOrigin({ velLabel.getLocalBounds().size.x / 2,
                         velLabel.getLocalBounds().size.y / 2 });
    velLabel.setPosition({ 140, 220 });

    struct Slider {
        sf::RectangleShape track;
        sf::CircleShape    thumb;
        float min = 0.f, max = 100.f, value = 50.f;
        float left = 16.f, top = 80.f, width = 288.f;
    };

    Slider sliders[3];
    Slider& massSlider = sliders[0];
    Slider& posSlider = sliders[1];
    Slider& velSlider = sliders[2];   // NEW

    massSlider.min = 0.000001;
    massSlider.max = 0.000100;
    massSlider.value = 0.000003003;
    massSlider.track.setSize({ massSlider.width, 6.f });
    massSlider.top = 100;
    massSlider.track.setPosition({ massSlider.left, massSlider.top });
    massSlider.track.setFillColor(sf::Color(80, 80, 90));

    massSlider.thumb.setRadius(10.f);
    massSlider.thumb.setOrigin({ 10.f, 10.f });
    massSlider.thumb.setFillColor(sf::Color(200, 200, 220));

    posSlider.min = 3;
    posSlider.max = 75;
    posSlider.track.setSize({posSlider.width, 6.f});
    posSlider.top = 180;
    posSlider.track.setPosition({ posSlider.left, posSlider.top });
    posSlider.track.setFillColor(sf::Color(80, 80, 90));

    posSlider.thumb.setRadius(10.f);
    posSlider.thumb.setOrigin({ 10.f, 10.f });
    posSlider.thumb.setFillColor(sf::Color(200, 200, 220));

    velSlider.min = 0.001f;
    velSlider.max = 1.0f;
    velSlider.value = 0.1f;
    velSlider.top = 260.f;
    velSlider.left = 16.f;
    velSlider.width = 288.f;

    velSlider.track.setSize({ velSlider.width, 6.f });
    velSlider.track.setPosition({ velSlider.left, velSlider.top });
    velSlider.track.setFillColor(sf::Color(80, 80, 90));

    velSlider.thumb.setRadius(10.f);
    velSlider.thumb.setOrigin({ 10.f, 10.f });
    velSlider.thumb.setFillColor(sf::Color(200, 200, 220));

    int sliderBeingDragged = 0;

    auto updateThumb = [&] {
        float t = (sliders[sliderBeingDragged].value - sliders[sliderBeingDragged].min) / (sliders[sliderBeingDragged].max - sliders[sliderBeingDragged].min);
        float x = sliders[sliderBeingDragged].left + t * sliders[sliderBeingDragged].width;
        sliders[sliderBeingDragged].thumb.setPosition({x, sliders[sliderBeingDragged].top + 3.f});
        if (sliderBeingDragged == 0) {
            massLabel.setString("Mass: " + to_string(sliders[sliderBeingDragged].value * 100000) + " kg");
        }
        else if (sliderBeingDragged == 1) {
            posLabel.setString("Distance From Sun: " + std::to_string(static_cast<int>(sliders[sliderBeingDragged].value)) + " AU");
        }
        else if (sliderBeingDragged == 2) {
            velLabel.setString("Speed: " + std::to_string(sliders[2].value) + " km/s");
        }
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
        double angle = i * 36.0 * (3.14 / 180.0); // Spread planets evenly around a circle
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
    // Probably should be (bodies), but it doesn't matter because of frame rate
    std::vector<sf::CircleShape> graphicsBodies = convertBodies({sun, earth});
    int drawNum = 0;
    int targetedPlanetIdx = 1;

    bool paused = false;                  // *** ADDED: pause flag

    while (window.isOpen())
    {
        //sf::Event e;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // *** ADDED: toggle pause with P
            if (event->is<sf::Event::KeyPressed>()) {                 // *** ADDED
                const auto &kp = event->getIf<sf::Event::KeyPressed>(); // *** ADDED
                if (kp->scancode == sf::Keyboard::Scan::P) {           // *** ADDED
                    paused = !paused;                                 // *** ADDED
                    std::cout << (paused? "PAUSED\n"                // *** ADDED
                                        : "RESUMED\n");              // *** ADDED
                }                                                     // *** ADDED
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (int i = 0; i < std::size(sliders); i++) {
                    if (sliders[i].thumb.getGlobalBounds().contains(mousePos)) {
                        sliderBeingDragged = i;
                        draggingSlider = true;
                        break;
                    }
                }
                mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), worldView);
                for (int i = 1; i < std::size(graphicsBodies); i++) {
                    if (graphicsBodies[i].getGlobalBounds().contains(mousePos)){
                        targetedPlanetIdx = i;
                        massSlider.value = bodies[i].getMass();
                        massLabel.setString("Mass: " + to_string(sliders[0].value * 100000) + " kg");
                        float t = (sliders[0].value - sliders[0].min) / (sliders[0].max - sliders[0].min);
                        float x = sliders[0].left + t * sliders[0].width;
                        sliders[0].thumb.setPosition({ x, sliders[0].top + 3.f });
                    }
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                if (draggingSlider == true) {
                    draggingSlider = false;
                    if (sliderBeingDragged == 0) {
                        nextBodies[targetedPlanetIdx].setMass(sliders[0].value);
                    }
                    // Deal with updating the position
                    else if (sliderBeingDragged == 1) {
                        sf::Vector2f dir = graphicsBodies[targetedPlanetIdx].getPosition() - graphicsBodies[0].getPosition();
                        dir = normalize(dir);
                        dir = graphicsBodies[0].getPosition() + dir * sliders[1].value * 5.f;
                        nextBodies[targetedPlanetIdx].setLocation({ dir.x, dir.y });
                    }
                    else if (sliderBeingDragged == 2) {
                        // Get targeted planet's velocity
                        // Find the angle with std::atan2(vy, vx)
                        // Convert to x,y with that angle and slider[2]'s value
                        // Apply changes to nextBodies
                        pos vel = bodies[targetedPlanetIdx].getVelocity();
                        float ang = (float)std::atan2(vel.y, vel.x);
                        float mag = sliders[2].value;
                        nextBodies[targetedPlanetIdx].setVelocity({ std::cos(ang) * mag, std::sin(ang) * mag });
                    }
                }
            }
            if (event->is<sf::Event::MouseMoved>() && draggingSlider) {
                float t = std::clamp((window.mapPixelToCoords(sf::Mouse::getPosition(window)).x - sliders[sliderBeingDragged].left)/sliders[sliderBeingDragged].width, 0.f, 1.f);
                sliders[sliderBeingDragged].value = sliders[sliderBeingDragged].min + t * (sliders[sliderBeingDragged].max - sliders[sliderBeingDragged].min);
                updateThumb();
            }
        }

        drawNum++;
        
        if (!draggingSlider) {
            sf::Vector2f dir = graphicsBodies[targetedPlanetIdx].getPosition() - graphicsBodies[0].getPosition();
            sliders[1].value = std::clamp(sqrtf(dir.x * dir.x + dir.y * dir.y) / 5, sliders[1].min, sliders[1].max);
            float t = (sliders[1].value - sliders[1].min) / (sliders[1].max - sliders[1].min);
            float x = sliders[1].left + t * sliders[1].width;
            sliders[1].thumb.setPosition({ x, sliders[1].top + 3.f });
            posLabel.setString("Distance From Sun: " + std::to_string(static_cast<int>(sliders[1].value)) + " AU");
            sliders[2].value = std::clamp((float)get_speed(bodies[targetedPlanetIdx]), sliders[2].min, sliders[2].max);
            t = (sliders[2].value - sliders[2].min) / (sliders[2].max - sliders[2].min);
            x = sliders[2].left + t * sliders[2].width;
            sliders[2].thumb.setPosition({ x, sliders[2].top + 3.f });
            velLabel.setString("Speed: " + std::to_string(sliders[2].value) + " km/h");
        }

        window.clear();
        window.setView(worldView);
        trails.printToScreen(window);
        int draw_loop_idx = 0;
        for (sf::CircleShape body : graphicsBodies) {
            if (draw_loop_idx == 0) {
                body.setTexture(&texSun);
            }
            else {
                body.setTexture(&texPlanet);
            }
            if (draw_loop_idx == targetedPlanetIdx) {
                body.setFillColor(sf::Color::Blue);
            }
            window.draw(body);
            draw_loop_idx++;
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
        window.draw(velLabel);
        for (Slider i : sliders) {
            window.draw(i.track);
            window.draw(i.thumb);
        }

        window.display();

        // *** ADDED: if paused, skip physics update and go to next frame
        if (paused) {                                // *** ADDED
            graphicsBodies = convertBodies(bodies);  // *** ADDED (keep visuals in sync)
            drawNum = 0;
            continue;                                // *** ADDED
        }

        // changed to 100 looks cool idk
        if (drawNum == 100) {
            drawNum = 0;
            for (int l = 0; l < bodies.size(); l++) {
                graphicsBodies[l].setFillColor(sf::Color::White);
                graphicsBodies[l].setRadius(5);
                graphicsBodies[l].setOrigin({ 5, 5 });
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


// Makes vector of planet graphics objects
std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies) {
    std::vector<sf::CircleShape> graphicsBodies = {};
    for (Object body : bodies) {
        sf::CircleShape graphicsBody;
        int bodyType = body.getType(); // type: 0 = sun, 1 = planet
        if (bodyType == 0) {
            graphicsBody.setRadius(20);
            graphicsBody.setOrigin({ 20,20 });
            graphicsBody.setFillColor(sf::Color::Yellow);
        }
        else if (bodyType == 1) {
            graphicsBody.setRadius(10);
            graphicsBody.setOrigin({ 10, 10 });
            graphicsBody.setFillColor(sf::Color::Green);
        }
        pos bodyLoc = body.getLocation();
        graphicsBody.setPosition({ (float)bodyLoc.x,(float)bodyLoc.y });
        graphicsBodies.push_back(graphicsBody);
    }
    return graphicsBodies;
}