#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>

using namespace std;

class Location {
    public:
        Location();
        Location(int x, int y);
        ~Location();

        // Getters
        int getX() const;
        int getY() const;

        // Setters
        void updateLocation(int x, int y);

        // Utility methods
        double calculateDistance(const Location& other) const;
        bool operator==(const Location& other) const;
        bool operator!=(const Location& other) const;

    private:
        int x;
        int y;
};

#endif