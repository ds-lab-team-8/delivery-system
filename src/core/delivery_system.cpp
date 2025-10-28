#include <queue>
#include <algorithm>
#include "delivery_system.h"

using namespace std;

DeliverySystem::DeliverySystem() : map(100, 100) {}                                 // 기본 맵 크기를 100x100으로 설정

DeliverySystem::~DeliverySystem() {                                                 // 소멸자에서 동적 할당된 주문 객체들 해제                                           
    for (Order* order : orders) {
        delete order;
    }
}

void DeliverySystem::addOrderer(const Orderer& orderer) {                           // 주문자 추가
    orderers.push_back(orderer);
    map.addItem(MapItem(orderer.getLocation(), ORDERER, orderer.getId()));
}

void DeliverySystem::addStore(const Store& store) {                                 // 가게 추가
    stores.push_back(store);
    map.addItem(MapItem(store.getLocation(), STORE, store.getId()));
}

void DeliverySystem::addDriver(const Driver& driver) {                              // 기사 추가
    drivers.push_back(driver);
    map.addItem(MapItem(driver.getCurrentLocation(), DRIVER, driver.getId()));
}

void DeliverySystem::addOrder(const Order& order) {                                 // 주문 추가
    Order* newOrder = new Order(
        order.getOrderId(),
        order.getOrdererId(),
        order.getStoreId(),
        order.getDeliveryLocation()
    );                                                                          // 동적 할당된 주문 객체 생성

    orders.push_back(newOrder);                                                 // 해당 주문을 받는 가게를 찾아서 주문 큐에 추가

    auto it = find_if(stores.begin(), stores.end(), [&](const Store& store) {
        return store.getId() == order.getStoreId();
        });                                                                         // 해당 주문을 받는 가게 찾기
    if (it != stores.end()) {
        it->receiveOrder(newOrder);                                             // 가게의 주문 큐에 주문 추가
    }
    else {
        cerr << "Error: Store with ID " << order.getStoreId() << " not found." << endl;
    }                                                                           // 가게가 존재하지 않는 경우 에러 메시지 출력                                                                    
}
/*
void DeliverySystem::requestCallsToDrivers() {
    for (Order* order : orders) {
        if (order->getStatus() == ORDER_ACCEPTED) {                             // 아직 배차되지 않은 주문에 대해서만 처리
            auto it = find_if(stores.begin(), stores.end(), [&](const Store& store) {
                return store.getId() == order->getStoreId();
                });
            if (it != stores.end() && it->hasOrdersWaiting()) {
                it->requestDriverCall(this);                                    // 가게에서 배차 요청
            }                                                                   // 해당 주문을 받는 가게를 찾아서 주문 큐 확인
        }
    }
}
*/
void DeliverySystem::acceptCall() {                                             // 특정 주문에 대해 배차 요청 수락
    // DeliverySystem_drivercall 인지 DeliverySystem_systemselection 인지에 따라 변동될 메서드
    // 오버라이드해 구현해두었음(DeliverySystem_drivercall에서는 orderid를 사용하지 않음)
}

void DeliverySystem::completePickup(int orderId) {                                      // 특정 주문에 대해 픽업 완료
    auto orderIt = find_if(orders.begin(), orders.end(), [&](Order* order) {        // 주문 ID로 주문 객체를 찾아 주문 상태 변경
        return order->getOrderId() == orderId;
        });
    if (orderIt == orders.end()) {
        cerr << "Error: Order with ID " << orderId << " not found." << endl;
        return;
    }
    Order* order = *orderIt;
    order->completePickup();
}

void DeliverySystem::completeDelivery(int orderId) {                                    // 특정 주문에 대해 배달 완료
    auto orderIt = find_if(orders.begin(), orders.end(), [&](Order* order) {        // 주문 ID로 주문 객체를 찾아 주문 상태 변경
        return order->getOrderId() == orderId;
        });
    if (orderIt == orders.end()) {
        cerr << "Error: Order with ID " << orderId << " not found." << endl;
        return;
    }
    Order* order = *orderIt;
    order->completeDelivery();

    auto driverIt = find_if(drivers.begin(), drivers.end(), [&](Driver& driver) {   // 해당 주문을 처리한 기사를 찾아서 기사 상태 업데이트
        return driver.getId() == order->getDriverId();
        });
    if (driverIt != drivers.end()) {
        driverIt->completeDelivery(orderId);
    }
    else {
        cerr << "Error: Driver with ID " << order->getDriverId() << " not found." << endl;
    }
}

void DeliverySystem::statusUpdate() {                                                // 주문 상태 점검용 메서드
    for (Order* order : orders) {
        OrderStatus status = order->getStatus();

        auto driverIt = find_if(drivers.begin(), drivers.end(), [&](const Driver& driver) {         // 기사 위치 찾기
            return driver.getId() == order->getDriverId();
            });
        if (driverIt == drivers.end()) continue;
        const Location& driverLoc = driverIt->getCurrentLocation();

        auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& store) {              // 가게 위치 찾기
            return store.getId() == order->getStoreId();
            });
        if (storeIt == stores.end()) continue;
        const Location& storeLoc = storeIt->getLocation();

        auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& orderer) {    // 주문자 위치 찾기
            return orderer.getId() == order->getOrdererId();
            });
        if (ordererIt == orderers.end()) continue;
        const Location& ordererLoc = ordererIt->getLocation();

        if (status == DRIVER_CALL_ACCEPTED &&                                       // 콜 수락 후 기사 위치가 가게 위치와 같으면 픽업 완료
            driverLoc.getX() == storeLoc.getX() && driverLoc.getY() == storeLoc.getY()) {
            order->completePickup();
        }

        else if (status == PICKUP_COMPLETE &&                                       // 픽업 완료 후 기사 위치가 주문자 위치와 같으면 배달 완료
            driverLoc.getX() == ordererLoc.getX() && driverLoc.getY() == ordererLoc.getY()) {
            order->completeDelivery();
            driverIt->completeDelivery(order->getOrderId());
        }
    }
}

