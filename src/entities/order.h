#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <string>
#include <ctime>
#include "../utils/location.h"

using namespace std;

enum OrderStatus {
    ORDER_REQUESTED,      // 주문 요청
    ORDER_ACCEPTED,       // 주문 수락 (기사 콜 요청)
    DRIVER_CALL_ACCEPTED, // 기사 콜 수락
    PICKUP_COMPLETE,      // 픽업 완료
    DELIVERY_COMPLETE     // 배달 완료
};

class Order {
    public:
        Order(int orderId, int ordererId, int storeId, const Location& deliveryAddress, const string& orderDetails);
        ~Order();

        // Getters
        int getOrderId() const;
        int getOrdererId() const;
        int getStoreId() const;
        int getDriverId() const;
        Location getDeliveryAddress() const;
        OrderStatus getStatus() const;

        // Setters
        // void setDriverId(int driverId);
        // void setStatus(OrderStatus status);

        // Order status management
        void acceptOrder();
        void assignDriver(int driverId);
        void completePickup();
        void completeDelivery();

        // Utility methods
        string getStatusString() const;
        void displayOrderInfo() const;
        bool isCompleted() const;

    private:
        int orderId;
        int ordererId;
        int storeId;
        int driverId;
        Location deliveryAddress;
        OrderStatus status;
};

#endif