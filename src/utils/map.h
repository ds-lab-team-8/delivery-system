#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include "location.h"

using namespace std;

class Map {
    public:
        Map(int width, int height);
        ~Map();

        // Map management
        void initializeMap();
        void displayMap() const;
        void updatePersonLocation(int personId, const Location& location);

        // Distance calculations
        double calculateDistance(const Location& a, const Location& b) const;

        // Map validation
        bool isValidLocation(const Location& location) const;

        // Getters
        int getWidth() const;
        int getHeight() const;

    private:
        int width;
        int height;
        vector<Location> personLocations;  // Track locations of people (orderers, stores, drivers)
};

#endif