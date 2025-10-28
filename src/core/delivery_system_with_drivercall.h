#ifndef DELIVERY_SYSTEM_WITH_DRIVER_CALL_H
#define DELIVERY_SYSTEM_WITH_DRIVER_CALL_H

#include "delivery_system.h"

class DeliverySystemWithDriverCall : public DeliverySystem {
public:
    DeliverySystemWithDriverCall();
    ~DeliverySystemWithDriverCall();
	
    // DeliverySystem_drivercall ���� DeliverySystem_systemselection ������ ���� ������ �޼���
    void acceptCall() override;
}

#endif