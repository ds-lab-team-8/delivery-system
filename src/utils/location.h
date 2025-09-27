#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>

using namespace std;

class Location {
    public:
        int x;
        int y;

        Location();
        Location(int x, int y);
        ~Location();

        // Getters
        int getX() const;
        int getY() const;

        // Setters
        void setX(int x);
        void setY(int y);
        void setLocation(int x, int y);

        // Utility methods
        void displayLocation() const;
        bool operator==(const Location& other) const;
        bool operator!=(const Location& other) const;
};

#endif