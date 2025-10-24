#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include "location.h"

using namespace std;

enum ItemType {
    ORDERER,
    DRIVER,
    STORE
};

class MapItem {
public:
    MapItem(const Location& location, ItemType itemType, int id);
    ~MapItem();

    // Getters
    Location getLocation() const;
    ItemType getItemType() const;
    int getId() const;

    // Setters
    void setLocation(const Location& newLocation);

private:
    Location location;
    ItemType itemType;
    int id;
};

class Map {
public:
    Map(int width, int height);
    ~Map();

    void addItem(MapItem& item);
    void addLocation(Location& pos);
    vector<MapItem> getAllItems() const;

    // Getters
    int getWidth() const;
    int getHeight() const;


    //map을 세팅할떄 주의점, MapItem을 전부 추가한뒤에 SetMap해야함 
    //
    //arr는 MapItem끼리 연결되있는지의 여부를 1과0의 정보로 받음 
    //예시) arr[1][3]=0 이면 items[3] 에서 items[1]로 가는 길은 없다는 의미 이다.
    //반대로 arr[3][1]=1 이면 items[1] 에서 items[3]로 가는 길은 있다는 의미 이다.
    void SetMap(int** arr);
    int GetMap_pos(int crt, int trg); //currentPos 에서 targetPos까지의 직접적인 거리. 길이없으면 -1 반환

    Location find_route(const Location& crt, const Location& trg); //crt에 위치했을때 trg로 가려면 어느 노드로 가야하는지 반환
private:
    int width;
    int height;

    vector<MapItem> items;

    vector<Location> nodes;

    int** map_pos;  //map의 연결정보를 담은 행렬그래프.   map[1][3]=5 이면 items[3] 에서 items[1]로 가는 길은 5의 시간을 소모한다는 의미이다.
    int** map_cost;   //items[j]에서 출발해서 items[i]로 도착하기위한 최단시간을 map_cost[i][j]로 저장한 행렬
    void loop_cost(vector<int> check, int* temp, int node);
};

MapItem::MapItem(const Location& location, ItemType itemType, int id)   // 맵 아이템 초기화 작업
    : location(location), itemType(itemType), id(id) {}

MapItem::~MapItem() {                                                 // 맵 아이템 소멸자

}

Location MapItem::getLocation() const {                                 // 위치 정보 반환
    return location;
}

ItemType MapItem::getItemType() const {
    return itemType;
}                                                                       // 아이템 타입 반환

int MapItem::getId() const {
    return id;
}                                                                       // 아이템 ID 반환

void MapItem::setLocation(const Location& newLocation) {                // 위치 정보 업데이트
    location = newLocation;
}

Map::Map(int width, int height) : width(width), height(height) {}       // 맵 초기화 작업 

Map::~Map() {                                                          // 맵 소멸자
    for (int i = 0;i < nodes..size();i++) {
        delete[] map_pos[i];
        delete[] map_cost[i];
    }

    delete[] map_pos;
    delete[] map_cost;

}

void Map::addItem(MapItem& item) {                                // 맵 아이템 추가
    items.push_back(item);
}

void Map::addLocation(Location& pos) {                                // 맵 아이템 추가
    nodes.push_back(pos);
    pos.node = node.size() - 1;
}

vector<MapItem> Map::getAllItems() const {                              // 모든 맵 아이템 반환
    return items;
}

int Map::getWidth() const {                                             // 맵의 너비 반환
    return width;
}

int Map::getHeight() const {                                            // 맵의 높이 반환
    return height;
}

void Map::SetMap(int** arr) {
    map_pos = arr;
    for (int i = 0;i < nodes.size();i++) {
        for (int j = 0;j < nodes.size();j++) {
            if (map_pos[i][j] == 1) {
                map_pos[i][j] = nodes.calculateDistance(nodes[j]); //i와 j사이의 거리는 추후에 임의로 수정가능한 코드
            }
            else if (map_pos[i][j] == 0) {
                map_pos[i][j] = -1;   //inf 취급
            }
        }
    }

    map_cost = new int* [nodes.size()];
    for (int i = 0; i < nodes.size(); i++)
    {
        map_cost[i] = new int[nodes.size()];
    }

    for (int j = 0; j < nodes.size(); j++)
    {
        int* temp = new int[nodes.size()];
        for (int i = 0; i < nodes.size(); i++)
        {
            temp[i] = INT_MAX;
        }
        temp[j] = 0;

        vector<int> v;
        loop_cost(v, temp, j);

        for (int i = 0; i < nodes.size(); i++)
        {
            map_cost[j][i] = temp[i];
        }

        delete temp;
    }
}

int Map::GetMap_pos(int crt, int trg) {
    return map_pos[trg][crt];
}

void Map::loop_cost(vector<int> check, int* temp, int node) {  //map_cost를 세팅하기위한 재귀함수
    check.push_back(node);


    int* crr = new int[nodes.size()];
    for (int i = 0; i < nodes.size(); i++)
    {
        crr[i] = 0;
    }

    for (int i = 0; i < check.capacity(); i++)
    {
        crr[check.at(i)] = 1;
    }

    for (int i = 0; i < nodes.size(); i++)
    {
        if (crr[i] == 1 || map_pos[node][i] < 0)continue;


        int cost = temp[node] + map_pos[node][i];
        if (cost < temp[i]) {
            temp[i] = cost;
            loop_cost(check, temp, i);
        }
    }

    delete crr;
}

Location  Map::find_route(const Location& crt, const Location& trg) {
    int crtPos = crt.node;
    int trgPos = trg.node;

    int min = INT_MAX;
    int result = -1;

    for (int i = 0; i < nodes.size(); i++)
    {
        if (map_pos[i][crtPos] <= 0) continue;

        int cost = map_pos[i][crtPos] + map_cost[trgPos][i];
        if (cost < min) {
            min = cost;
            result = i;
        }
    }

    return nodes[result];
}


#endif
