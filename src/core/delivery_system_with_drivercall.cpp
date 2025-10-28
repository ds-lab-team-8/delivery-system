#include <queue>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include "delivery_system_with_drivercall.h"

using namespace std;

DeliverySystemWithDriverCall::DeliverySystemWithDriverCall() : DeliverySystem() {}                  // 부모 생성자 호출

DeliverySystemWithDriverCall::~DeliverySystemWithDriverCall() = default;                            // 부모 소멸자 호출

void DeliverySystemWithDriverCall::acceptCall() {
    set<int> assignedOrderIds;

    vector<Driver>& drivers = getDrivers();
	vector<Order*>& orders = getOrders();
	vector<Store>& stores = getStores();
	vector<Orderer>& orderers = getOrderers();
	Map& map = getMap();

    for (Driver& driver : drivers) {
        if (!driver.isAvailable()) continue;                                                    // 사용 가능한 기사만 고려
        
        using OrderDist = pair<double, Order*>;
        priority_queue<OrderDist, vector<OrderDist>, greater<OrderDist>> orderHeap;             // 최소 힙 (거리 기준)

        for (Order* order : orders) {
            if (assignedOrderIds.count(order->getOrderId()) == 0 &&
                order->getStatus() == ORDER_ACCEPTED)
            {
                auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& store) {              // 가게 찾기
                    return store.getId() == order->getStoreId();
                    });
                if (storeIt == stores.end()) continue;
                const Location& storeLoc = storeIt->getLocation();

                auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& orderer) {    // 주문자 찾기
                    return orderer.getId() == order->getOrdererId();
                    });
                if (ordererIt == orderers.end()) continue;
                const Location& ordererLoc = ordererIt->getLocation();

                double distToStore = map.GetMap_cost(driver.getCurrentLocation(), storeLoc);  // 기사 → 가게, 가게 → 주문자 최단거리 합산
                double distToOrderer = map.GetMap_cost(storeLoc, ordererLoc);
                double totalDist = distToStore + distToOrderer;

                orderHeap.push({ totalDist, order });                                                       // 거리와 주문 쌍을 힙에 추가
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
                break;                                                                                      // 한 기사당 하나만 할당
            }
        }
    }
}