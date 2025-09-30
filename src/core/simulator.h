#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <vector>
#include "delivery_system.h"
#include "../entities/orderer.h"
#include "../entities/store.h"
#include "../entities/driver.h"
#include "../utils/map.h"

using namespace std;

class Simulator {
public:
    Simulator();
    ~Simulator();
    
    void simulateWithUserInput();

private:
    DeliverySystem* deliverySystem;
};

#endif