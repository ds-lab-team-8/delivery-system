#include "simulator.h"
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

Simulator::Simulator() : useRealImplementation(false), 
                         nextOrdererId(1), nextDriverId(1), 
                         nextStoreId(1), nextOrderId(1) {}

Simulator::~Simulator() {
    // 동적 할당된 Order 객체들 삭제
    for (Order* order : orders) {
        delete order;
    }
}

void Simulator::simulateWithUserInput() {
    printHeader();

    // 현재 모드 출력
    cout << "\n[실행 모드: " << (useRealImplementation ? "실제 구현 사용" : "시뮬레이터 독립 실행")
         << "]" << endl;
    if (!useRealImplementation) {
        cout << "[참고] 현재는 Mock 모드로 실행됩니다." << endl;
        cout << "       실제 DeliverySystem 구현이 완료되면 useRealImplementation을 true로 변경하세요." << endl;
    }

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

            if (useRealImplementation) {
                deliverySystem.addOrderer(orderer);
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

            if (useRealImplementation) {
                deliverySystem.addDriver(driver);
            }

            drivers.push_back(driver);

            cout << "[기사 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_store") {
            int x, y;
            string name;
            iss >> name >> x >> y;

            int id = nextStoreId++;
            Location loc(x, y);
            Store store(id, name, loc);

            if (useRealImplementation) {
                deliverySystem.addStore(store);
            }

            stores.push_back(store);

            cout << "[매장 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_order") {
            int ordererId, storeId, x, y;
            iss >> ordererId >> storeId >> x >> y;

            int orderId = nextOrderId++;
            Location deliveryLoc(x, y);
            Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

            if (useRealImplementation) {
                deliverySystem.addOrder(*order);
            }

            orders.push_back(order);

            cout << "[주문 추가] 주문 ID: " << orderId
                 << ", 주문자 ID: " << ordererId
                 << ", 매장 ID: " << storeId
                 << ", 배달 위치: (" << x << ", " << y << ")" << endl;

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

                if (useRealImplementation) {
                    deliverySystem.addOrderer(orderer);
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

                if (useRealImplementation) {
                    deliverySystem.addDriver(driver);
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
            for (int i = 0; i < n; i++) {
                string name = generateRandomName("Store");
                int x = generateRandomCoordinate(0, 100);
                int y = generateRandomCoordinate(0, 100);
                int id = nextStoreId++;

                Location loc(x, y);
                Store store(id, name, loc);

                if (useRealImplementation) {
                    deliverySystem.addStore(store);
                }

                stores.push_back(store);
                cout << "  ID: " << id << ", 이름: " << name 
                     << ", 위치: (" << x << ", " << y << ")" << endl;
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

                Location deliveryLoc(x, y);
                Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

                if (useRealImplementation) {
                    deliverySystem.addOrder(*order);
                }

                orders.push_back(order);
                cout << "  주문 ID: " << orderId 
                     << ", 주문자 ID: " << ordererId
                     << ", 매장 ID: " << storeId
                     << ", 배달 위치: (" << x << ", " << y << ")" << endl;
            }

        } else if (cmd == "add_all_random") {
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

                if (useRealImplementation) {
                    deliverySystem.addOrderer(orderer);
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

                if (useRealImplementation) {
                    deliverySystem.addDriver(driver);
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
                Store store(id, name, loc);

                if (useRealImplementation) {
                    deliverySystem.addStore(store);
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

                if (useRealImplementation) {
                    deliverySystem.addOrder(*order);
                }

                orders.push_back(order);
                cout << "  주문 ID: " << orderId 
                     << ", 주문자 ID: " << ordererId
                     << ", 매장 ID: " << storeId
                     << ", 배달 위치: (" << x << ", " << y << ")" << endl;
            }

            cout << "\n[완료] 주문자 " << nOrderers << "명, 기사 " << nDrivers 
                 << "명, 매장 " << nStores << "개, 주문 " << nOrders << "건이 추가되었습니다." << endl;
            printSeparator();

        } else if (cmd == "list") {
            listAll();

        } else if (cmd == "start") {
            cout << "\n시뮬레이션을 시작합니다...\n" << endl;
            runSimulation();

        } else if (cmd == "quit" || cmd == "exit") {
            cout << "시뮬레이터를 종료합니다." << endl;
            running = false;

        } else if (cmd == "help") {
            printHelp();

        } else {
            cout << "알 수 없는 명령어입니다. 'help'를 입력하여 사용 가능한 명령어를 확인하세요." << endl;
        }
    }
}

void Simulator::runSimulation() {
    if (orders.empty()) {
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
    cout << "시뮬레이션 진행" << endl;
    printSeparator();

    const double SPEED = 1.0;

    // 통계 데이터 초기화
    map<int, DriverStats> driverStats;
    map<int, OrderStats> orderStats;
    map<int, Location> driverLocations;
    map<int, double> driverAvailableTime;

    for (const Driver& driver : drivers) {
        driverStats[driver.getId()] = DriverStats();
        driverLocations[driver.getId()] = driver.getCurrentLocation();
        driverAvailableTime[driver.getId()] = 0.0;
    }

    // 배차 요청
    if (useRealImplementation) {
        deliverySystem.requestCallsToDrivers();
    }

    cout << "\n[시간: 0.0초] 모든 주문이 기사들에게 배차 요청되었습니다.\n" << endl;

    // 이벤트 큐 생성
    vector<Event> events;
    map<int, Store*> storeMap;
    map<int, string> driverNames;

    // 매장 맵 생성
    for (Store& store : stores) {
        storeMap[store.getId()] = &store;
    }

    // 기사 이름 맵 생성
    for (const Driver& driver : drivers) {
        driverNames[driver.getId()] = driver.getName();
    }

    // 시간 0에 모든 주문에 대해 가장 빠르게 처리 가능한 기사 배정
    for (Order* order : orders) {
        Store* targetStore = storeMap[order->getStoreId()];
        if (!targetStore) continue;

        Location storeLocation = targetStore->getLocation();
        
        // 가장 빠르게 도착 가능한 기사 찾기
        int bestDriverId = -1;
        double earliestArrival = -1;

        for (const auto& pair : driverAvailableTime) {
            int driverId = pair.first;
            double availableTime = pair.second;
            Location driverLoc = driverLocations[driverId];
            double distance = driverLoc.calculateDistance(storeLocation);
            double arrivalTime = availableTime + distance / SPEED;

            if (earliestArrival < 0 || arrivalTime < earliestArrival) {
                earliestArrival = arrivalTime;
                bestDriverId = driverId;
            }
        }

        if (bestDriverId == -1) continue;

        // 기사 배정
        order->assignDriver(bestDriverId);
        if (useRealImplementation) {
            deliverySystem.acceptCall(order->getOrderId());
        }

        // 픽업 거리 및 시간 계산
        Location currentDriverLoc = driverLocations[bestDriverId];
        double pickupDistance = currentDriverLoc.calculateDistance(storeLocation);
        double pickupTime = pickupDistance / SPEED;
        double pickupCompleteTime = driverAvailableTime[bestDriverId] + pickupTime;

        // 배달 거리 및 시간 계산
        Location deliveryLocation = order->getDeliveryLocation();
        double deliveryDistance = storeLocation.calculateDistance(deliveryLocation);
        double deliveryTime = deliveryDistance / SPEED;
        double deliveryCompleteTime = pickupCompleteTime + deliveryTime;

        // 이벤트 생성
        events.push_back(Event(driverAvailableTime[bestDriverId], EVENT_ORDER_ASSIGNED, 
                              order->getOrderId(), bestDriverId, currentDriverLoc));
        events.push_back(Event(pickupCompleteTime, EVENT_PICKUP_COMPLETE, 
                              order->getOrderId(), bestDriverId, storeLocation, pickupDistance));
        events.push_back(Event(deliveryCompleteTime, EVENT_DELIVERY_COMPLETE, 
                              order->getOrderId(), bestDriverId, deliveryLocation, deliveryDistance));

        // 기사의 다음 가용 시간 업데이트
        driverAvailableTime[bestDriverId] = deliveryCompleteTime;
        driverLocations[bestDriverId] = deliveryLocation;

        // 통계 데이터 준비
        OrderStats oStats;
        oStats.orderId = order->getOrderId();
        oStats.driverId = bestDriverId;
        oStats.pickupDistance = pickupDistance;
        oStats.deliveryDistance = deliveryDistance;
        oStats.totalTime = pickupTime + deliveryTime;
        orderStats[order->getOrderId()] = oStats;

        driverStats[bestDriverId].deliveryCount++;
        driverStats[bestDriverId].totalDistance += (pickupDistance + deliveryDistance);
        driverStats[bestDriverId].completedOrders.push_back(order->getOrderId());
    }

    // 이벤트를 시간순으로 정렬
    sort(events.begin(), events.end());

    // 이벤트 처리 및 출력
    for (const Event& event : events) {
        Order* currentOrder = nullptr;
        
        switch (event.type) {
            case EVENT_ORDER_ASSIGNED:
                cout << "[시간: " << fixed << setprecision(1) << event.time
                     << "초] 기사 #" << event.driverId << " (" << driverNames[event.driverId]
                     << ")이(가) 주문 #" << event.orderId << "을(를) 수락했습니다." << endl;
                break;

            case EVENT_PICKUP_COMPLETE:
                for (Order* o : orders) {
                    if (o->getOrderId() == event.orderId) {
                        currentOrder = o;
                        break;
                    }
                }
                if (currentOrder) {
                    currentOrder->completePickup();
                    if (useRealImplementation) {
                        deliverySystem.completePickup(event.orderId);
                    }
                    Store* store = storeMap[currentOrder->getStoreId()];
                    cout << "[시간: " << fixed << setprecision(1) << event.time
                         << "초] 기사 #" << event.driverId << "이(가) 매장 #" << store->getId()
                         << " (" << store->getName() << ")에서 주문 #" << event.orderId
                         << " 픽업 완료 (거리: " << fixed << setprecision(2) << event.distance << ")" << endl;
                }
                break;

            case EVENT_DELIVERY_COMPLETE:
                for (Order* o : orders) {
                    if (o->getOrderId() == event.orderId) {
                        currentOrder = o;
                        break;
                    }
                }
                if (currentOrder) {
                    currentOrder->completeDelivery();
                    if (useRealImplementation) {
                        deliverySystem.completeDelivery(event.orderId);
                    }
                    cout << "[시간: " << fixed << setprecision(1) << event.time
                         << "초] 기사 #" << event.driverId << "이(가) 주문 #" << event.orderId
                         << " 배달 완료! (거리: " << fixed << setprecision(2) << event.distance << ")" << endl;
                }
                break;
        }
    }

    double totalTime = events.empty() ? 0.0 : events.back().time;
    cout << "\n모든 주문이 처리되었습니다!\n" << endl;

    // 결과 출력
    printSimulationResults(driverStats, orderStats, totalTime);
}

void Simulator::printSimulationResults(const map<int, DriverStats>& driverStats,
                                       const map<int, OrderStats>& orderStats,
                                       double totalTime) {
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

        double totalDistance = 0.0;
        int totalDeliveries = 0;

        for (const auto& pair : driverStats) {
            int driverId = pair.first;
            const DriverStats& stats = pair.second;

            // 기사 이름 찾기
            string driverName;
            for (const Driver& driver : drivers) {
                if (driver.getId() == driverId) {
                    driverName = driver.getName();
                    break;
                }
            }

            cout << "기사 #" << driverId << " (" << driverName << ")" << endl;
            cout << "  - 배달 완료: " << stats.deliveryCount << "건" << endl;
            cout << "  - 총 이동 거리: " << fixed << setprecision(2)
                 << stats.totalDistance << endl;

            if (stats.deliveryCount > 0) {
                cout << "  - 평균 이동 거리: " << fixed << setprecision(2)
                     << (stats.totalDistance / stats.deliveryCount) << endl;
            }
            cout << endl;

            totalDistance += stats.totalDistance;
            totalDeliveries += stats.deliveryCount;
        }

        // 전체 평균 통계
        if (totalDeliveries > 0) {
            cout << "전체 평균 통계:" << endl;
            printSeparator();
            cout << "평균 이동 거리: " << fixed << setprecision(2)
                 << (totalDistance / totalDeliveries) << endl;

            if (totalTime > 0) {
                cout << "평균 배달 시간: " << fixed << setprecision(1)
                     << (totalTime / totalDeliveries) << "초" << endl;
            }
        }
    } else {
        cout << "시뮬레이션 데이터가 없습니다." << endl;
    }

    // 결과 파일 저장
    saveResultsToFile(driverStats, orderStats, totalTime);
    printSeparator();
}

void Simulator::saveResultsToFile(const map<int, DriverStats>& driverStats,
                                  const map<int, OrderStats>& orderStats,
                                  double totalTime) {
    // out 디렉토리 생성
    mkdir("out", 0755);
    
    // 현재 시간으로 타임스탬프 생성
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    
    ostringstream filename;
    filename << "out/simulation_result_"
             << (1900 + ltm->tm_year)
             << setfill('0') << setw(2) << (1 + ltm->tm_mon)
             << setfill('0') << setw(2) << ltm->tm_mday << "_"
             << setfill('0') << setw(2) << ltm->tm_hour
             << setfill('0') << setw(2) << ltm->tm_min
             << setfill('0') << setw(2) << ltm->tm_sec
             << ".txt";
    
    ofstream outFile(filename.str());

    if (!outFile.is_open()) {
        cout << "결과 파일 저장에 실패했습니다." << endl;
        return;
    }

    outFile << "====================================\n";
    outFile << "    배달 시스템 시뮬레이션 결과\n";
    outFile << "====================================\n\n";

    // 기본 통계
    outFile << "총 주문 수: " << orderStats.size() << "건\n";
    outFile << "완료된 주문 수: " << orderStats.size() << "건\n";
    outFile << "전체 소요 시간: " << fixed << setprecision(1) << totalTime << "초\n\n";

    // 기사별 통계
    if (!driverStats.empty()) {
        outFile << "기사별 통계:\n";
        outFile << "------------------------------------\n";

        double totalDistance = 0.0;
        int totalDeliveries = 0;

        for (const auto& pair : driverStats) {
            const DriverStats& stats = pair.second;
            outFile << "기사 ID: " << pair.first << "\n";
            outFile << "  - 배달 완료: " << stats.deliveryCount << "건\n";
            outFile << "  - 총 이동 거리: " << fixed << setprecision(2)
                    << stats.totalDistance << "km\n";

            if (stats.deliveryCount > 0) {
                outFile << "  - 평균 이동 거리: " << fixed << setprecision(2)
                        << (stats.totalDistance / stats.deliveryCount) << "km\n";
            }
            outFile << "\n";

            totalDistance += stats.totalDistance;
            totalDeliveries += stats.deliveryCount;
        }

        // 전체 평균 통계
        if (totalDeliveries > 0) {
            outFile << "전체 평균 통계:\n";
            outFile << "------------------------------------\n";
            outFile << "평균 이동 거리: " << fixed << setprecision(2)
                    << (totalDistance / totalDeliveries) << "km\n";

            if (totalTime > 0) {
                outFile << "평균 배달 시간: " << fixed << setprecision(1)
                        << (totalTime / totalDeliveries) << "초\n";
            }
        }
    }

    // 주문별 상세 정보
    if (!orderStats.empty()) {
        outFile << "\n주문별 상세 정보:\n";
        outFile << "------------------------------------\n";

        for (const auto& pair : orderStats) {
            const OrderStats& stats = pair.second;
            outFile << "주문 ID: " << stats.orderId << "\n";
            outFile << "  - 담당 기사 ID: " << stats.driverId << "\n";
            outFile << "  - 픽업 거리: " << fixed << setprecision(2)
                    << stats.pickupDistance << "km\n";
            outFile << "  - 배달 거리: " << fixed << setprecision(2)
                    << stats.deliveryDistance << "km\n";
            outFile << "  - 총 처리 시간: " << fixed << setprecision(1)
                    << stats.totalTime << "초\n\n";
        }
    }

    outFile << "====================================\n";
    outFile.close();
    
    cout << "\n결과가 '" << filename.str() << "'에 저장되었습니다." << endl;
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
    cout << "  add_store <name> <x> <y>" << endl;
    cout << "    - 매장을 추가합니다. (ID는 자동 생성)" << endl;
    cout << "  add_order <ordererId> <storeId> <deliveryX> <deliveryY>" << endl;
    cout << "    - 주문을 추가합니다. (주문 ID는 자동 생성)" << endl;
    cout << "  add_orderer_random <n>" << endl;
    cout << "    - n개의 주문자를 랜덤으로 추가합니다." << endl;
    cout << "  add_driver_random <n>" << endl;
    cout << "    - n개의 기사를 랜덤으로 추가합니다." << endl;
    cout << "  add_store_random <n>" << endl;
    cout << "    - n개의 매장을 랜덤으로 추가합니다." << endl;
    cout << "  add_order_random <n>" << endl;
    cout << "    - n개의 주문을 랜덤으로 추가합니다." << endl;
    cout << "  add_all_random <nOrderers> <nDrivers> <nStores> <nOrders>" << endl;
    cout << "    - 주문자, 기사, 매장, 주문을 한 번에 랜덤으로 추가합니다." << endl;
    cout << "  list" << endl;
    cout << "    - 모든 주문자, 기사, 매장, 주문을 조회합니다." << endl;
    cout << "  start" << endl;
    cout << "    - 시뮬레이션을 시작합니다." << endl;
    cout << "  help" << endl;
    cout << "    - 도움말을 출력합니다." << endl;
    cout << "  quit / exit" << endl;
    cout << "    - 시뮬레이터를 종료합니다." << endl;
    printSeparator();
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
             << store.getLocation().getY() << ")" << endl;
    }
    printSeparator();
}

void Simulator::listOrders() {
    if (orders.empty()) {
        cout << "등록된 주문이 없습니다." << endl;
        return;
    }
    
    cout << "[주문 목록 (총 " << orders.size() << "건)]" << endl;
    printSeparator();
    for (const Order* order : orders) {
        cout << "주문 ID: " << order->getOrderId()
             << ", 주문자 ID: " << order->getOrdererId()
             << ", 매장 ID: " << order->getStoreId()
             << ", 배달 위치: (" << order->getDeliveryLocation().getX() << ", " 
             << order->getDeliveryLocation().getY() << ")" << endl;
    }
    printSeparator();
}