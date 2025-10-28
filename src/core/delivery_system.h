#ifndef DELIVERY_SYSTEM_H
#define DELIVERY_SYSTEM_H

#include <iostream>
#include <vector>
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
    void acceptCall();   // 특정 주문을 배차 요청에 수락 (driver가 호출)
	void statusUpdate();          // 주문 상태 업데이트용 메서드 (픽업 완료, 배달 완료 시점 업데이트용)

    /*
	statusUpdate() 메서드 내에서 처리되므로 아래 메서드들은 주석 처리
    void completePickup(int orderId);   // 특정 주문을 픽업 완료 (주문 상태 변경)
    void completeDelivery(int orderId);   // 특정 주문을 배달 완료 (주문 상태 변경)
    */

protected:
	// getters
    Map& getMap() { return map; }
    vector<Orderer>& getOrderers() { return orderers; }
    vector<Driver>& getDrivers() { return drivers; }
    vector<Store>& getStores() { return stores; }
	vector<Order*>& getOrders() { return orders; }

private:
    Map map;
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<Order*> orders;
};


#endif
