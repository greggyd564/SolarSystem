#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "Object.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "cirQueue.h"
//#include <cassert>

double computeGForce(double m1, double m2, double r);
double computeAcceleration(double F, double m);
double distanceBetweenTwoObjects(double x1, double y1, double x2, double y2);
std::vector<sf::CircleShape> convertBodies(std::vector<Object> bodies);


double G = 10.0; // For testing
double dt = 0.1; // DeltaTime (Change in time)

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1100, 600}), "CMake SFML Project");
    sf::View worldView = sf::View();
    worldView.setSize({770, 600});
    worldView.setCenter({385, 300});
    //sf::FloatRect world_viewport = sf::FloatRect({ 0, 0 }, { .7f, 1.f });
    worldView.setViewport(sf::FloatRect({ 0, 0 }, { .7f, 1.f }));
    sf::View uiView = sf::View();
    uiView.setSize({330, 600});
    uiView.setCenter({165, 300});
    uiView.setViewport(sf::FloatRect({.7, 0}, {.3, 1}));
    //std::cout << "Current working directory: "
    //    << std::filesystem::current_path() << '\n';
    sf::Font font(std::string(PROJECT_ROOT) + "/arial.ttf");
    sf::Text title(font);
    title.setString("Settings");
    title.setCharacterSize(24);
    title.setOrigin({title.getLocalBounds().size.x/2,title.getLocalBounds().size.y/2});
    title.setPosition({165, 20});

    struct Slider {
        sf::RectangleShape track;
        sf::CircleShape    thumb;
        float min = 0.f, max = 100.f, value = 50.f;
        float left = 16.f, top = 80.f, width = 288.f;
    } slider;

    slider.track.setSize({ slider.width, 6.f });
    slider.track.setPosition({ slider.left, slider.top });
    slider.track.setFillColor(sf::Color(80, 80, 90));

    slider.thumb.setRadius(10.f);
    slider.thumb.setOrigin({ 10.f, 10.f }); // center origin
    slider.thumb.setFillColor(sf::Color(200, 200, 220));

    auto updateThumb = [&] {
        float t = (slider.value - slider.min) / (slider.max - slider.min);
        float x = slider.left + t * slider.width;
        slider.thumb.setPosition({ x, slider.top + 3.f });
    };
    updateThumb();

    bool draggingSlider = false;
    bool inputFocused = false;

    window.setFramerateLimit(1000);

    cirQueue trails = cirQueue(20);
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

    // Initialize the Sun & the Earth ( 2 Body System )
    Object sun(sMass, 0, 0, 0, sX, sY);
    Object earth(pMass, 1, pVeloX, pVeloY, pX, pY);

    std::vector<Object> bodies = {sun, earth};

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

            if (event->is<sf::Event::MouseButtonPressed>() ) { //&& e.mouseButton.button == sf::Mouse::Left) {
                // Slider hit
                sf::FloatRect trackBounds(slider.track.getPosition(), slider.track.getSize());
                //trackBounds.top -= 10.f; trackBounds.height += 20.f; // easier to catch
                //if (trackBounds.contains(uiPos)) {
                    draggingSlider = true;
                //}

                // Input focus
                //inputFocused = inputRect.getGlobalBounds().contains(uiPos);
                //inputRect.setOutlineColor(inputFocused ? sf::Color(180, 180, 220)
                //    : sf::Color(100, 100, 120));
            }
            if (event->is<sf::Event::MouseButtonReleased>()) { //&& e.mouseButton.button == sf::Mouse::Left) {
                draggingSlider = false;
            }
            if (event->is<sf::Event::MouseMoved>() && draggingSlider) {
                //std::cout << "Mouse x: " << sf::Mouse::getPosition().x << std::endl;
                //std::cout << "Slider.left: " << slider.left << std::endl;
                //std::cout << "Mouse x to Coords: " << window.mapPixelToCoords(sf::Mouse::getPosition(window)).x << std::endl;
                float t = std::clamp((window.mapPixelToCoords(sf::Mouse::getPosition(window)).x - slider.left)/slider.width, 0.f, 1.f);
                slider.value = slider.min + t * (slider.max - slider.min);
                //std::cout << "t: " << t << std::endl;
                //std::cout << "Slider Value: " << slider.value << std::endl;
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
        // --- Background panel that fills the UI view ---
        sf::RectangleShape uiBg;
        uiBg.setSize({ 330, static_cast<float>(window.getSize().y) });
        uiBg.setPosition({ 0.f, 0.f }); // in UI-view space, (0,0) is the top-left of the panel
        uiBg.setFillColor(sf::Color(30, 30, 35)); // dark panel
        window.draw(uiBg);
        window.draw(title);
        window.draw(slider.track);
        window.draw(slider.thumb);
        
        
        window.display();

        if (drawNum == 1000) {
            drawNum = 0;
            graphicsBodies[1].setFillColor(sf::Color::White);
            trails.push(graphicsBodies[1]);
        }

        double r = distanceBetweenTwoObjects(sun.getLocation().x, sun.getLocation().y, earth.getLocation().x, earth.getLocation().y);
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
