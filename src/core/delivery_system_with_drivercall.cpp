#include <queue>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include "delivery_system_with_drivercall.h"

using namespace std;

DeliverySystemWithDriverCall::DeliverySystemWithDriverCall() : DeliverySystem() {}

DeliverySystemWithDriverCall::~DeliverySystemWithDriverCall() = default;

vector<vector<Order*>> DeliverySystemWithDriverCall::generateOrderCombos(const vector<Order*>& availableOrders, int maxComboSize) {
    vector<vector<Order*>> combos;
    int n = availableOrders.size();
    int maxSize = min(maxComboSize, n);
    
    for (int size = 1; size <= maxSize; ++size) {
        vector<int> indices(size);
        iota(indices.begin(), indices.end(), 0);
        do {
            vector<Order*> combo;
            for (int index : indices) {
                combo.push_back(availableOrders[index]);
            }
            combos.push_back(combo);
		} while (next_combination(indices.begin(), indices.end(), n));
    }
    return combos;
}

double DeliverySystemWithDriverCall::bestDistanceForOrderCombo(const vector<Order*>& orderCombo, const Driver& driver, const Map& map) {
    struct Node {
        int orderId;
        bool isPickup;
        int node;
        bool operator<(const Node& other) const {
            if (orderId != other.orderId) return orderId < other.orderId;
            return (isPickup && !other.isPickup);
        }
    };

    vector<Node> nodes;
    for (Order* order : orderCombo) {
        nodes.push_back({ order->getOrderId(), true,  order->getStore()->getLocation().getNode() });
        nodes.push_back({ order->getOrderId(), false, order->getOrderer()->getLocation().getNode() });
    }

    sort(nodes.begin(), nodes.end());

    double bestDist = numeric_limits<double>::max();

    do {
        bool valid = true;
        set<int> picked;
        for (auto& n : nodes) {
            if (n.isPickup) picked.insert(n.orderId);
            else if (!picked.count(n.orderId)) { valid = false; break; }
        }
        if (!valid) continue;

        double distSum = 0;
        int cur = driver.getCurrentLocation().getNode();
        for (auto& n : nodes) {
            distSum += map.GetMap_cost(cur, n.node);
            cur = n.node;
        }
        bestDist = min(bestDist, distSum);

    } while (next_permutation(nodes.begin(), nodes.end()));

    return bestDist;
}

double DeliverySystemWithDriverCall::computeEfficiency(const vector<Order*>& group, double totalDist) {
    double totalFee = 0;
    for (Order* order : group) {
        totalFee += order->getDeliveryFee();
    }
    return totalFee / totalDist;
}

void DeliverySystemWithDriverCall::acceptCall() {
    Map& map = getMap();
    vector<Driver>& drivers = getDrivers();
    vector<Order*>& orders = getOrders();
    int limitOrderReceive = getLimitOrderReceive();
    set<int> assignedOrderIds;

    for (Driver& driver : drivers) {
        if (!driver.isAvailable()) continue;
        vector<Order*> availableOrders;

        for (Order* order : orders) {
            if (order->getStatus() == ORDER_ACCEPTED && !assignedOrderIds.count(order->getOrderId())) {
                availableOrders.push_back(order);
            }
        }

        if (availableOrders.empty()) continue;

        vector<vector<Order*>> orderCombos = generateOrderCombos(availableOrders, limitOrderReceive);
        double bestEfficiency = -1.0;
        vector<Order*> bestGroup;
        for (const auto& group : orderCombos) {
            double bestDist = bestDistanceForOrderCombo(group, driver, map);
            double efficiency = computeEfficiency(group, bestDist);
            if (efficiency > bestEfficiency) {
                bestEfficiency = efficiency;
                bestGroup = group;
            }
        }

        for (Order* order : bestGroup) {
            if (assignedOrderIds.count(order->getOrderId())) continue;
            if (order->getStatus() != ORDER_ACCEPTED) continue;

            if (assignOrderToDriver(order, driver)) {
                assignedOrderIds.insert(order->getOrderId());
            }
        }
    }

}
