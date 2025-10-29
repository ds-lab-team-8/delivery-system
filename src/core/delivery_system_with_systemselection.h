#ifndef DELIVERY_SYSTEM_WITH_SYSTEM_SELECTION_H
#define DELIVERY_SYSTEM_WITH_SYSTEM_SELECTION_H

#include "delivery_system.h"

class DeliverySystemWithSystemSelection : public DeliverySystem {
public:
    DeliverySystemWithSystemSelection();
    ~DeliverySystemWithSystemSelection();
    
    void acceptCall() override;
};

#endif