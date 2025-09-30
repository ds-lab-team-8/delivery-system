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

    void addOrderer(const Orderer& orderer);
    void addStore(const Store& store);
    void addDriver(const Driver& driver);
    void addOrder(const Order& order);

    void processOrdersWithDriverCall();
    void processOrdersWithSystemSelection();

private:
    Map map;
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<Order*> orders;
};

#endif