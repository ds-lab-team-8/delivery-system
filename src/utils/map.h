#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include "location.h"

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

    void addItem(const MapItem& item);
    void addLocation(Location& pos);
    vector<MapItem> getAllItems() const;

    double calculateShortestDistance(const Location& from, const Location& to) const;   // 최단 거리 계산 (다익스트라)
    vector<Location> getShortestPath(const Location& from, const Location& to) const;

    void addEdge(const Location& from, const Location& to, double weight);              // 그래프 간선 추가
    
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

    double** map_pos;  //map의 연결정보를 담은 행렬그래프.   map[1][3]=5 이면 items[3] 에서 items[1]로 가는 길은 5의 시간을 소모한다는 의미이다.
    double** map_cost;   //items[j]에서 출발해서 items[i]로 도착하기위한 최단시간을 map_cost[i][j]로 저장한 행렬

    vector<Location> nodes;

private:
    int width;
    int height;
    vector<MapItem> items;
    // map<Location, vector<pair<Location, double>>> adjacencyList;     
    
    void loop_cost(vector<int> check, double* temp, int node);// 그래프 표현: 인접 리스트
};

#endif