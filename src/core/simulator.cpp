#include "simulator.h"
#include "delivery_system_with_drivercall.h"
#include "delivery_system_with_systemselection.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>
#include <cmath>
#include <algorithm>
#include <random>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <thread>

// 통계 데이터 구조체
struct DriverStats {
    int deliveryCount;
    double totalDistance;
    vector<int> completedOrders;

    DriverStats() : deliveryCount(0), totalDistance(0.0) {}
};

struct OrderStats {
    int orderId;
    int driverId;
    double pickupDistance;
    double deliveryDistance;
    double totalTime;

    OrderStats() : orderId(0), driverId(0), pickupDistance(0.0),
                   deliveryDistance(0.0), totalTime(0.0) {}
};

enum EventType {
    EVENT_ORDER_ASSIGNED,
    EVENT_PICKUP_COMPLETE,
    EVENT_DELIVERY_COMPLETE
};

struct Event {
    double time;
    EventType type;
    int orderId;
    int driverId;
    Location location;
    double distance;

    Event(double t, EventType et, int oid, int did, Location loc, double dist = 0.0)
        : time(t), type(et), orderId(oid), driverId(did), location(loc), distance(dist) {}

    bool operator<(const Event& other) const {
        return time < other.time;
    }
};

// 시뮬레이터 상수 정의
const double Simulator::DRIVER_SPEED = 1.0;
const int Simulator::DEFAULT_SIMULATION_TIME = 600; // 기본 10분 = 600초
int Simulator::nextOrderTime = 5; // 첫 번째 주문은 5초에 시작

// 전략 패턴 구현
bool SystemSelectionStrategy::shouldDispatch(int currentTime, int lastDispatchTime) {
    return (currentTime - lastDispatchTime) >= DISPATCH_INTERVAL;
}

int SystemSelectionStrategy::getDispatchInterval() const {
    return DISPATCH_INTERVAL;
}

string SystemSelectionStrategy::getName() const {
    return "SystemSelection (1초마다 배차)";
}

bool DriverCallStrategy::shouldDispatch(int, int) {
    // DriverCall은 주문 발생 시점과 기사 배달 완료 시점에 트리거됨
    // 실제 트리거 조건은 시뮬레이션 루프에서 직접 처리
    return false;
}

int DriverCallStrategy::getDispatchInterval() const {
    return 0; // 이벤트 기반이므로 고정 간격 없음
}

string DriverCallStrategy::getName() const {
    return "DriverCall (이벤트 트리거 방식)";
}

Simulator::Simulator() : deliverySystem(nullptr),
                         dispatchStrategy(nullptr),
                         systemType(DRIVER_CALL),
                         simulationTimeLimit(DEFAULT_SIMULATION_TIME),
                         nextOrdererId(1), nextDriverId(1),
                         nextStoreId(1), nextOrderId(1) {
    // 기본값으로 DRIVER_CALL 시스템 초기화
    switchSystemType(DRIVER_CALL);
}

Simulator::~Simulator() {
    if (deliverySystem) {
        delete deliverySystem;
    }

    if (dispatchStrategy) {
        delete dispatchStrategy;
    }

    for (OrderSchedule& orderSchedule : scheduledOrders) {
        delete orderSchedule.order;
    }
}

void Simulator::switchSystemType(SystemType newType) {
    if (deliverySystem) {
        delete deliverySystem;
        deliverySystem = nullptr;
    }

    if (dispatchStrategy) {
        delete dispatchStrategy;
        dispatchStrategy = nullptr;
    }

    systemType = newType;

    switch (systemType) {
        case DRIVER_CALL:
            deliverySystem = new DeliverySystemWithDriverCall();
            dispatchStrategy = new DriverCallStrategy();
            cout << "[시스템 모드 변경] DriverCall 방식으로 변경되었습니다." << endl;
            break;
        case SYSTEM_SELECTION:
            deliverySystem = new DeliverySystemWithSystemSelection();
            dispatchStrategy = new SystemSelectionStrategy();
            cout << "[시스템 모드 변경] SystemSelection 방식으로 변경되었습니다." << endl;
            break;
        case MOCK:
            deliverySystem = nullptr;
            dispatchStrategy = nullptr;
            cout << "[시스템 모드 변경] Mock 모드로 변경되었습니다." << endl;
            break;
    }
}

