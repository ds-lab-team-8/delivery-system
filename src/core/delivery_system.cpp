#include <queue>
#include <algorithm>
#include "delivery_system.h"

DeliverySystem::DeliverySystem() : map(100, 100) {}                                 // �⺻ �� ũ�⸦ 100x100���� ����

DeliverySystem::~DeliverySystem() {                                                 // �Ҹ��ڿ��� ���� �Ҵ�� �ֹ� ��ü�� ����                                           
    for (Order* order : orders) {
        delete order;
    }
}

void DeliverySystem::addOrderer(const Orderer& orderer) {                           // �ֹ��� �߰�
    orderers.push_back(orderer);
    map.addItem(MapItem(orderer.getLocation(), ORDERER, orderer.getId()));
}

void DeliverySystem::addStore(const Store& store) {                                 // ���� �߰�
    stores.push_back(store);
    map.addItem(MapItem(store.getLocation(), STORE, store.getId()));
}

void DeliverySystem::addDriver(const Driver& driver) {                              // ��� �߰�
    drivers.push_back(driver);
    map.addItem(MapItem(driver.getCurrentLocation(), DRIVER, driver.getId()));
}

void DeliverySystem::addOrder(const Order& order) {                                 // �ֹ� �߰�
    Order* newOrder = new Order(
        order.getOrderId(),
        order.getOrdererId(),
        order.getStoreId(),
        order.getDeliveryLocation()
    );                                                                          // ���� �Ҵ�� �ֹ� ��ü ����

    orders.push_back(newOrder);                                                 // �ش� �ֹ��� �޴� ���Ը� ã�Ƽ� �ֹ� ť�� �߰�

    auto it = find_if(stores.begin(), stores.end(), [&](const Store& store) {
        return store.getId() == order.getStoreId();
        });                                                                         // �ش� �ֹ��� �޴� ���� ã��
    if (it != stores.end()) {
        it->receiveOrder(newOrder);                                             // ������ �ֹ� ť�� �ֹ� �߰�
    }
    else {
        cerr << "Error: Store with ID " << order.getStoreId() << " not found." << endl;
    }                                                                           // ���԰� �������� �ʴ� ��� ���� �޽��� ���                                                                    
}

void DeliverySystem::requestCallsToDrivers() {
    for (Order* order : orders) {
        if (order->getStatus() == ORDER_ACCEPTED) {                             // ���� �������� ���� �ֹ��� ���ؼ��� ó��
            auto it = find_if(stores.begin(), stores.end(), [&](const Store& store) {
                return store.getId() == order->getStoreId();
                });
            if (it != stores.end() && it->hasOrdersWaiting()) {
                it->requestDriverCall(this);                                    // ���Կ��� ���� ��û
            }                                                                   // �ش� �ֹ��� �޴� ���Ը� ã�Ƽ� �ֹ� ť Ȯ��
        }
    }
}

void DeliverySystem::acceptCall(int orderId) {                                      // Ư�� �ֹ��� ���� ���� ��û ����
    // DeliverySystem_drivercall ���� DeliverySystem_systemselection ������ ���� ������ �޼���
    // �������̵��� �����صξ���(DeliverySystem_drivercall������ orderid�� ������� ����)
}

void DeliverySystem::completePickup(int orderId) {                                      // Ư�� �ֹ��� ���� �Ⱦ� �Ϸ�
    auto orderIt = find_if(orders.begin(), orders.end(), [&](Order* order) {        // �ֹ� ID�� �ֹ� ��ü�� ã�� �ֹ� ���� ����
        return order->getOrderId() == orderId;
        });
    if (orderIt == orders.end()) {
        cerr << "Error: Order with ID " << orderId << " not found." << endl;
        return;
    }
    Order* order = *orderIt;
    order->completePickup();
}

void DeliverySystem::completeDelivery(int orderId) {                                    // Ư�� �ֹ��� ���� ��� �Ϸ�
    auto orderIt = find_if(orders.begin(), orders.end(), [&](Order* order) {        // �ֹ� ID�� �ֹ� ��ü�� ã�� �ֹ� ���� ����
        return order->getOrderId() == orderId;
        });
    if (orderIt == orders.end()) {
        cerr << "Error: Order with ID " << orderId << " not found." << endl;
        return;
    }
    Order* order = *orderIt;
    order->completeDelivery();

    auto driverIt = find_if(drivers.begin(), drivers.end(), [&](Driver& driver) {   // �ش� �ֹ��� ó���� ��縦 ã�Ƽ� ��� ���� ������Ʈ
        return driver.getId() == order->getDriverId();
        });
    if (driverIt != drivers.end()) {
        driverIt->completeDelivery(orderId);
    }
    else {
        cerr << "Error: Driver with ID " << order->getDriverId() << " not found." << endl;
    }
}

void DeliverySystem::statusUpdate() {                                                // �ֹ� ���� ���˿� �޼���
    for (Order* order : orders) {
        OrderStatus status = order->getStatus();

        auto driverIt = find_if(drivers.begin(), drivers.end(), [&](const Driver& driver) {         // ��� ��ġ ã��
            return driver.getId() == order->getDriverId();
            });
        if (driverIt == drivers.end()) continue;
        const Location& driverLoc = driverIt->getCurrentLocation();

        auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& store) {              // ���� ��ġ ã��
            return store.getId() == order->getStoreId();
            });
        if (storeIt == stores.end()) continue;
        const Location& storeLoc = storeIt->getLocation();

        auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& orderer) {    // �ֹ��� ��ġ ã��
            return orderer.getId() == order->getOrdererId();
            });
        if (ordererIt == orderers.end()) continue;
        const Location& ordererLoc = ordererIt->getLocation();

        if (status == DRIVER_CALL_ACCEPTED &&                                       // �� ���� �� ��� ��ġ�� ���� ��ġ�� ������ �Ⱦ� �Ϸ�
            driverLoc.getX() == storeLoc.getX() && driverLoc.getY() == storeLoc.getY()) {
            order->completePickup();
        }

        else if (status == PICKUP_COMPLETE &&                                       // �Ⱦ� �Ϸ� �� ��� ��ġ�� �ֹ��� ��ġ�� ������ ��� �Ϸ�
            driverLoc.getX() == ordererLoc.getX() && driverLoc.getY() == ordererLoc.getY()) {
            order->completeDelivery();
            driverIt->completeDelivery(order->getOrderId());
        }
    }
}
