#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <string>
#include <queue>
#include "../utils/location.h"

using namespace std;

class Order;
class DeliverySystem;

class Store {
    public:
        Store(int id, const string& name, const Location& location);
        ~Store();

        // Getters
        int getId() const;
        string getName() const;
        Location getLocation() const;
        queue<Order*> getOrderQueue() const;

        // Order management methods
        void receiveOrder(Order* order);
        Order* processNextOrder();
        void requestDriverCall(DeliverySystem* system);
        void setPickupComplete(int orderId);
        bool hasOrdersWaiting() const;
        void displayOrderQueue() const;

    private:
        int id;
        string name;
        Location location;
        queue<Order*> orderQueue;
};

#endif