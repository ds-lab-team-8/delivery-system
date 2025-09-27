#ifndef DELIVERY_SYSTEM_H
#define DELIVERY_SYSTEM_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "../utils/map.h"
#include "../entities/orderer.h"
#include "../entities/driver.h"
#include "../entities/store.h"
#include "../entities/order.h"

using namespace std;

class DeliverySystem {
    public:
        DeliverySystem();
        ~DeliverySystem();

        // Participant management
        void addOrderer(const Orderer& orderer);
        void addStore(const Store& store);
        void addDriver(const Driver& driver);

        // Order lifecycle methods
        void processNewOrder(int ordererId, int storeId, const string& orderDetails);
        void storeRequestDriverCall();
        void driverViewAvailableOrders(int driverId);
        void driverAcceptOrder(int driverId, int orderId);
        void storeSetPickupComplete(int orderId);
        void driverCompleteDelivery(int orderId);

        // Optimization algorithms (core features)
        int findOptimalDriver(const Order& order);
        vector<Order*> optimizeMultipleDeliveries(int driverId);
        double calculateDeliveryDistance(const Location& store, const Location& customer, const Location& driverPos);

        // Getters
        vector<Orderer>& getOrderers();
        vector<Store>& getStores();
        vector<Driver>& getDrivers();
        Order* findOrderById(int orderId);
        Store* findStoreById(int storeId);
        Driver* findDriverById(int driverId);
        Orderer* findOrdererById(int ordererId);

    private:
        Map map;
        vector<Orderer> orderers;
        vector<Driver> drivers;
        vector<Store> stores;
        vector<Order*> orders;
        queue<Order*> ordersWaitingStore;
        queue<Order*> ordersWaitingDriver;
        int nextOrderId;

        // Internal helper methods
        void autoProcessStoreOrders();
        void autoAssignDrivers();
        void updateOrderStatus(int orderId, OrderStatus newStatus);
        double calculateDistance(const Location& a, const Location& b) const;
};

#endif