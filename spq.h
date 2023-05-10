#ifndef SPQ_H
#define SPQ_H

#pragma once

#include "diff-serv.h"
#include "filter.h"
#include <iostream>
#include <unistd.h>

class SPQ : public DiffServ {
public:
    SPQ() {
        SetUp();
    }
    ~SPQ() {};

    static ns3::TypeId GetTypeId() {
        static ns3::TypeId tid = ns3::TypeId("SPQ")
            .SetParent<DiffServ>()
            .AddConstructor<SPQ>();
        return tid;
    }

    void SetUp() {
        int port = 1000; // TODO: config?
        for(size_t i=0; i < q_class.size(); i++) {
            vector<FilterElements*> filter_elements;
            filter_elements.push_back(new Destination_Port_Number(port));
            vector<Filter*> filters;
            filters.push_back (new Filter());
            filters[i]->set(filter_elements);
            q_class[i]->SetFilters(filters);
            port += 10000;
        }
    }

protected:

    // --------Classify---------
    uint32_t classify(Ptr<Packet> p) override {
        Ptr<Packet> copy = p->Copy();

        PppHeader pppHeader;
        copy->RemoveHeader(pppHeader);

        Ipv4Header ipv4Header;
        copy->RemoveHeader(ipv4Header);

        UdpHeader udpHeader;
        copy->PeekHeader(udpHeader);
        uint32_t destPort = udpHeader.GetDestinationPort();
        cout << "~~~Classify - Destination Port: " << destPort << endl;

        int index = GetDefaultIndex();
        for (size_t i = 0; i < q_class.size(); i++) {
            cout<<"\tInside for loop for q_class = " << i << endl;



            if (q_class[i]->match(p)) {

                return i;
            }
        }
        return index;
    }

    // --------Schedule---------
    Ptr<Packet> schedule() override {
        int highest_priority_index = GetHighestPriorityAvailableIndex();
        if (highest_priority_index == -1) {
            cout << "Schedule - all queues are empty" << endl;
            return nullptr;
        }
        cout << "Schedule - Q" << highest_priority_index << " is not empty, return the first packet in Q" << highest_priority_index << endl;
        Ptr<Packet> p = q_class[highest_priority_index]->Dequeue();
        return p;
    }
};

#endif /* SPQ_H */