void Simulator::simulateWithUserInput() {
    printHeader();

    string modeStr;
    switch (systemType) {
        case MOCK: modeStr = "Mock 모드"; break;
        case DRIVER_CALL: modeStr = "DriverCall 방식"; break;
        case SYSTEM_SELECTION: modeStr = "SystemSelection 방식"; break;
    }
    cout << "\n[실행 모드: " << modeStr << "]" << endl;

    printHelp();

    string command;
    bool running = true;

    while (running) {
        cout << "\n> ";
        getline(cin, command);

        if (command.empty()) continue;

        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "add_orderer") {
            int x, y;
            string name;
            iss >> name >> x >> y;

            int id = nextOrdererId++;
            Location loc(x, y);
            Orderer orderer(id, name, loc);

            if (deliverySystem) {
                deliverySystem->addOrderer(orderer);
            }

            orderers.push_back(orderer);

            cout << "[주문자 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_driver") {
            int x, y;
            string name;
            iss >> name >> x >> y;

            int id = nextDriverId++;
            Location loc(x, y);
            Driver driver(id, name, loc);

            if (deliverySystem) {
                deliverySystem->addDriver(driver);
            }

            drivers.push_back(driver);

            cout << "[기사 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_store") {
            int x, y;
            double feePerDistance = 200.0; // 기본 배달 단가 (거리 1당 200원)
            string name;
            iss >> name >> x >> y;

            // 배달 단가가 주어진 경우
            if (iss >> feePerDistance) {
                // 추가 입력 처리됨
            }

            int id = nextStoreId++;
            Location loc(x, y);
            Store store(id, name, loc, feePerDistance);

            if (deliverySystem) {
                deliverySystem->addStore(store);
            }

            stores.push_back(store);

            cout << "[매장 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << "), 배달 단가: " << (int)feePerDistance << "원/거리" << endl;

        } else if (cmd == "add_order") {
            int ordererId, storeId, x, y, orderTime = -1;
            iss >> ordererId >> storeId >> x >> y;

            // 주문 시간이 주어진 경우
            if (iss >> orderTime) {
                // 추가 입력 처리됨
            } else {
                // 주문 시간이 주어지지 않으면 랜덤 생성
                orderTime = generateRandomOrderTime();
            }

            int orderId = nextOrderId++;
            Location deliveryLoc(x, y);
            Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

            auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& o) {
                return o.getId() == ordererId;
            });
            if (ordererIt != orderers.end()) {
                order->setOrderer(&(*ordererIt));
            }

            auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& s) {
                return s.getId() == storeId;
            });
            if (storeIt != stores.end()) {
                order->setStore(&(*storeIt));
                // 배달비 계산
                double distance = storeIt->getLocation().calculateDistance(deliveryLoc);
                double deliveryFee = distance * storeIt->getFeePerDistance();
                order->setDeliveryFee(deliveryFee);
            }

            scheduledOrders.push_back(OrderSchedule(order, orderTime));

            cout << "[주문 추가] 주문 ID: " << orderId
                 << ", 주문자 ID: " << ordererId
                 << ", 매장 ID: " << storeId
                 << ", 배달 위치: (" << x << ", " << y << ")"
                 << ", 주문 시간: " << orderTime << "초"
                 << ", 배달비: " << (int)order->getDeliveryFee() << "원" << endl;

        } else if (cmd == "add_orderer_random") {
            int n;
            iss >> n;

            if (n <= 0) {
                cout << "개수는 1 이상이어야 합니다." << endl;
                continue;
            }

            cout << "[" << n << "명의 주문자를 랜덤으로 추가합니다]" << endl;
            for (int i = 0; i < n; i++) {
                string name = generateRandomName("Orderer");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextOrdererId++;

                Location loc(x, y);
                Orderer orderer(id, name, loc);

                if (deliverySystem) {
                    deliverySystem->addOrderer(orderer);
                }

                orderers.push_back(orderer);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
            }

        } else if (cmd == "add_driver_random") {
            int n;
            iss >> n;

            if (n <= 0) {
                cout << "개수는 1 이상이어야 합니다." << endl;
                continue;
            }

            cout << "[" << n << "명의 기사를 랜덤으로 추가합니다]" << endl;
            for (int i = 0; i < n; i++) {
                string name = generateRandomName("Driver");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextDriverId++;

                Location loc(x, y);
                Driver driver(id, name, loc);

                if (deliverySystem) {
                    deliverySystem->addDriver(driver);
                }

                drivers.push_back(driver);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
            }

        } else if (cmd == "add_store_random") {
            int n;
            iss >> n;

            if (n <= 0) {
                cout << "개수는 1 이상이어야 합니다." << endl;
                continue;
            }

            cout << "[" << n << "개의 매장을 랜덤으로 추가합니다]" << endl;
            static random_device rd;
            static mt19937 gen(rd());
            uniform_real_distribution<> feeDis(150.0, 250.0); // 배달 단가 150~250원/거리 랜덤

            for (int i = 0; i < n; i++) {
                string name = generateRandomName("Store");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                double feePerDistance = feeDis(gen);
                int id = nextStoreId++;

                Location loc(x, y);
                Store store(id, name, loc, feePerDistance);

                if (deliverySystem) {
                    deliverySystem->addStore(store);
                }

                stores.push_back(store);
                cout << "  ID: " << id << ", 이름: " << name
                     << ", 위치: (" << x << ", " << y << "), 배달 단가: 200원/거리" << endl;
            }

        } else if (cmd == "add_order_random") {
            int n;
            iss >> n;

            if (n <= 0) {
                cout << "개수는 1 이상이어야 합니다." << endl;
                continue;
            }

            if (orderers.empty()) {
                cout << "주문자가 없습니다. 먼저 주문자를 추가해주세요." << endl;
                continue;
            }

            if (stores.empty()) {
                cout << "매장이 없습니다. 먼저 매장을 추가해주세요." << endl;
                continue;
            }

            cout << "[" << n << "건의 주문을 랜덤으로 추가합니다]" << endl;
            
            static random_device rd;
            static mt19937 gen(rd());
            uniform_int_distribution<> ordererDis(0, orderers.size() - 1);
            uniform_int_distribution<> storeDis(0, stores.size() - 1);

            for (int i = 0; i < n; i++) {
                int orderId = nextOrderId++;
                int ordererId = orderers[ordererDis(gen)].getId();
                int storeId = stores[storeDis(gen)].getId();
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int orderTime = generateRandomOrderTime();

                Location deliveryLoc(x, y);
                Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

                auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& o) {
                    return o.getId() == ordererId;
                });
                if (ordererIt != orderers.end()) {
                    order->setOrderer(&(*ordererIt));
                }

                auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& s) {
                    return s.getId() == storeId;
                });
                if (storeIt != stores.end()) {
                    order->setStore(&(*storeIt));
                    // 배달비 계산
                    double distance = storeIt->getLocation().calculateDistance(deliveryLoc);
                    double deliveryFee = distance * storeIt->getFeePerDistance();
                    order->setDeliveryFee(deliveryFee);
                }

                scheduledOrders.push_back(OrderSchedule(order, orderTime));
                cout << "  주문 ID: " << orderId
                     << ", 주문자 ID: " << ordererId
                     << ", 매장 ID: " << storeId
                     << ", 배달 위치: (" << x << ", " << y << ")"
                     << ", 주문 시간: " << orderTime << "초"
                     << ", 배달비: " << (int)order->getDeliveryFee() << "원" << endl;
            }

        } else if (cmd == "add_all_random" || cmd == "aar") {
            int nOrderers, nDrivers, nStores, nOrders;
            iss >> nOrderers >> nDrivers >> nStores >> nOrders;

            if (nOrderers <= 0 || nDrivers <= 0 || nStores <= 0 || nOrders <= 0) {
                cout << "모든 개수는 1 이상이어야 합니다." << endl;
                continue;
            }

            cout << "\n[전체 랜덤 데이터 생성]" << endl;
            printSeparator();

            // 주문자 추가
            cout << "\n[주문자 " << nOrderers << "명 추가]" << endl;
            for (int i = 0; i < nOrderers; i++) {
                string name = generateRandomName("Orderer");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextOrdererId++;

                Location loc(x, y);
                Orderer orderer(id, name, loc);

                if (deliverySystem) {
                    deliverySystem->addOrderer(orderer);
                }

                orderers.push_back(orderer);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
            }

            // 기사 추가
            cout << "\n[기사 " << nDrivers << "명 추가]" << endl;
            for (int i = 0; i < nDrivers; i++) {
                string name = generateRandomName("Driver");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextDriverId++;

                Location loc(x, y);
                Driver driver(id, name, loc);

                if (deliverySystem) {
                    deliverySystem->addDriver(driver);
                }

                drivers.push_back(driver);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
            }

            // 매장 추가
            cout << "\n[매장 " << nStores << "개 추가]" << endl;
            for (int i = 0; i < nStores; i++) {
                string name = generateRandomName("Store");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextStoreId++;

                Location loc(x, y);
                Store store(id, name, loc, 200.0); // 기본 배달 단가

                if (deliverySystem) {
                    deliverySystem->addStore(store);
                }

                stores.push_back(store);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
            }

            // 주문 추가
            cout << "\n[주문 " << nOrders << "건 추가]" << endl;
            
            static random_device rd;
            static mt19937 gen(rd());
            uniform_int_distribution<> ordererDis(0, orderers.size() - 1);
            uniform_int_distribution<> storeDis(0, stores.size() - 1);

            for (int i = 0; i < nOrders; i++) {
                int orderId = nextOrderId++;
                int ordererId = orderers[ordererDis(gen)].getId();
                int storeId = stores[storeDis(gen)].getId();
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);

                Location deliveryLoc(x, y);
                Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

                auto ordererIt = find_if(orderers.begin(), orderers.end(), [&](const Orderer& o) {
                    return o.getId() == ordererId;
                });
                if (ordererIt != orderers.end()) {
                    order->setOrderer(&(*ordererIt));
                }

                auto storeIt = find_if(stores.begin(), stores.end(), [&](const Store& s) {
                    return s.getId() == storeId;
                });
                if (storeIt != stores.end()) {
                    order->setStore(&(*storeIt));
                }

                int orderTime = generateRandomOrderTime();

                // 배달비 계산
                auto storeIt2 = find_if(stores.begin(), stores.end(), [&](const Store& s) {
                    return s.getId() == storeId;
                });
                if (storeIt2 != stores.end()) {
                    double distance = storeIt2->getLocation().calculateDistance(deliveryLoc);
                    double deliveryFee = distance * storeIt2->getFeePerDistance();
                    order->setDeliveryFee(deliveryFee);
                }

                scheduledOrders.push_back(OrderSchedule(order, orderTime));
                cout << "  주문 ID: " << orderId
                     << ", 주문자 ID: " << ordererId
                     << ", 매장 ID: " << storeId
                     << ", 배달 위치: (" << x << ", " << y << ")"
                     << ", 주문 시간: " << orderTime << "초"
                     << ", 배달비: " << (int)order->getDeliveryFee() << "원" << endl;
            }

            cout << "\n[완료] 주문자 " << nOrderers << "명, 기사 " << nDrivers 
                 << "명, 매장 " << nStores << "개, 주문 " << nOrders << "건이 추가되었습니다." << endl;
            printSeparator();

        } else if (cmd == "list") {
            listAll();

        } else if (cmd == "switch_system" || cmd == "ss") {
            string systemTypeStr;
            iss >> systemTypeStr;
            
            if (systemTypeStr.empty()) {
                // 인자가 없으면 driver_call과 system_selection 사이에서 토글
                if (systemType == DRIVER_CALL) {
                    switchSystemType(SYSTEM_SELECTION);
                } else if (systemType == SYSTEM_SELECTION) {
                    switchSystemType(DRIVER_CALL);
                } else {
                    // mock이거나 초기 상태면 driver_call로 설정
                    switchSystemType(DRIVER_CALL);
                }
            } else if (systemTypeStr == "mock") {
                switchSystemType(MOCK);
            } else if (systemTypeStr == "driver_call") {
                switchSystemType(DRIVER_CALL);
            } else if (systemTypeStr == "system_selection") {
                switchSystemType(SYSTEM_SELECTION);
            } else {
                cout << "잘못된 시스템 타입입니다. (driver_call, system_selection 중 하나를 입력하세요)" << endl;
            }

        } else if (cmd == "set_time") {
            int timeInSeconds;
            iss >> timeInSeconds;

            if (timeInSeconds <= 0) {
                cout << "시간은 1초 이상이어야 합니다." << endl;
                continue;
            }

            simulationTimeLimit = timeInSeconds;
            int minutes = timeInSeconds / 60;
            int seconds = timeInSeconds % 60;

            cout << "[시뮬레이션 시간 설정] " << timeInSeconds << "초";
            if (minutes > 0) {
                cout << " (" << minutes << "분 " << seconds << "초)";
            }
            cout << "로 설정되었습니다." << endl;

        } else if (cmd == "start" || cmd == "s") {
            int minutes = simulationTimeLimit / 60;
            int seconds = simulationTimeLimit % 60;
            cout << "\n실시간 시뮬레이션을 시작합니다 (시간 제한: "
                 << simulationTimeLimit << "초";
            if (minutes > 0) {
                cout << " = " << minutes << "분 " << seconds << "초";
            }
            cout << ")...\n" << endl;
            runRealTimeSimulation();

        } else if (cmd == "quit" || cmd == "exit" || cmd == "q") {
            cout << "시뮬레이터를 종료합니다." << endl;
            running = false;

        } else if (cmd == "help") {
            printHelp();

        } else {
            cout << "알 수 없는 명령어입니다. 'help'를 입력하여 사용 가능한 명령어를 확인하세요." << endl;
        }
    }
}

