#include "order.h"

// Constructor
Order::Order(int orderId_in, int ordererId_in, int storeId_in, const Location& deliveryLocation_in)
    :orderId(orderId_in),
    ordererId(ordererId_in),
    storeId(storeId_in),
    deliveryLocation(deliveryLocation_in),
    driverId(-1), // 기사 배정 전 -> -1로 초기화
    status(ORDER_REQUESTED) {
}

//Destructor
Order::~Order() {

}

//Getter
int Order::getOrderId() const {
    return orderId;
}

int Order::getOrdererId() const {
    return ordererId;
}

int Order::getStoreId() const {
    return storeId;
}

int Order::getDriverId() const {
    return driverId;
}

Location Order::getDeliveryLocation() const {
    return deliveryLocation;
}

OrderStatus Order::getStatus() const {
    return status;
}

// Order status management
void Order::acceptOrder() {
    status = ORDER_ACCEPTED;
}

void Order::assignDriver(int driverId_in) {
    driverId = driverId_in;
    status = DRIVER_CALL_ACCEPTED;
}

void Order::completePickup() {
    status = PICKUP_COMPLETE;
}

void Order::completeDelivery() {
    status = DELIVERY_COMPLETE;
}

//Utility methods
bool Order::isDeliveryCompleted() const {
    return (status == DELIVERY_COMPLETE);
}