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
    Store(int id, const string& name, const Location& location, double feePerDistance);
        ~Store();

        // Getters
        int getId() const;
        string getName() const;
        Location getLocation() const;
        queue<Order*> getOrderQueue() const;
        
        // Location node 설정
        void setLocationNode(int node);

        // Order management methods
        void receiveOrder(Order* order);
        Order* processNextOrder();
        void requestDriverCall(DeliverySystem* system);
        void setPickupComplete(int orderId);
        bool hasOrdersWaiting() const;
        void displayOrderQueue() const;
        double getFeePerDistance() const;

    private:
        int id;
        string name;
        Location location;
        queue<Order*> orderQueue;
        double feePerDistance;
};

#endif