void Simulator::runRealTimeSimulation() {
    if (scheduledOrders.empty()) {
        cout << "주문이 없습니다. 시뮬레이션을 진행할 수 없습니다." << endl;
        return;
    }

    if (drivers.empty()) {
        cout << "기사가 없습니다. 시뮬레이션을 진행할 수 없습니다." << endl;
        return;
    }

    if (stores.empty()) {
        cout << "매장이 없습니다. 시뮬레이션을 진행할 수 없습니다." << endl;
        return;
    }

    printSeparator();
    int minutes = simulationTimeLimit / 60;
    int seconds = simulationTimeLimit % 60;
    cout << "실시간 시뮬레이션 시작 - " << dispatchStrategy->getName() << endl;
    cout << "시간 제한: " << simulationTimeLimit << "초";
    if (minutes > 0) {
        cout << " (" << minutes << "분 " << seconds << "초)";
    }
    cout << endl;
    printSeparator();

    // 주문을 시간 순으로 정렬
    sort(scheduledOrders.begin(), scheduledOrders.end(),
         [](const OrderSchedule& a, const OrderSchedule& b) {
             return a.orderTime < b.orderTime;
         });

    // 시뮬레이션 상태 초기화
    map<int, Location> driverLocations;
    map<int, int> driverStates; // 0: 대기, 1: 픽업중, 2: 배달중
    map<int, double> driverAvailableTime;
    map<int, int> driverCurrentOrder;
    vector<Order*> pendingOrders;
    vector<Order*> completedOrders;

    int currentTime = 0;
    int lastDispatchTime = -1;
    int orderIndex = 0;

    for (const Driver& driver : drivers) {
        driverLocations[driver.getId()] = driver.getCurrentLocation();
        driverStates[driver.getId()] = 0; // 대기 상태
        driverAvailableTime[driver.getId()] = 0.0;
        driverCurrentOrder[driver.getId()] = -1;
    }

    // 배달 시스템 초기화
    if (deliverySystem) {
        for (const Orderer& orderer : orderers) {
            deliverySystem->addOrderer(orderer);
        }
        for (const Driver& driver : drivers) {
            deliverySystem->addDriver(driver);
        }
        for (const Store& store : stores) {
            deliverySystem->addStore(store);
        }
        deliverySystem->initializeMap();
    }

    cout << "[시간: 0초] 시뮬레이션 시작" << endl;

    // 메인 시뮬레이션 루프
    while (!pendingOrders.empty() ||
           any_of(driverStates.begin(), driverStates.end(),
                  [](const pair<int, int>& p) { return p.second != 0; }) ||
           (currentTime < simulationTimeLimit && static_cast<size_t>(orderIndex) < scheduledOrders.size())) {

        bool newOrderAdded = false;
        bool driverCompleted = false;

        // 기사 상태 업데이트 (실제 위치 기반 픽업/배달 완료 처리) - 먼저 처리
        for (auto& driverPair : driverStates) {
            int driverId = driverPair.first;
            int& state = driverPair.second;

            if (state != 0) { // 이동 중인 기사들 체크
                int orderId = driverCurrentOrder[driverId];
                Order* order = nullptr;

                // 현재 주문 찾기 (deliverySystem에서)
                if (deliverySystem) {
                    vector<Order*>& systemOrders = deliverySystem->getAllOrders();
                    for (Order* o : systemOrders) {
                        if (o->getOrderId() == orderId) {
                            order = o;
                            break;
                        }
                    }
                }

                if (order) {
                    Location currentPos = driverLocations[driverId];
                    Location targetPos;

                    if (state == 1) { // 픽업 중 - 매장으로 이동
                        targetPos = order->getStore()->getLocation();
                    } else { // 배달 중 - 배달지로 이동
                        targetPos = order->getDeliveryLocation();
                    }

                    // 목적지 도착 확인 (거리 1.0 이하면 도착으로 간주)
                    double distanceToTarget = currentPos.calculateDistance(targetPos);

                    if (distanceToTarget <= DRIVER_SPEED) {
                        // 목적지 도착!
                        driverLocations[driverId] = targetPos; // 정확한 목적지 위치로 설정

                        if (state == 1) { // 픽업 완료
                            state = 2; // 배달 중 상태로 변경
                            order->completePickup();

                            Location storeLocation = order->getStore()->getLocation();
                            Location deliveryLoc = order->getDeliveryLocation();

                            cout << "[시간: " << currentTime << "초] 기사 #" << driverId
                                 << " 픽업 완료! 매장: (" << storeLocation.getX() << ", " << storeLocation.getY()
                                 << ") → 배달지: (" << deliveryLoc.getX() << ", " << deliveryLoc.getY()
                                 << ") 배달 시작! (주문 ID: " << orderId << ")" << endl;

                        } else if (state == 2) { // 배달 완료
                            state = 0; // 대기 상태로 변경
                            driverCurrentOrder[driverId] = -1;

                            // 완료된 주문 처리 (deliverySystem에서 찾기)
                            if (deliverySystem) {
                                vector<Order*>& systemOrders = deliverySystem->getAllOrders();
                                for (auto it = systemOrders.begin(); it != systemOrders.end(); ++it) {
                                    Order* o = *it;
                                    if (o->getOrderId() == orderId) {
                                        o->completeDelivery();
                                        completedOrders.push_back(o);

                                        cout << "[시간: " << currentTime << "초] 기사 #" << driverId
                                             << " 배달 완료! 최종 위치: (" << targetPos.getX() << ", " << targetPos.getY()
                                             << "), 배달비: " << (int)o->getDeliveryFee() << "원 (주문 ID: " << orderId << ")" << endl;

                                        // 완료된 주문을 deliverySystem에서도 제거
                                        systemOrders.erase(it);
                                        break;
                                    }
                                }
                            }

                            driverCompleted = true;
                        }
                    }
                }
            }
        }

        // 새로운 주문 추가 (시간 제한 내에서만)
        while (currentTime < simulationTimeLimit &&
               static_cast<size_t>(orderIndex) < scheduledOrders.size() &&
               scheduledOrders[orderIndex].orderTime <= currentTime) {
            Order* newOrder = scheduledOrders[orderIndex].order;
            pendingOrders.push_back(newOrder);

            if (deliverySystem) {
                deliverySystem->addOrder(*newOrder);
            }

            cout << "[시간: " << currentTime << "초] 새로운 주문 발생 - 주문 ID: "
                 << newOrder->getOrderId() << ", 배달비: " << (int)newOrder->getDeliveryFee() << "원" << endl;

            newOrderAdded = true;
            orderIndex++;
        }

        // 이동 중인 기사들의 실시간 위치 업데이트 (매초 점진적 이동)
        map<int, Location> driverTargets; // 기사별 목적지
        map<int, Location> driverStartPositions; // 기사별 이동 시작 위치
        map<int, double> driverStartTimes; // 기사별 이동 시작 시간

        for (auto& driverPair : driverStates) {
            int driverId = driverPair.first;
            int state = driverPair.second;

            if (state == 1 || state == 2) { // 픽업 중 또는 배달 중
                int orderId = driverCurrentOrder[driverId];
                Order* order = nullptr;

                // 현재 주문 찾기 (deliverySystem에서)
                if (deliverySystem) {
                    vector<Order*>& systemOrders = deliverySystem->getAllOrders();
                    for (Order* o : systemOrders) {
                        if (o->getOrderId() == orderId) {
                            order = o;
                            break;
                        }
                    }
                }

                if (order) {
                    Location currentPos = driverLocations[driverId];
                    Location targetPos;

                    if (state == 1) { // 픽업 중 - 매장으로 이동
                        targetPos = order->getStore()->getLocation();
                    } else { // 배달 중 - 배달지로 이동
                        targetPos = order->getDeliveryLocation();
                    }

                    // 목적지와의 거리 계산
                    double distanceToTarget = currentPos.calculateDistance(targetPos);

                    if (distanceToTarget > DRIVER_SPEED) {
                        // 아직 목적지에 도착하지 않음 - 이동 계속
                        double dx = targetPos.getX() - currentPos.getX();
                        double dy = targetPos.getY() - currentPos.getY();
                        double totalDistance = sqrt(dx * dx + dy * dy);

                        // 단위벡터 계산 (방향)
                        double unitX = dx / totalDistance;
                        double unitY = dy / totalDistance;

                        // 속도에 따른 새로운 위치 계산
                        double newX = currentPos.getX() + (unitX * DRIVER_SPEED);
                        double newY = currentPos.getY() + (unitY * DRIVER_SPEED);

                        // 위치 업데이트
                        driverLocations[driverId] = Location((int)round(newX), (int)round(newY));

                        // 이동 로그 출력
                        cout << "[이동 " << currentTime << "초] 기사 #" << driverId
                             << (state == 1 ? "(픽업중)" : "(배달중)")
                             << ": (" << (int)round(newX) << ", " << (int)round(newY)
                             << ") → 목적지: (" << targetPos.getX() << ", " << targetPos.getY()
                             << "), 남은 거리: " << fixed << setprecision(1)
                             << driverLocations[driverId].calculateDistance(targetPos) << endl;
                    }
                }
            }
        }


        // 배차 처리 조건 확인
        bool shouldCallDispatch = false;

        if (systemType == SYSTEM_SELECTION) {
            // SystemSelection: 정기적으로 배차
            shouldCallDispatch = dispatchStrategy->shouldDispatch(currentTime, lastDispatchTime);
        } else if (systemType == DRIVER_CALL) {
            // DriverCall: 새 주문이나 기사 완료 시 배차
            shouldCallDispatch = newOrderAdded || driverCompleted;
        }

        // 배차 처리
        if (shouldCallDispatch && deliverySystem && !pendingOrders.empty()) {
            deliverySystem->acceptCall();
            lastDispatchTime = currentTime;

            // 새로 할당된 주문들 처리
            vector<Order*>& systemOrders = deliverySystem->getAllOrders();
            for (Order* order : systemOrders) {
                if (order->getDriverId() != -1 && driverStates[order->getDriverId()] == 0) {
                    int driverId = order->getDriverId();
                    driverStates[driverId] = 1; // 픽업 중 상태
                    driverCurrentOrder[driverId] = order->getOrderId();

                    // 실제 이동 기반 시스템 - driverAvailableTime 사용하지 않음
                    Location storeLocation = order->getStore()->getLocation();
                    double pickupDistance = driverLocations[driverId].calculateDistance(storeLocation);

                    cout << "[시간: " << currentTime << "초] 기사 #" << driverId
                         << " 배차 수락! 현재위치: (" << driverLocations[driverId].getX() << ", " << driverLocations[driverId].getY()
                         << ") → 매장: (" << storeLocation.getX() << ", " << storeLocation.getY()
                         << "), 거리: " << fixed << setprecision(1) << pickupDistance
                         << ", 속도: " << DRIVER_SPEED << "/초 (주문 ID: " << order->getOrderId() << ")" << endl;

                    // 할당된 주문을 pendingOrders에서 제거
                    for (auto it = pendingOrders.begin(); it != pendingOrders.end(); ++it) {
                        if ((*it)->getOrderId() == order->getOrderId()) {
                            pendingOrders.erase(it);
                            break;
                        }
                    }
                }
            }
        }

        // 현재 상태 출력 (매초마다)
        printSimulationStatus(currentTime, driverLocations, driverStates, pendingOrders, completedOrders);

        // 1초 대기 (실제 시간)
        this_thread::sleep_for(chrono::seconds(1));
        currentTime++;
    }

    cout << "\n[시뮬레이션 종료] 총 " << completedOrders.size() << "건 완료" << endl;
    printSeparator();
}

