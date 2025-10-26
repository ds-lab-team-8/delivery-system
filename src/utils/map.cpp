#include "map.h"

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

Map::Map(int width, int height) : width(width), height(height) {}       // �� �ʱ�ȭ �۾� (��: �׷��� �ʱ�ȭ ��)

Map::~Map() {}                                                          // �� �Ҹ���

void Map::addItem(const MapItem& item) {                                // �� ������ �߰�
    items.push_back(item);
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

void Map::addEdge(const Location& from, const Location& to, double weight) {                // �׷��� ���� �߰�
    adjacencyList[from].push_back(make_pair(to, weight));
    adjacencyList[to].push_back(make_pair(from, weight));                                   // ������ �׷����̹Ƿ� ����� ���� �߰�
}

double Map::calculateShortestDistance(const Location& from, const Location& to) const {       // ���ͽ�Ʈ�� �˰����� �̿��� �ִ� �Ÿ� ���
    // ���ͽ�Ʈ�� �˰��� ���� ��ġ
    return 0.0;                     // �ӽ� ��ȯ��
}

vector<Location> Map::getShortestPath(const Location& from, const Location& to) const {     // ���ͽ�Ʈ�� �˰����� �̿��� �ִ� ��� ��ȯ
    // ���ͽ�Ʈ�� �˰��� ���� ��ġ
    return vector<Location>();      // �ӽ� ��ȯ��
}
