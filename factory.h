#ifndef FACTORY_H
#define FACTORY_H

#pragma once

#include "ns3/queue.h"
#include "diff-serv.h"
#include "simple-queue.h"
#include "spq.h"
#include "drr.h"
#include <iostream>

using namespace ns3;
using namespace std;

void CreateDiffServ(string inputfile, Ptr<PointToPointNetDevice>& middleDevice) {
    string queue_name = inputfile.substr(inputfile.find_last_of("/\\") + 1);
    queue_name = queue_name.substr(0, queue_name.find_last_of("."));
    cout << "QoS: " << queue_name << endl;
    if (queue_name == "simple") {
        Ptr<SimpleQueue> pointQueue = CreateObject<SimpleQueue>();
        middleDevice->SetQueue(pointQueue);
        pointQueue->readConfig(inputfile);
    } else if (queue_name == "spq") {
        Ptr<SPQ> pointQueue = CreateObject<SPQ>();
        middleDevice->SetQueue(pointQueue);
        pointQueue->readConfig(inputfile);
        pointQueue->SetUp();
        cout << "-------SetUp() DONE-------" << endl;
    } else if  (queue_name == "drr") {
        Ptr<DRR> pointQueue = CreateObject<DRR>();
        middleDevice->SetQueue(pointQueue);
        pointQueue->readConfig(inputfile);
        pointQueue->SetUp();
    } else {
        cerr << "Error: queue name " << queue_name << " is not supported" << endl;
    }
};

#endif /* FACTORY_H */