void Simulator::runSimulation() {
    // 기존 runSimulation()을 유지하여 하위 호환성 보장
    runRealTimeSimulation();
}

void Simulator::printSimulationResults(const map<int, DriverStats>& driverStats,
                                       const map<int, OrderStats>& orderStats,
                                       double totalTime,
                                       const vector<string>&,
                                       const map<int, string>& driverNames) {
    printSeparator();
    cout << "시뮬레이션 결과" << endl;
    printSeparator();

    // 기본 통계
    cout << "총 주문 수: " << orderStats.size() << "건" << endl;
    cout << "완료된 주문 수: " << orderStats.size() << "건" << endl;
    cout << "전체 소요 시간: " << fixed << setprecision(1) << totalTime << "초\n" << endl;

    // 기사별 통계
    if (!driverStats.empty()) {
        cout << "기사별 통계:" << endl;
        printSeparator();


        for (const auto& pair : driverStats) {
            int driverId = pair.first;
            const DriverStats& stats = pair.second;

            string driverName;
            auto nameIt = driverNames.find(driverId);
            if (nameIt != driverNames.end()) {
                driverName = nameIt->second;
            }

            cout << "기사 #" << driverId << " (" << driverName << ")" << endl;
            cout << "  - 배달 완료: " << stats.deliveryCount << "건" << endl;
        }
    }

    printSeparator();
}

