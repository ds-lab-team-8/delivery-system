#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "delivery_system.h"
#include "../entities/orderer.h"
#include "../entities/store.h"
#include "../entities/driver.h"
#include "../utils/map.h"

using namespace std;

struct DriverStats;
struct OrderStats;

enum SystemType {
    MOCK,
    DRIVER_CALL,
    SYSTEM_SELECTION
};

class Simulator {
public:
    Simulator();
    ~Simulator();

    void simulateWithUserInput();

private:
    DeliverySystem* deliverySystem;

    // 시뮬레이션을 위한 데이터 저장
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<Order*> orders;

    SystemType systemType;

    // ID 자동 증가 카운터
    int nextOrdererId;
    int nextDriverId;
    int nextStoreId;
    int nextOrderId;

    // 시뮬레이션 메서드
    void runSimulation();
    void switchSystemType(SystemType newType);

    // 출력 메서드
    void printSimulationResults(const map<int, DriverStats>& driverStats,
                                const map<int, OrderStats>& orderStats,
                                double totalTime,
                                const vector<string>& eventLogs);
    void saveResultsToFile(const map<int, DriverStats>& driverStats,
                          const map<int, OrderStats>& orderStats,
                          double totalTime,
                          const vector<string>& eventLogs);

    // UI 헬퍼 메서드
    void printHeader();
    void printSeparator();
    void printHelp();

    // 유틸리티 메서드
    string generateRandomName(const string& prefix);
    int generateRandomCoordinate(int min, int max);

    // 조회 메서드
    void listAll();
    void listOrderers();
    void listDrivers();
    void listStores();
    void listOrders();
};

#endif