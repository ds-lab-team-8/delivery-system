#include "store.h"
#include "order.h"
#include "../core/delivery_system.h"

// Constructor
Store::Store(int id_in, const string& name_in, const Location& location_in) 
    :id(id_in), name(name_in), location(location_in) {

}

// Destructor
Store::~Store() {

}

// Getters
int Store::getId() const {
    return id;
}

string Store::getName() const {
    return name;
}

Location Store::getLocation() const {
    return location;
}

queue<Order*> Store::getOrderQueue() const {
    return orderQueue;
}

void Store::setLocationNode(int node) {
    location.node = node;
}

// Order management
void Store::receiveOrder(Order* order) {
    order -> acceptOrder(); // ORDER_ACCEPTED로 변경
    orderQueue.push(order);
}

Order* Store::processNextOrder() {
    if(!orderQueue.empty()) {
        Order* orderToProcess = orderQueue.front();
        orderQueue.pop();
        return orderToProcess;
    }
    return nullptr; // 처리할 주문 없을 시
}

void Store::requestDriverCall(DeliverySystem* system) {
    system->acceptCall();
}

void Store::setPickupComplete(int orderId) {

}

void Store::displayOrderQueue() const {
    
}
