#ifndef ORDERER_H
#define ORDERER_H

#include <iostream>
#include <string>
#include "../utils/location.h"

using namespace std;

class Order;
class DeliverySystem;

class Orderer {
    public:
        Orderer(int id, const string& name, const Location& address);
        ~Orderer();

        // Getters
        int getId() const;
        string getName() const;
        Location getAddress() const;

        // Methods
        void placeOrder(DeliverySystem* system, int storeId);

    private:
        int id;
        string name;
        Location address;
};

#endif