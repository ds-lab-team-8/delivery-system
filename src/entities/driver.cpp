#include "driver.h"
#include "order.h"
#include "../core/delivery_system.h"

// Constructor
Driver::Driver(int id_in, const string& name_in, const Location& initialLocation)
    :id(id_in),
    name(name_in),
    currentLocation(initialLocation),
    available(true), // 초기화 -> 배달 가능 상태
    totalEarnings(0.0) { 

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

bool Driver::isAvailable() const {
    return available;
}

double Driver::getTotalEarnings() const {
    return totalEarnings;
}

// Location management
void Driver::updateLocation(const Location& newLocation) {
    currentLocation = newLocation;
}

void Driver::setLocationNode(int node) {
    currentLocation.node = node;
}

// Order management methods
void Driver::addOrder(Order* order) {
    orderQueue.push(order);
    available = false; // -> 배달 불가능 상태로 변경
}

void Driver::acceptOrder(int orderId, DeliverySystem* system) {

}

// 수입이 추가 메서드
void Driver::addEarnings(double amount) {
    totalEarnings += amount;
}

void Driver::completeDelivery(int orderId) { // 추가(수정)된 부분 : 수입 정산 로직 
    if(!orderQueue.empty()) {
        Order* completedOrder = orderQueue.front();
        double fee = completedOrder->getDeliveryFee();
        addEarnings(fee);
        orderQueue.pop();
        completedOrder->completeDelivery();

        if (orderQueue.empty()) available = true;
    }
    

}
