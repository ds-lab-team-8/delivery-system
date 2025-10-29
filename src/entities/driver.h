#ifndef DRIVER_H
#define DRIVER_H

#include <iostream>
#include <string>
#include <queue>
#include "../utils/location.h"

using namespace std;

class Order;
class DeliverySystem;

class Driver {
    public:
        Driver(int id, const string& name, const Location& initialLocation);
        ~Driver();

        // Getters
        int getId() const;
        string getName() const;
        Location getCurrentLocation() const;
        bool isAvailable() const;

        // Location management
        void updateLocation(const Location& newLocation);

        // Order management methods
        void addOrder(Order* order);
        void acceptOrder(int orderId, DeliverySystem* system);
        void completeDelivery(int orderId);

    private:
        int id;
        string name;
        Location currentLocation;
        queue<Order*> orderQueue;
        bool available;
};

#endif
