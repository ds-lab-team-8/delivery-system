# 배달 시스템 클래스 다이어그램

## Mermaid 클래스 다이어그램

```mermaid
classDiagram
    %% Core System Classes
    class Simulator {
        -DeliverySystem* deliverySystem
        -Map* map
        -bool simulationRunning
        -int simulationStep
        +initializeSystem()
        +startSimulation()
        +runSimulationStep()
        +addOrderer(id, name, x, y)
        +addStore(id, name, x, y)
        +addDriver(id, name, x, y)
        +processUserCommand()
    }

    class DeliverySystem {
        -Map map
        -vector~Orderer~ orderers
        -vector~Driver~ drivers
        -vector~Store~ stores
        -vector~Order*~ orders
        -queue~Order*~ ordersWaitingStore
        -queue~Order*~ ordersWaitingDriver
        -int nextOrderId
        +addOrderer(orderer)
        +addStore(store)
        +addDriver(driver)
        +processNewOrder(ordererId, storeId, orderDetails)
        +findOptimalDriver(order)
        +optimizeMultipleDeliveries(driverId)
        +calculateDeliveryDistance(store, customer, driverPos)
    }

    %% Entity Classes
    class Orderer {
        -int id
        -string name
        -Location address
        +getId()
        +getName()
        +getAddress()
        +placeOrder(system, storeId)
    }

    class Store {
        -int id
        -string name
        -Location location
        -queue~Order*~ orderQueue
        +getId()
        +getName()
        +getLocation()
        +receiveOrder(order)
        +processNextOrder()
        +requestDriverCall(system)
        +hasOrdersWaiting()
    }

    class Driver {
        -int id
        -string name
        -Location currentLocation
        -queue~Order*~ orderQueue
        -bool available
        +getId()
        +getName()
        +getCurrentLocation()
        +updateLocation(newLocation)
        +receiveOrder(order)
        +acceptOrder(orderId, system)
        +completeDelivery(orderId)
        +hasOrdersInQueue()
    }

    class Order {
        -int orderId
        -int ordererId
        -int storeId
        -int driverId
        -Location deliveryAddress
        -OrderStatus status
        +getOrderId()
        +getOrdererId()
        +getStoreId()
        +getDriverId()
        +getDeliveryAddress()
        +getStatus()
        +acceptOrder()
        +assignDriver(driverId)
        +completePickup()
        +completeDelivery()
        +isCompleted()
    }

    %% Utility Classes
    class Location {
        +int x
        +int y
        +getX()
        +getY()
        +setX(x)
        +setY(y)
        +setLocation(x, y)
        +displayLocation()
        +operator==(other)
        +operator!=(other)
    }

    class Map {
        -int width
        -int height
        -vector~Location~ personLocations
        +initializeMap()
        +displayMap()
        +updatePersonLocation(personId, location)
        +calculateDistance(a, b)
        +isValidLocation(location)
        +getWidth()
        +getHeight()
    }

    %% Enums
    class OrderStatus {
        <<enumeration>>
        ORDER_REQUESTED
        ORDER_ACCEPTED
        DRIVER_CALL_ACCEPTED
        PICKUP_COMPLETE
        DELIVERY_COMPLETE
    }

    class SimulationCommand {
        <<enumeration>>
        ADD_ORDERER
        ADD_STORE
        ADD_DRIVER
        PLACE_ORDER
        VIEW_STATUS
        VIEW_DRIVER_STATUS
        VIEW_STORE_STATUS
        RUN_SIMULATION_STEP
        EXIT_SIMULATION
    }

    %% Relationships
    Simulator "1" *-- "1" DeliverySystem : contains
    Simulator "1" *-- "1" Map : contains

    DeliverySystem "1" *-- "1" Map : contains
    DeliverySystem "1" *-- "*" Orderer : manages
    DeliverySystem "1" *-- "*" Store : manages
    DeliverySystem "1" *-- "*" Driver : manages
    DeliverySystem "1" *-- "*" Order : manages

    Orderer "1" *-- "1" Location : has address
    Store "1" *-- "1" Location : has location
    Store "1" o-- "*" Order : queues
    Driver "1" *-- "1" Location : has current location
    Driver "1" o-- "*" Order : delivers
    Order "1" *-- "1" Location : has delivery address
    Order "1" -- "1" OrderStatus : has status

    Map "1" *-- "*" Location : tracks positions

    %% Dependencies
    Orderer ..> DeliverySystem : uses
    Store ..> DeliverySystem : uses
    Driver ..> DeliverySystem : uses
    Simulator ..> SimulationCommand : uses
```

## 클래스 관계 설명

### 핵심 시스템 클래스
- **Simulator**: 전체 시뮬레이션을 관리하는 메인 클래스
- **DeliverySystem**: 배달 시스템의 핵심 로직을 담당하는 클래스

### 엔티티 클래스
- **Orderer**: 주문자를 나타내는 클래스
- **Store**: 상점을 나타내는 클래스
- **Driver**: 배달 기사를 나타내는 클래스
- **Order**: 주문을 나타내는 클래스

### 유틸리티 클래스
- **Location**: 2D 좌표를 나타내는 클래스
- **Map**: 지도 관리 및 거리 계산을 담당하는 클래스

### 열거형
- **OrderStatus**: 주문 상태를 나타내는 열거형
- **SimulationCommand**: 시뮬레이션 명령을 나타내는 열거형

### 주요 관계
1. **Composition (집합)**: Simulator는 DeliverySystem과 Map을 포함
2. **Aggregation (연관)**: Store와 Driver는 Order 큐를 가짐
3. **Association (연관)**: 각 엔티티는 Location을 가짐
4. **Dependency (의존)**: 엔티티들은 DeliverySystem을 사용