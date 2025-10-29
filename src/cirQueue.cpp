#include "cirQueue.h"

cirQueue::cirQueue(int size) {
	this->size = size;
	elements.resize(size);
	head = 0;
	tail = 0;
}

void cirQueue::push(sf::CircleShape shape) {
	elements[head] = shape;
	head++;
	if (head == size) {
		head = 0;
	}
	if (head == tail) {
		pop();
	}
}

sf::CircleShape cirQueue::pop() {
	sf::CircleShape output = elements[tail];
	tail++;
	if (tail == size) {
		tail = 0;
	}
	return output;
}

void cirQueue::printToScreen(sf::RenderWindow& window) {
	int tempTail = tail;
	while (tempTail != head) {
		window.draw(elements[tempTail]);
		tempTail++;
		if (tempTail == size) {
			tempTail = 0;
		}
	}
}

