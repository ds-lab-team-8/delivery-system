#include "location.h"

#include <cmath>

//Constructor
Location::Location() : x(0), y(0) {
}

Location::Location(int x_in, int y_in) : x(x_in), y(y_in) {
}

//Destructor
Location::~Location() {
}

// Getters
int Location::getX() const {
    return x;
}

int Location::getY() const {
    return y;
}

// Setters
void Location::updateLocation(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

// Utility methods
double Location::calculateDistance(const Location& other) const {
    // sqrt((x2-x1)^2 + (y2-y1)^2)
    return sqrt(pow(other.x - this->x, 2) + pow(other.y - this->y, 2));
}

bool Location::operator==(const Location& other) const {
    return (this->x == other.x && this->y == other.y);
}

bool Location::operator!=(const Location& other) const {
    return !(*this == other);
}