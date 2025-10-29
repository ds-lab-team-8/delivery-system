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

class Orderer;
class Store;

class Order {
    public:
        Order(int orderId, int ordererId, int storeId, const Location& deliveryLocation);
        ~Order();

        // Getters
        int getOrderId() const;
        int getOrdererId() const;
        int getStoreId() const;
        int getDriverId() const;
        Location getDeliveryLocation() const;
        OrderStatus getStatus() const;

        const Orderer* getOrderer() const;
        const Store* getStore() const;

        // Setters
        void setOrderer(const Orderer* orderer);
        void setStore(const Store* store);
        void setDeliveryLocationNode(int node);

        // Order status management
        void acceptOrder();                     // 주문 수락 (상태 변경)
        void assignDriver(int driverId);        // 기사 할당 (상태 변경 및 driverId 지정)
        void completePickup();                  // 픽업 완료 (상태 변경)
        void completeDelivery();                // 배달 완료 (상태 변경)

        // Utility methods
        bool isDeliveryCompleted() const;       // 배달 완료 여부 확인

    private:
        int orderId;
        int ordererId;
        int storeId;
        int driverId;
        Location deliveryLocation;
        OrderStatus status;

        const Orderer* orderer;
        const Store* store;
};

#endif
