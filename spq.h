#ifndef SPQ_H
#define SPQ_H

#pragma once

#include "diff-serv.h"
#include "filter.h"
#include <iostream>
#include <unistd.h>

class SPQ : public DiffServ {
public:
    SPQ() {};
    ~SPQ() {};

    static ns3::TypeId GetTypeId() {
        static ns3::TypeId tid = ns3::TypeId("SPQ")
            .SetParent<DiffServ>()
            .AddConstructor<SPQ>();
        return tid;
    }

    void SetUp() {
        int port = 10000;
        for(size_t i=0; i < q_class.size(); i++) {
            vector<FilterElements*> filter_elements;
            filter_elements.push_back(new Destination_Port_Number(port));
            vector<Filter*> filters;
            filters.push_back (new Filter());
            filters[0]->set(filter_elements);
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

        int index = GetDefaultIndex();
        for (size_t i = 0; i < q_class.size(); i++) {
            if (q_class[i]->match(p)) {
                cout << "---Classify - index: " << i << endl;
                return i;
            }
        }
        cout << "---Classify - index: " << index << endl;
        return index;
    }

    // --------Schedule---------
    Ptr<const Packet> schedule() override {
        int highest_priority_index = GetHighestPriorityAvailableIndex();
        if (highest_priority_index == -1) {
            return nullptr;
        }
        Ptr<Packet> p = q_class[highest_priority_index]->Dequeue();
        return p;
    }
};

#endif /* SPQ_H */
