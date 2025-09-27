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
        queue<Order*> getOrderQueue() const;

        // Location management
        void updateLocation(const Location& newLocation);
        // void moveTowards(const Location& destination);

        // Order management methods
        void receiveOrder(Order* order);
        void viewAvailableOrders(DeliverySystem* system);
        void acceptOrder(int orderId, DeliverySystem* system);
        void completeDelivery(int orderId);
        bool hasOrdersInQueue() const;
        void displayOrderQueue() const;

    private:
        int id;
        string name;
        Location currentLocation;
        queue<Order*> orderQueue;
        bool available;
};

#endif