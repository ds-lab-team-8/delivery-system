#ifndef ORDERER_H
#define ORDERER_H

#include <iostream>
#include <string>
#include "../utils/location.h"

using namespace std;

class Orderer {
    public:
        Orderer(int id, const string& name, const Location& location);
        ~Orderer();

        // Getters
        int getId() const;
        string getName() const;
        Location getLocation() const;

    private:
        int id;
        string name;
        Location location;
};

#endif