void Simulator::saveResultsToFile(const map<int, DriverStats>&,
                                  const map<int, OrderStats>&,
                                  double,
                                  const vector<string>&) {
    cout << "\n결과 저장 기능은 추후 구현 예정입니다." << endl;
}

void Simulator::printHeader() {
    cout << "====================================" << endl;
    cout << "    배달 시스템 시뮬레이터" << endl;
    cout << "====================================" << endl;
}

void Simulator::printSeparator() {
    cout << "------------------------------------" << endl;
}

void Simulator::printHelp() {
    cout << "\n명령어:" << endl;
    printSeparator();
    cout << "  add_orderer <name> <x> <y>" << endl;
    cout << "    - 주문자를 추가합니다. (ID는 자동 생성)" << endl;
    cout << "  add_driver <name> <x> <y>" << endl;
    cout << "    - 기사를 추가합니다. (ID는 자동 생성)" << endl;
    cout << "  add_store <name> <x> <y> [feePerDistance]" << endl;
    cout << "    - 매장을 추가합니다. (ID는 자동 생성, 배달 단가 기본값: 200원/거리)" << endl;
    cout << "  add_order <ordererId> <storeId> <deliveryX> <deliveryY> [orderTime]" << endl;
    cout << "    - 주문을 추가합니다. (주문 ID는 자동 생성, 주문 시간 옵션)" << endl;
    cout << "  add_orderer_random <n>" << endl;
    cout << "    - n개의 주문자를 랜덤으로 추가합니다." << endl;
    cout << "  add_driver_random <n>" << endl;
    cout << "    - n개의 기사를 랜덤으로 추가합니다." << endl;
    cout << "  add_store_random <n>" << endl;
    cout << "    - n개의 매장을 랜덤으로 추가합니다. (배달 단가 150~250원/거리)" << endl;
    cout << "  add_order_random <n>" << endl;
    cout << "    - n개의 주문을 랜덤으로 추가합니다. (랜덤 주문 시간 포함)" << endl;
    cout << "  add_all_random <nOrderers> <nDrivers> <nStores> <nOrders> (별칭: aar)" << endl;
    cout << "    - 주문자, 기사, 매장, 주문을 한 번에 랜덤으로 추가합니다." << endl;
    cout << "  list" << endl;
    cout << "    - 모든 주문자, 기사, 매장, 주문을 조회합니다." << endl;
    cout << "  switch_system [type] (별칭: ss)" << endl;
    cout << "    - 배달 시스템 타입을 변경합니다. (driver_call, system_selection)" << endl;
    cout << "    - driver_call: 주문 발생/배달 완료 시 배차" << endl;
    cout << "    - system_selection: 1초마다 정기적으로 배차" << endl;
    cout << "  set_time <seconds>" << endl;
    cout << "    - 시뮬레이션 시간을 초 단위로 설정합니다. (기본값: 600초 = 10분)" << endl;
    cout << "  start (별칭: s)" << endl;
    cout << "    - 실시간 시뮬레이션을 시작합니다. (1초마다 진행 상황 출력)" << endl;
    cout << "  help" << endl;
    cout << "    - 도움말을 출력합니다." << endl;
    cout << "  quit / exit (별칭: q)" << endl;
    cout << "    - 시뮬레이터를 종료합니다." << endl;
    printSeparator();
}

