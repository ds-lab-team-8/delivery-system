#include "driver.h"
#include "order.h"
#include "../core/delivery_system.h"

// Constructor
Driver::Driver(int id_in, const string& name_in, const Location& initialLocation)
    :id(id_in),
    name(name_in),
    currentLocation(initialLocation),
    available(true) { // 초기화 -> 배달 가능 상태

}

// Destructor
Driver::~Driver() {

}

// Getters
int Driver::getId() const {
    return id;
}

string Driver::getName() const {
    return name;
}

Location Driver::getCurrentLocation() const {
    return currentLocation;
}

// Location management
void Driver::updateLocation(const Location& newLocation) {
    currentLocation = newLocation;
}

// Order management methods
void Driver::addOrder(Order* order) {
    orderQueue.push(order);
    available = false; // -> 배달 불가능 상태로 변경
}

void Driver::acceptOrder(int orderId, DeliverySystem* system) {

}

void Driver::completeDelivery(int orderId) {
    if(!orderQueue.empty()) {
        Order* completedOrder = orderQueue.front();
        orderQueue.pop();

        if (orderQueue.empty()) available = true;
    }
    
}