#include "simulator.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>
#include <cmath>
#include <algorithm>

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

Simulator::Simulator() : useRealImplementation(false) {
    // useRealImplementation을 false로 초기화 (Mock 모드)
    // 실제 DeliverySystem 구현이 완료되면 true로 변경
}

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
            int id, x, y;
            string name;
            iss >> id >> name >> x >> y;

            Location loc(x, y);
            Orderer orderer(id, name, loc);

            // 실제 구현이 완료되면 사용
            if (useRealImplementation) {
                deliverySystem.addOrderer(orderer);
            }

            orderers.push_back(orderer);

            cout << "[주문자 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_driver") {
            int id, x, y;
            string name;
            iss >> id >> name >> x >> y;

            Location loc(x, y);
            Driver driver(id, name, loc);

            // 실제 구현이 완료되면 사용
            if (useRealImplementation) {
                deliverySystem.addDriver(driver);
            }

            drivers.push_back(driver);

            cout << "[기사 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_store") {
            int id, x, y;
            string name;
            iss >> id >> name >> x >> y;

            Location loc(x, y);
            Store store(id, name, loc);

            // 실제 구현이 완료되면 사용
            if (useRealImplementation) {
                deliverySystem.addStore(store);
            }

            stores.push_back(store);

            cout << "[매장 추가] ID: " << id << ", 이름: " << name
                 << ", 위치: (" << x << ", " << y << ")" << endl;

        } else if (cmd == "add_order") {
            int orderId, ordererId, storeId, x, y;
            iss >> orderId >> ordererId >> storeId >> x >> y;

            Location deliveryLoc(x, y);
            Order* order = new Order(orderId, ordererId, storeId, deliveryLoc);

            // 실제 구현이 완료되면 사용
            if (useRealImplementation) {
                deliverySystem.addOrder(*order);
            }

            orders.push_back(order);

            cout << "[주문 추가] 주문 ID: " << orderId
                 << ", 주문자 ID: " << ordererId
                 << ", 매장 ID: " << storeId
                 << ", 배달 위치: (" << x << ", " << y << ")" << endl;

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

    // 통계 데이터 초기화
    map<int, DriverStats> driverStats;
    map<int, OrderStats> orderStats;

    for (const Driver& driver : drivers) {
        driverStats[driver.getId()] = DriverStats();
    }

    double currentTime = 0.0;
    const double SPEED = 1.0;  // 1 거리 단위당 1초 (시뮬레이션 속도)

    // 배차 요청 (실제 구현이 완료되면 사용)
    if (useRealImplementation) {
        deliverySystem.requestCallsToDrivers();
    }

    cout << "\n[시간: " << fixed << setprecision(1) << currentTime
         << "초] 모든 주문이 기사들에게 배차 요청되었습니다.\n" << endl;

    // 각 기사의 현재 위치 추적
    map<int, Location> driverLocations;
    for (const Driver& driver : drivers) {
        driverLocations[driver.getId()] = driver.getCurrentLocation();
    }

    // 주문별로 시뮬레이션 진행
    for (Order* order : orders) {
        cout << "------------------------------------" << endl;
        cout << "[주문 #" << order->getOrderId() << " 처리 시작]" << endl;

        // 매장 찾기
        Store* targetStore = nullptr;
        for (Store& store : stores) {
            if (store.getId() == order->getStoreId()) {
                targetStore = &store;
                break;
            }
        }

        if (!targetStore) {
            cout << "매장을 찾을 수 없습니다. 주문을 건너뜁니다." << endl;
            continue;
        }

        // 가장 가까운 기사 찾기
        int nearestDriverId = -1;
        double minDistance = -1;
        Location storeLocation = targetStore->getLocation();

        for (const auto& pair : driverLocations) {
            int driverId = pair.first;
            Location driverLoc = pair.second;
            double distance = driverLoc.calculateDistance(storeLocation);

            if (minDistance < 0 || distance < minDistance) {
                minDistance = distance;
                nearestDriverId = driverId;
            }
        }

        if (nearestDriverId == -1) {
            cout << "배차 가능한 기사가 없습니다." << endl;
            continue;
        }

        // 기사 배정
        order->assignDriver(nearestDriverId);

        // 실제 구현이 완료되면 사용
        if (useRealImplementation) {
            deliverySystem.acceptCall(order->getOrderId());
        }

        // 기사 이름 찾기
        string driverName;
        for (const Driver& driver : drivers) {
            if (driver.getId() == nearestDriverId) {
                driverName = driver.getName();
                break;
            }
        }

        cout << "[시간: " << fixed << setprecision(1) << currentTime
             << "초] 기사 #" << nearestDriverId << " (" << driverName
             << ")이(가) 주문을 수락했습니다." << endl;

        // 매장으로 이동
        double pickupDistance = minDistance;
        double pickupTime = pickupDistance / SPEED;
        currentTime += pickupTime;

        cout << "[시간: " << fixed << setprecision(1) << currentTime
             << "초] 기사가 매장 #" << targetStore->getId()
             << " (" << targetStore->getName() << ")에 도착했습니다. (거리: "
             << fixed << setprecision(2) << pickupDistance << ")" << endl;

        // 픽업 완료
        order->completePickup();

        // 실제 구현이 완료되면 사용
        if (useRealImplementation) {
            deliverySystem.completePickup(order->getOrderId());
        }

        driverLocations[nearestDriverId] = storeLocation;

        cout << "[시간: " << fixed << setprecision(1) << currentTime
             << "초] 픽업이 완료되었습니다." << endl;

        // 배달지로 이동
        Location deliveryLocation = order->getDeliveryLocation();
        double deliveryDistance = storeLocation.calculateDistance(deliveryLocation);
        double deliveryTime = deliveryDistance / SPEED;
        currentTime += deliveryTime;

        cout << "[시간: " << fixed << setprecision(1) << currentTime
             << "초] 기사가 배달지에 도착했습니다. (거리: "
             << fixed << setprecision(2) << deliveryDistance << ")" << endl;

        // 배달 완료
        order->completeDelivery();

        // 실제 구현이 완료되면 사용
        if (useRealImplementation) {
            deliverySystem.completeDelivery(order->getOrderId());
        }

        driverLocations[nearestDriverId] = deliveryLocation;

        cout << "[시간: " << fixed << setprecision(1) << currentTime
             << "초] 배달이 완료되었습니다!" << endl;

        // 통계 수집
        driverStats[nearestDriverId].deliveryCount++;
        driverStats[nearestDriverId].totalDistance += (pickupDistance + deliveryDistance);
        driverStats[nearestDriverId].completedOrders.push_back(order->getOrderId());

        OrderStats oStats;
        oStats.orderId = order->getOrderId();
        oStats.driverId = nearestDriverId;
        oStats.pickupDistance = pickupDistance;
        oStats.deliveryDistance = deliveryDistance;
        oStats.totalTime = pickupTime + deliveryTime;
        orderStats[order->getOrderId()] = oStats;
    }

    cout << "\n모든 주문이 처리되었습니다!\n" << endl;

    // 결과 출력
    printSimulationResults(driverStats, orderStats, currentTime);
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

    cout << "\n결과가 'simulation_result.txt'에 저장되었습니다." << endl;
    printSeparator();
}

void Simulator::saveResultsToFile(const map<int, DriverStats>& driverStats,
                                  const map<int, OrderStats>& orderStats,
                                  double totalTime) {
    ofstream outFile("simulation_result.txt");

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
    cout << "  add_orderer <id> <name> <x> <y>" << endl;
    cout << "    - 주문자를 추가합니다." << endl;
    cout << "  add_driver <id> <name> <x> <y>" << endl;
    cout << "    - 기사를 추가합니다." << endl;
    cout << "  add_store <id> <name> <x> <y>" << endl;
    cout << "    - 매장을 추가합니다." << endl;
    cout << "  add_order <orderId> <ordererId> <storeId> <deliveryX> <deliveryY>" << endl;
    cout << "    - 주문을 추가합니다." << endl;
    cout << "  start" << endl;
    cout << "    - 시뮬레이션을 시작합니다." << endl;
    cout << "  help" << endl;
    cout << "    - 도움말을 출력합니다." << endl;
    cout << "  quit / exit" << endl;
    cout << "    - 시뮬레이터를 종료합니다." << endl;
    printSeparator();
}