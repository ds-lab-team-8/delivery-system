#include <queue>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include "delivery_system_with_drivercall.h"

using namespace std;

DeliverySystemWithDriverCall::DeliverySystemWithDriverCall() : DeliverySystem() {}                  // �θ� ������ ȣ��

DeliverySystemWithDriverCall::~DeliverySystemWithDriverCall() = default;                            // �θ� �Ҹ��� ȣ��

void DeliverySystemWithDriverCall::acceptCall() {
    set<int> assignedOrderIds;

    vector<Driver>& drivers = getDrivers();
	vector<Order*>& orders = getOrders();
	vector<Store>& stores = getStores();
	vector<Orderer>& orderers = getOrderers();
	Map& map = getMap();

    for (Driver& driver : drivers) {
        if (!driver.isAvailable()) continue;                                                    // ��� ������ ��縸 ����
        
        using OrderDist = pair<double, Order*>;
        priority_queue<OrderDist, vector<OrderDist>, greater<OrderDist>> orderHeap;             // �ּ� �� (�Ÿ� ����)

        for (Order* order : orders) {
            if (assignedOrderIds.count(order->getOrderId()) == 0 &&
                order->getStatus() == ORDER_ACCEPTED)
            {
                const Store* orderStore = order->getStore();
                const Orderer* orderOrderer = order->getOrderer();
                
                if (!orderStore || !orderOrderer) continue;
                
                const Location& storeLoc = orderStore->getLocation();
                const Location& ordererLoc = orderOrderer->getLocation();
                const Location& driverLoc = driver.getCurrentLocation();

                if (storeLoc.getNode() == -1 || ordererLoc.getNode() == -1 || driverLoc.getNode() == -1) {
                    continue;
                }

                double distToStore = map.GetMap_cost(driverLoc.getNode(), storeLoc.getNode());
                double distToOrderer = map.GetMap_cost(storeLoc.getNode(), ordererLoc.getNode());
                double totalDist = distToStore + distToOrderer;

                orderHeap.push({ totalDist, order });
            }
        }

        while (!orderHeap.empty()) {
            Order* bestOrder = orderHeap.top().second;
            orderHeap.pop();

            if (assignedOrderIds.count(bestOrder->getOrderId()) == 0) {
                driver.addOrder(bestOrder);
                bestOrder->assignDriver(driver.getId());
                bestOrder->acceptOrder();
                assignedOrderIds.insert(bestOrder->getOrderId());
                break;                                                                                      // �� ���� �ϳ��� �Ҵ�
            }
        }
    }
}