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


        int node; //해당 좌표가 맵에 몇번째 노드에 위치해있는지 저장

    private:
        int x;
        int y;
};

#endif