void Simulator::printSimulationStatus(int currentTime, const map<int, Location>& driverLocations,
                                      const map<int, int>& driverStates, const vector<Order*>& pendingOrders,
                                      const vector<Order*>& completedOrders) {
    // 매 10초마다 요약 상태 출력
    if (currentTime % 10 == 0) {
        cout << "[상태 " << currentTime << "초] ";

        int waitingDrivers = 0, pickupDrivers = 0, deliveryDrivers = 0;
        for (const auto& pair : driverStates) {
            switch (pair.second) {
                case 0: waitingDrivers++; break;
                case 1: pickupDrivers++; break;
                case 2: deliveryDrivers++; break;
            }
        }

        cout << "대기: " << waitingDrivers << "명, "
             << "픽업중: " << pickupDrivers << "명, "
             << "배달중: " << deliveryDrivers << "명, "
             << "미배차: " << pendingOrders.size() << "건, "
             << "완료: " << completedOrders.size() << "건" << endl;
    }

    // 실제 이동 로그는 이동 시스템에서 처리됨 ([이동 X초] 로그)
}

string Simulator::generateRandomName(const string& prefix) {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(1000, 9999);

    return prefix + to_string(dis(gen));
}

int Simulator::generateRandomCoordinate(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

int Simulator::generateRandomOrderTime() {
    static random_device rd;
    static mt19937 gen(rd());

    // 10~20초 간격으로 랜덤 생성 (평균 15초)
    uniform_int_distribution<> intervalDis(10, 20);
    int interval = intervalDis(gen);

    // 현재 nextOrderTime에 간격을 더해서 다음 주문 시간 생성
    int currentOrderTime = nextOrderTime;
    nextOrderTime += interval;

    // 시뮬레이션 시간 제한을 초과하지 않도록 제한
    if (currentOrderTime >= simulationTimeLimit - 30) { // 30초 여유 두기
        // 시뮬레이션 시간을 초과하면 초기 시간대로 래핑
        currentOrderTime = 5 + (currentOrderTime % 30);
    }

    return currentOrderTime;
}

void Simulator::listAll() {
    cout << "\n====================================" << endl;
    cout << "         전체 목록 조회" << endl;
    cout << "====================================" << endl;

    listOrderers();
    cout << endl;
    listDrivers();
    cout << endl;
    listStores();
    cout << endl;
    listOrders();
}

void Simulator::listOrderers() {
    if (orderers.empty()) {
        cout << "등록된 주문자가 없습니다." << endl;
        return;
    }

    cout << "[주문자 목록 (총 " << orderers.size() << "명)]" << endl;
    printSeparator();
    for (const Orderer& orderer : orderers) {
        cout << "ID: " << orderer.getId()
             << ", 이름: " << orderer.getName()
             << ", 위치: (" << orderer.getLocation().getX() << ", "
             << orderer.getLocation().getY() << ")" << endl;
    }
    printSeparator();
}

void Simulator::listDrivers() {
    if (drivers.empty()) {
        cout << "등록된 기사가 없습니다." << endl;
        return;
    }

    cout << "[기사 목록 (총 " << drivers.size() << "명)]" << endl;
    printSeparator();
    for (const Driver& driver : drivers) {
        cout << "ID: " << driver.getId()
             << ", 이름: " << driver.getName()
             << ", 위치: (" << driver.getCurrentLocation().getX() << ", "
             << driver.getCurrentLocation().getY() << ")" << endl;
    }
    printSeparator();
}

void Simulator::listStores() {
    if (stores.empty()) {
        cout << "등록된 매장이 없습니다." << endl;
        return;
    }

    cout << "[매장 목록 (총 " << stores.size() << "개)]" << endl;
    printSeparator();
    for (const Store& store : stores) {
        cout << "ID: " << store.getId()
             << ", 이름: " << store.getName()
             << ", 위치: (" << store.getLocation().getX() << ", "
             << store.getLocation().getY() << ")"
             << ", 배달 단가: " << (int)store.getFeePerDistance() << "원/거리" << endl;
    }
    printSeparator();
}

void Simulator::listOrders() {
    if (scheduledOrders.empty()) {
        cout << "등록된 주문이 없습니다." << endl;
        return;
    }

    cout << "[주문 목록 (총 " << scheduledOrders.size() << "건)]" << endl;
    printSeparator();
    for (const OrderSchedule& orderSchedule : scheduledOrders) {
        const Order* order = orderSchedule.order;
        cout << "주문 ID: " << order->getOrderId()
             << ", 주문자 ID: " << order->getOrdererId()
             << ", 매장 ID: " << order->getStoreId()
             << ", 배달 위치: (" << order->getDeliveryLocation().getX() << ", "
             << order->getDeliveryLocation().getY() << ")"
             << ", 주문 시간: " << orderSchedule.orderTime << "초"
             << ", 배달비: " << (int)order->getDeliveryFee() << "원" << endl;
    }
    printSeparator();
}



