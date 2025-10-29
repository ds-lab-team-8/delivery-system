#include "delivery_system_with_systemselection.h"
#include <climits>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

DeliverySystemWithSystemSelection::DeliverySystemWithSystemSelection() : DeliverySystem() {}

DeliverySystemWithSystemSelection::~DeliverySystemWithSystemSelection() = default;

class DistanceAndNode {
public:
    DistanceAndNode(double dis, int d, int o) {
        distance = dis;
        driver = d;
        order = o;
    }


    double distance;
    int driver;
    int order;
};


bool sort_dis(DistanceAndNode* a, DistanceAndNode* b) {
    return a->distance < b->distance;
}

void DeliverySystemWithSystemSelection::acceptCall() {
    vector<Driver>& drivers = getDrivers();
    vector<Order*>& orders = getOrders();
    Map& map = getMap();
    
    vector<Order*> acceptedOrders;
    for (Order* order : orders) {
        if (order->getStatus() == ORDER_ACCEPTED) {
            acceptedOrders.push_back(order);
        }
    }
    
    if (acceptedOrders.empty() || drivers.empty()) {
        return;
    }
    
    if (map.map_cost == nullptr) {
        return;
    }
    
    double** distance_arr;
    vector<vector<DistanceAndNode*>> vector_arr;

    distance_arr = new double* [drivers.size()];
    for (int i = 0;i < drivers.size();i++) {
        distance_arr[i] = new double[acceptedOrders.size()];

        vector<DistanceAndNode*> temp;

        double min = INT_MAX;

        for (int j = 0;j < (int)acceptedOrders.size();j++) {
            const Store* orderStore = acceptedOrders[j]->getStore();
            const Orderer* orderOrderer = acceptedOrders[j]->getOrderer();
            
            if (!orderStore || !orderOrderer) {
                distance_arr[i][j] = INT_MAX;
                continue;
            }
            
            const Location& storeLoc = orderStore->getLocation();
            const Location& ordererLoc = orderOrderer->getLocation();
            const Location& driverLoc = drivers[i].getCurrentLocation();
            
            if (storeLoc.node == -1 || ordererLoc.node == -1 || driverLoc.node == -1) {
                distance_arr[i][j] = INT_MAX;
                continue;
            }
            
            if (storeLoc.node >= (int)map.nodes.size() || ordererLoc.node >= (int)map.nodes.size() || 
                driverLoc.node >= (int)map.nodes.size()) {
                distance_arr[i][j] = INT_MAX;
                continue;
            }

            double cost1 = map.map_cost[storeLoc.node][driverLoc.node];
            double cost2 = map.map_cost[ordererLoc.node][storeLoc.node];
            
            distance_arr[i][j] = cost1 + cost2;

            if (distance_arr[i][j] < min) min = distance_arr[i][j];
        }

        for (int j = 0;j < (int)acceptedOrders.size();j++) {
            distance_arr[i][j] -= min;

            temp.push_back(new DistanceAndNode(distance_arr[i][j], i, j));
        }

        sort(temp.begin(), temp.end(), sort_dis);

        vector_arr.push_back(temp);
    }

    vector<DistanceAndNode*> result;

    while (true) {
        if (vector_arr.size() == 0) break;
        if (vector_arr[0].size() == 0) break;

        if (vector_arr.size() == 1) {   //남은 기사가 한명
            result.push_back(vector_arr[0][0]);

            for (int i = 1; i < vector_arr[0].size(); i++)
            {
                delete vector_arr[0][i];
            }

            break;
        }

        if (vector_arr[0].size() == 1) {  //남은 주문이 1개
            result.push_back(vector_arr[0][0]);

            for (int i = 1; i < vector_arr.size(); i++)
            {
                delete vector_arr[i][0];
            }

            break;
        }


        //배차 알고리즘
        double min = INT_MAX;
        int index_i;
        DistanceAndNode* temp;
        for (int i = 0;i < vector_arr.size();i++) {
            double value = vector_arr[i][1]->distance - vector_arr[i][0]->distance;

            if (value < min) {
                min = value;

                index_i = i;
                temp = vector_arr[i][0];
            }
        }

        for (int i = 1; i < vector_arr[index_i].size(); i++)
        {
            delete vector_arr[index_i][i];
        }
        vector_arr.erase(vector_arr.begin() + index_i);

        for (int i = 0;i < vector_arr.size();i++) {
            for (int j = 0; j < vector_arr[i].size(); j++)
            {
                if (vector_arr[i][j]->order == temp->order) {
                    delete vector_arr[i][j];
                    vector_arr[i].erase(vector_arr[i].begin() + j);
                    break;
                }
            }
        }

        result.push_back(temp);
    }


    for (int i = 0;i < (int)result.size();i++) {
        int driverId = result[i]->driver;
        int orderIdx = result[i]->order;
        Order* order = acceptedOrders[orderIdx];
        
        drivers[driverId].addOrder(order);
        order->assignDriver(drivers[driverId].getId());
        
        delete result[i];
    }
    
    for (int i = 0; i < (int)drivers.size(); i++) {
        delete[] distance_arr[i];
    }
    delete[] distance_arr;
}
