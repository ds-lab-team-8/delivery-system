#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <vector>
#include "delivery_system.h"
#include "../entities/orderer.h"
#include "../entities/store.h"
#include "../entities/driver.h"
#include "../utils/map.h"

using namespace std;

enum SimulationCommand {
    ADD_ORDERER,
    ADD_STORE,
    ADD_DRIVER,
    PLACE_ORDER,
    VIEW_STATUS,
    VIEW_DRIVER_STATUS,
    VIEW_STORE_STATUS,
    RUN_SIMULATION_STEP,
    EXIT_SIMULATION
};

class Simulator {
    public:
        Simulator();
        ~Simulator();

        // Initialization methods
        void initializeSystem();
        void setupDefaultParticipants();
        void addOrderer(int id, const string& name, int x, int y);
        void addStore(int id, const string& name, int x, int y);
        void addDriver(int id, const string& name, int x, int y);

        // Simulation control methods
        void startSimulation();
        void runSimulationStep();
        void processUserCommand();
        void executeCommand(SimulationCommand command, const vector<string>& params);

    private:
        DeliverySystem* deliverySystem;
        Map* map;
        bool simulationRunning;
        int simulationStep;

        // Default setup methods
        void createDefaultOrderers();
        void createDefaultStores();
        void createDefaultDrivers();
};

#endif