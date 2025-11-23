#include "map.h"
#include <climits>

using namespace std;

MapItem::MapItem(const Location& location, ItemType itemType, int id)   // �� ������ �ʱ�ȭ �۾�
    : location(location), itemType(itemType), id(id) {
}

MapItem::~MapItem() {}                                                  // �� ������ �Ҹ���

Location MapItem::getLocation() const {                                 // ��ġ ���� ��ȯ
    return location;
}

ItemType MapItem::getItemType() const {
    return itemType;
}                                                                       // ������ Ÿ�� ��ȯ

int MapItem::getId() const {
    return id;
}                                                                       // ������ ID ��ȯ

void MapItem::setLocation(const Location& newLocation) {                // ��ġ ���� ������Ʈ
    location = newLocation;
}

Map::Map(int width, int height) : width(width), height(height), map_pos(nullptr), map_cost(nullptr) {}       // �� �ʱ�ȭ �۾� (��: �׷��� �ʱ�ȭ ��)

Map::~Map() {                                                          // 맵 소멸자
    if (map_pos != nullptr) {
        for (int i = 0; i < (int)nodes.size(); i++) {
            delete[] map_pos[i];
        }
        delete[] map_pos;
    }
    
    if (map_cost != nullptr) {
        for (int i = 0; i < (int)nodes.size(); i++) {
            delete[] map_cost[i];
        }
        delete[] map_cost;
    }
}

void Map::addItem(const MapItem& item) {                                // �� ������ �߰�
    items.push_back(item);
}

void Map::addLocation(Location& pos) {                                // 맵 아이템 추가
    nodes.push_back(pos);
    pos.node = nodes.size() - 1;
}

vector<MapItem> Map::getAllItems() const {                              // ��� �� ������ ��ȯ
    return items;
}

int Map::getWidth() const {                                             // ���� �ʺ� ��ȯ
    return width;
}

int Map::getHeight() const {                                            // ���� ���� ��ȯ
    return height;
}

void Map::SetMap(int** arr) {
    map_pos = new double*[nodes.size()];
    for (int i = 0; i < nodes.size(); i++)
    {
        map_pos[i] = new double[nodes.size()];
    }


    for (int i = 0;i < nodes.size();i++) {
        for (int j = 0;j < nodes.size();j++) {
            if (arr[i][j] == 1) {
                map_pos[i][j] = nodes[i].calculateDistance(nodes[j]); //i와 j사이의 거리는 추후에 임의로 수정가능한 코드
            }
            else if (arr[i][j] == 0) {
                map_pos[i][j] = -1;   //inf 취급
            }
        }
    }

    map_cost = new double* [nodes.size()];
    for (int i = 0; i < nodes.size(); i++)
    {
        map_cost[i] = new double[nodes.size()];
    }

    for (int j = 0; j < nodes.size(); j++)
    {
        double* temp = new double[nodes.size()];
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

        delete[] temp;
    }


    for (int i = 0;i < nodes.size();i++) {
        delete[] arr[i];
    }

    delete[] arr;
}

int Map::GetMap_pos(int crt, int trg) {
    return map_pos[trg][crt];
}

double Map::GetMap_cost(int crt,int trg) const {
    return map_cost[trg][crt];
}

void Map::loop_cost(vector<int> check, double* temp, int node) {  //map_cost를 세팅하기위한 재귀함수
    check.push_back(node);


    int* crr = new int[nodes.size()];
    for (int i = 0; i < nodes.size(); i++)
    {
        crr[i] = 0;
    }

    for (int i = 0; i < (int)check.size(); i++)
    {
        crr[check.at(i)] = 1;
    }

    for (int i = 0; i < nodes.size(); i++)
    {
        if (crr[i] == 1 || map_pos[node][i] < 0)continue;


        double cost = temp[node] + map_pos[node][i];
        if (cost < temp[i]) {
            temp[i] = cost;
            loop_cost(check, temp, i);
        }
    }

    delete[] crr;
}

Location  Map::find_route(const Location& crt, const Location& trg) {
    int crtNode = crt.node;
    int trgNode = trg.node;

    double min = INT_MAX;
    int result = -1;

    for (int i = 0; i < nodes.size(); i++)
    {
        if (map_pos[i][crtNode] <= 0) continue;

        double cost = map_pos[i][crtNode] + map_cost[trgNode][i];
        if (cost < min) {
            min = cost;
            result = i;
        }
    }

    return nodes[result];

}

