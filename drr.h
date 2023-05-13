#ifndef DRR_H
#define DRR_H

#pragma once

#include "diff-serv.h"
#include "filter.h"
#include <iostream>
#include <unistd.h>

class DRR : public DiffServ {
public:
    DRR() {}
    ~DRR() {};

    static ns3::TypeId GetTypeId() {
        static ns3::TypeId tid = ns3::TypeId("DRR")
            .SetParent<DiffServ>()
            .AddConstructor<DRR>();
        return tid;
    }

    void SetUp() {
        // Set up Deficit Counter
        for (size_t i = 0; i < q_class.size(); i++) {
            deficit_counter.push_back(0);
        }

        next_queue_index = 0;

        int port = 10000;
        for(size_t i=0; i < q_class.size(); i++) {
            vector<FilterElements*> filter_elements;
            filter_elements.push_back (new Destination_Port_Number (port));
            vector<Filter*> filter;
            filter.push_back (new Filter());
            filter[0]->set(filter_elements);
            q_class[i]->SetFilters(filter);
            port += 10000;
        }
    }
private:

    vector<int> deficit_counter;
    int next_queue_index;


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
        cout << "---schedule" << endl;
        while (true) {
            if (AreAllQueuesEmpty()) {
                return nullptr;
            }
            if (!q_class[next_queue_index]->IsEmpty()) {
                deficit_counter[next_queue_index] += q_class[next_queue_index]->GetWeight();
                while (deficit_counter[next_queue_index] > 0 && !q_class[next_queue_index]->IsEmpty()) {
                    Ptr<Packet> p = q_class[next_queue_index]->Peek();
                    uint32_t packet_size = p->GetSize();
                    if (deficit_counter[next_queue_index] >= packet_size) {
                        deficit_counter[next_queue_index] -= packet_size;
                        return q_class[next_queue_index]->Dequeue();
                    } else {
                        break;
                    }
                }
                if (q_class[next_queue_index]->IsEmpty()) {
                    deficit_counter[next_queue_index] = 0;
                } else {
                    next_queue_index = (next_queue_index + 1) % q_class.size();
                }
            } else {
                next_queue_index = (next_queue_index + 1) % q_class.size();
            }
        }
    }
};

#endif /* DRR_H */
