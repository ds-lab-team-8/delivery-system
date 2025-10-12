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

class Simulator {
public:
    Simulator();
    ~Simulator();

    void simulateWithUserInput();

private:
    DeliverySystem deliverySystem;

    // 시뮬레이션을 위한 데이터 저장
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<Order*> orders;

    // 실제 DeliverySystem 구현 사용 여부 (false: Mock 모드, true: 실제 구현 사용)
    bool useRealImplementation;

    // ID 자동 증가 카운터
    int nextOrdererId;
    int nextDriverId;
    int nextStoreId;
    int nextOrderId;

    // 시뮬레이션 메서드
    void runSimulation();

    // 출력 메서드
    void printSimulationResults(const map<int, DriverStats>& driverStats,
                                const map<int, OrderStats>& orderStats,
                                double totalTime);
    void saveResultsToFile(const map<int, DriverStats>& driverStats,
                          const map<int, OrderStats>& orderStats,
                          double totalTime);

    // UI 헬퍼 메서드
    void printHeader();
    void printSeparator();
    void printHelp();
};

#endif