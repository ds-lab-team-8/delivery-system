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

    // 초기화 단계의 메서드들
    void addOrderer(const Orderer& orderer);
    void addStore(const Store& store);
    void addDriver(const Driver& driver);
    void addOrder(const Order& order);

    // 배차 및 주문 처리 단계의 메서드들
    void requestCallsToDrivers();   // 현재 orders 내에 있는 모든 주문들을 drivers에게 배차 요청 (driver의 배차 큐에 추가)
    // TODO: 실패 시 예외 발생 처리
    void acceptCall(int orderId);   // 특정 주문을 배차 요청에 수락 (driver가 호출)
    void completePickup(int orderId);   // 특정 주문을 픽업 완료 (주문 상태 변경)
    void completeDelivery(int orderId);   // 특정 주문을 배달 완료 (주문 상태 변경)

private:
    Map map;
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<Order*> orders;
};

#endif