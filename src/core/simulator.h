#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include "delivery_system.h"
#include "../entities/orderer.h"
#include "../entities/store.h"
#include "../entities/driver.h"
#include "../utils/map.h"

using namespace std;

struct DriverStats;
struct OrderStats;
struct OrderSchedule;

enum SystemType {
    MOCK,
    DRIVER_CALL,
    SYSTEM_SELECTION
};

// 배차 전략을 위한 전략 패턴
class DispatchStrategy {
public:
    virtual ~DispatchStrategy() {}
    virtual bool shouldDispatch(int currentTime, int lastDispatchTime) = 0;
    virtual int getDispatchInterval() const = 0;
    virtual string getName() const = 0;
};

class SystemSelectionStrategy : public DispatchStrategy {
private:
    static const int DISPATCH_INTERVAL = 1; // 1초마다 배차
public:
    bool shouldDispatch(int currentTime, int lastDispatchTime) override;
    int getDispatchInterval() const override;
    string getName() const override;
};

class DriverCallStrategy : public DispatchStrategy {
public:
    bool shouldDispatch(int currentTime, int lastDispatchTime) override;
    int getDispatchInterval() const override;
    string getName() const override;
};

class Simulator {
public:
    Simulator();
    ~Simulator();

    void simulateWithUserInput();

private:
    DeliverySystem* deliverySystem;
    DispatchStrategy* dispatchStrategy;

    // 시뮬레이션을 위한 데이터 저장
    vector<Orderer> orderers;
    vector<Driver> drivers;
    vector<Store> stores;
    vector<OrderSchedule> scheduledOrders;  // 시간 정보와 함께 저장되는 주문들

    SystemType systemType;

    // 실시간 시뮬레이션 설정
    static const double DRIVER_SPEED; // 기사 이동 속도 (거리 단위/초)
    static const int DEFAULT_SIMULATION_TIME; // 기본 시뮬레이션 시간 (초)
    int simulationTimeLimit; // 사용자 설정 시뮬레이션 시간 (초)

    // ID 자동 증가 카운터
    int nextOrdererId;
    int nextDriverId;
    int nextStoreId;
    int nextOrderId;

    // 시뮬레이션 메서드
    void runSimulation();
    void runRealTimeSimulation();  // 새로운 실시간 시뮬레이션
    void switchSystemType(SystemType newType);

    // 출력 메서드
    void printSimulationResults(const map<int, DriverStats>& driverStats,
                                const map<int, OrderStats>& orderStats,
                                double totalTime,
                                const vector<string>& eventLogs,
                                const map<int, string>& driverNames);
    void saveResultsToFile(const map<int, DriverStats>& driverStats,
                          const map<int, OrderStats>& orderStats,
                          double totalTime,
                          const vector<string>& eventLogs);

    // UI 헬퍼 메서드
    void printHeader();
    void printSeparator();
    void printHelp();

    // 시뮬레이션 상태 출력 메서드 (나중에 visualize()로 교체 예정)
    void printSimulationStatus(int currentTime, const map<int, Location>& driverLocations,
                              const map<int, int>& driverStates, const vector<Order*>& pendingOrders);

    // 유틸리티 메서드
    string generateRandomName(const string& prefix);
    int generateRandomCoordinate(int min, int max);
    int generateRandomOrderTime();  // 주문 시간 랜덤 생성

    // 조회 메서드
    void listAll();
    void listOrderers();
    void listDrivers();
    void listStores();
    void listOrders();
};

// 시간 정보를 포함한 주문 스케줄 구조체
struct OrderSchedule {
    Order* order;
    int orderTime;  // 주문이 발생하는 시간 (초)

    OrderSchedule(Order* o, int time) : order(o), orderTime(time) {}
};

#endif