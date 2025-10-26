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

#endif