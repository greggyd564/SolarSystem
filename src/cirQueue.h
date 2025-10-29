

#ifndef CIRQUEUE_H
#define CIRQUEUE_H

#include <vector>
#include <SFML/Graphics.hpp>

class cirQueue {
public:
    // Constructor
    cirQueue(int size);

    // Getters
    sf::CircleShape pop();

    // Setters
    void push(sf::CircleShape shape);
    
    void printToScreen(sf::RenderWindow& window);


private:
    int size;
    int head;
    int tail;
    std::vector<sf::CircleShape> elements;

};



#endif //CIRQUEUE_H