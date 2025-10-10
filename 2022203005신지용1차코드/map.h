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

    void addItem(const MapItem& item);
    vector<MapItem> getAllItems() const;

    double calculateShortestDistance(const Location& from, const Location& to) const;   // 최단 거리 계산 (다익스트라)
    vector<Location> getShortestPath(const Location& from, const Location& to) const;

    void addEdge(const Location& from, const Location& to, double weight);              // 그래프 간선 추가
    
    // Getters
    int getWidth() const;
    int getHeight() const;

private:
    int width;
    int height;
    vector<MapItem> items;
    map<Location, vector<pair<Location, double>>> adjacencyList;                        // 그래프 표현: 인접 리스트
};

MapItem::MapItem(const Location& location, ItemType itemType, int id)   // 맵 아이템 초기화 작업
	: location(location), itemType(itemType), id(id) {}

MapItem::~MapItem() {}                                                  // 맵 아이템 소멸자

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

Map::Map(int width, int height) : width(width), height(height) {}       // 맵 초기화 작업 (예: 그래프 초기화 등)

Map::~Map() {}                                                          // 맵 소멸자

void Map::addItem(const MapItem& item) {                                // 맵 아이템 추가
    items.push_back(item);
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

void Map::addEdge(const Location& from, const Location& to, double weight) {                // 그래프 간선 추가
    adjacencyList[from].push_back(make_pair(to, weight));
    adjacencyList[to].push_back(make_pair(from, weight));                                   // 무방향 그래프이므로 양방향 간선 추가
}

void Map::calculateShortestDistance(const Location& from, const Location& to) const {       // 다익스트라 알고리즘을 이용한 최단 거리 계산
    // 다익스트라 알고리즘 구현 위치
    return 0.0;                     // 임시 반환값
}

vector<Location> Map::getShortestPath(const Location& from, const Location& to) const {     // 다익스트라 알고리즘을 이용한 최단 경로 반환
    // 다익스트라 알고리즘 구현 위치
    return vector<Location>();      // 임시 반환값
}

#endif