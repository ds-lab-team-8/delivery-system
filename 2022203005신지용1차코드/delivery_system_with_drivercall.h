#ifndef DELIVERY_SYSTEM_WITH_DRIVER_CALL_H
#define DELIVERY_SYSTEM_WITH_DRIVER_CALL_H

#include <queue>
#include <set>
#include <vector>
#include <utility>
#include "delivery_system.h"

using namespace std;

class DeliverySystemWithDriverCall : public DeliverySystem {
public:
    DeliverySystemWithDriverCall();
    ~DeliverySystemWithDriverCall();
	
    // DeliverySystem_drivercall 인지 DeliverySystem_systemselection 인지에 따라 변동될 메서드
    void acceptCall(int orderId) override;
    
	void DeliverySystemWithDriverCall::acceptCall(int /*orderId*/) {                                // 이 경우 orderId는 사용되지 않음
        set<int> assignedOrderIds;

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
                    
                    double distToStore = map.calculateShortestDistance(driver.getCurrentLocation(), storeLoc);  // 기사 → 가게, 가게 → 주문자 최단거리 합산
                    double distToOrderer = map.calculateShortestDistance(storeLoc, ordererLoc);
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
}

DeliverySystemWithDriverCall::DeliverySystemWithDriverCall() : DeliverySystem() {}                  // 부모 생성자 호출

DeliverySystemWithDriverCall::~DeliverySystemWithDriverCall() = default;                            // 부모 소멸자 호출

#endif