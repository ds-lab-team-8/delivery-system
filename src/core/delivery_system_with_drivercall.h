#ifndef DELIVERY_SYSTEM_WITH_DRIVER_CALL_H
#define DELIVERY_SYSTEM_WITH_DRIVER_CALL_H

#include "delivery_system.h"

class DeliverySystemWithDriverCall : public DeliverySystem {
public:
    DeliverySystemWithDriverCall();
    ~DeliverySystemWithDriverCall();
	
    // DeliverySystem_drivercall 인지 DeliverySystem_systemselection 인지에 따라 변동될 메서드
    void acceptCall() override;
}

#endif