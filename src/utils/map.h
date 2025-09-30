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

    Location getLocation() const;
    ItemType getItemType() const;
    int getId() const;
    
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

    // Getters
    int getWidth() const;
    int getHeight() const;

private:
    int width;
    int height;
    vector<MapItem> items;

    // 추가로 그래프 가중치 정보 및 최단 거리 정보 등이 필요한 경우 선언하여 사용

};

#endif