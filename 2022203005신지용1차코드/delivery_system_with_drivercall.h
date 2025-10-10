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
	
    // DeliverySystem_drivercall ���� DeliverySystem_systemselection ������ ���� ������ �޼���
    void acceptCall(int orderId) override;
    
	void DeliverySystemWithDriverCall::acceptCall(int /*orderId*/) {                                // �� ��� orderId�� ������ ����
        set<int> assignedOrderIds;

        for (Driver& driver : drivers) {
            if (!driver.isAvailable()) continue;                                                    // ��� ������ ��縸 ���

            using OrderDist = pair<double, Order*>;
			priority_queue<OrderDist, vector<OrderDist>, greater<OrderDist>> orderHeap;             // �ּ� �� (�Ÿ� ����)

            for (Order* order : orders) {
                if (assignedOrderIds.count(order->getOrderId()) == 0 &&
                    order->getStatus() == ORDER_ACCEPTED)
                {
                    auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& store) {              // ���� ã��
                        return store.getId() == order->getStoreId();
                        });
                    if (storeIt == stores.end()) continue;
                    const Location& storeLoc = storeIt->getLocation();

                    auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& orderer) {    // �ֹ��� ã��
                        return orderer.getId() == order->getOrdererId();
                        });
                    if (ordererIt == orderers.end()) continue;
                    const Location& ordererLoc = ordererIt->getLocation();
                    
                    double distToStore = map.calculateShortestDistance(driver.getCurrentLocation(), storeLoc);  // ��� �� ����, ���� �� �ֹ��� �ִܰŸ� �ջ�
                    double distToOrderer = map.calculateShortestDistance(storeLoc, ordererLoc);
                    double totalDist = distToStore + distToOrderer;

					orderHeap.push({ totalDist, order });                                                       // �Ÿ��� �ֹ� ���� ���� �߰�
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
}

DeliverySystemWithDriverCall::DeliverySystemWithDriverCall() : DeliverySystem() {}                  // �θ� ������ ȣ��

DeliverySystemWithDriverCall::~DeliverySystemWithDriverCall() = default;                            // �θ� �Ҹ��� ȣ��

#endif