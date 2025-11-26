#include "driver.h"
#include "order.h"
#include "../core/delivery_system.h"
#include <iostream>

// Constructor
Driver::Driver(int id_in, const string& name_in, const Location& initialLocation)
    :id(id_in),
    name(name_in),
    currentLocation(initialLocation),
    available(true),
    totalEarnings(0.0) {
}

Driver::~Driver() {}

// Getters
int Driver::getId() const { return id; }
string Driver::getName() const { return name; }
Location Driver::getCurrentLocation() const { return currentLocation; }
bool Driver::isAvailable() const { return available; }
double Driver::getTotalEarnings() const { return totalEarnings; }

// Location management
void Driver::updateLocation(const Location& newLocation) { currentLocation = newLocation; }
void Driver::setLocationNode(int node) { currentLocation.node = node; }

// Order management
void Driver::addOrder(Order* order) {
    orderQueue.push(order);
    available = false;
}

void Driver::acceptOrder(int orderId, DeliverySystem* system) {}

void Driver::addEarnings(double amount) {
    totalEarnings += amount;
}

int Driver::getPendingOrderCount() const {
    return static_cast<int>(orderQueue.size());
}

vector<int> Driver::getQueuedOrderIds() const {
    vector<int> ids;
    queue<Order*> tmp = orderQueue;
    while (!tmp.empty()) {
        ids.push_back(tmp.front()->getOrderId());
        tmp.pop();
    }
    return ids;
}

void Driver::completeDelivery(int orderId) {
    if (orderQueue.empty()) {
        cerr << "[Warning] Driver::completeDelivery called but queue is empty (driver #" << id << ")." << endl;
        available = true;
        return;
    }

    Order* removedOrder = nullptr;

    if (orderQueue.front()->getOrderId() == orderId) {
        removedOrder = orderQueue.front();
        orderQueue.pop();
    } else {
        queue<Order*> tmp;
        bool found = false;
        while (!orderQueue.empty()) {
            Order* o = orderQueue.front();
            orderQueue.pop();
            if (!found && o->getOrderId() == orderId) {
                removedOrder = o;
                found = true;
                continue;
            }
            tmp.push(o);
        }
        orderQueue = std::move(tmp);
        if (!found) {
            cerr << "[Error] Driver::completeDelivery: order id=" << orderId
                 << " not found in driver #" << id << " queue. No pop performed." << endl;
            available = orderQueue.empty();
            return;
        }
    }

    if (removedOrder) {
        addEarnings(removedOrder->getDeliveryFee());
    }

    available = orderQueue.empty();
}
