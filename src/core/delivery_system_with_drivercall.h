#ifndef DELIVERY_SYSTEM_WITH_DRIVER_CALL_H
#define DELIVERY_SYSTEM_WITH_DRIVER_CALL_H

#include "delivery_system.h"

class DeliverySystemWithDriverCall : public DeliverySystem {
public:
    DeliverySystemWithDriverCall();
    ~DeliverySystemWithDriverCall();

    void acceptCall() override;

protected:
	vector<vector<Order*>> generateOrderCombos(const vector<Order*>& availableOrders, int maxComboSize = 3);
	double bestDistanceForOrderCombo(const vector<Order*>& orderCombo, const Driver& driver, const Map& map);
	double computeEfficiency(const vector<Order*>& group, double totalDist);

};

#endif