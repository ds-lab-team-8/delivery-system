#ifndef DELIVERY_SYSTEM_WITH_SYSTEM_SELECTION_H
#define DELIVERY_SYSTEM_WITH_SYSTEM_SELECTION_H

#include "delivery_system.h"
#include <climits>
#include <vector>
#include <algorithm>
#include <iostream>

class DeliverySystemWithSystemSelection : DeliverySystem {
public:
    DeliverySystemWithSystemSelection();
    ~DeliverySystemWithSystemSelection();


    void Selection(); //어떤 주문을 어떤 기사에게 배치할지 정함
};



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

using namespace std;

bool sort_dis(DistanceAndNode* a, DistanceAndNode* b) {
    return a->distance < b->distance;
}

void DeliverySystemWithSystemSelection::Selection() {
    double** distance_arr;
    vector<vector<DistanceAndNode*>> vector_arr;

    distance_arr = new double* [drivers.size()];
    for (int i = 0;i < drivers.size();i++) {
        distance_arr[i] = new double[orders.size()];

        vector<DistanceAndNode*> temp;

        double min = INT_MAX;

        for (int j = 0;j < orders.size();j++) {

            //i번째기사가 음식점을 거쳐서 j번째 주문자에게 도달하기위한 최단거리
            distance_arr[i][j] =
                map.map_cost[orders[j]->store.getLocation().node][drivers[i].getCurrentLocation().node]
                + map.map_cost[orders[j]->orderer.getLocation().node][orders[j]->store.getLocation().node];

            if (distance_arr[i][j] < min) min = distance_arr[i][j];
        }

        for (int j = 0;j < orders.size();j++) {
            distance_arr[i][j] -= min;

            temp.push_back(new DistanceAndNode(distance_arr[i][j], i, j));
        }

        sort(temp.begin(), temp.end(), sort_dis);

        vector_arr.push_back(temp);
    }

    vector<DistanceAndNode*> result;

    while (true) {
        if (vector_arr.size() == 0) break;
        if (vector_arr[0].size() == 0)  //작동하면 에러
        {
            cout << "에러" << endl;
        }

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

        for (int i = 1; i < vector_arr[0].size(); i++)
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


    for (int i = 0;i < result.size();i++) {
        drivers[result[i]->driver].addOrder(orders[result[i]->order]); //주문 넣는 작업
        delete result[i];
    }
}


#endif
