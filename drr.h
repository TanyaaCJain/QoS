#ifndef DRR_H
#define DRR_H

#pragma once

#include "diff-serv.h"
#include "filter.h"
#include <iostream>
#include <unistd.h>

class DRR : public DiffServ {
public:
    DRR() {
        SetUp();
    }
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

        int port = 1000;
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
    vector<int> active_list;


protected:

    // --------DoEnqueue---------
    bool Enqueue(Ptr<Packet> p) override {
        uint32_t index = classify(p);
        // if index does not exist in active_list, add it to active_list and set deficit_counter to 0
        if (find(active_list.begin(), active_list.end(), index) == active_list.end()) {
            active_list.push_back(index);
            deficit_counter[index] = 0;
        }
        // if no buffer space, free buffer
        if (q_class[index]->IsFull()) {
            cout << "DoEnqueue - Q" << index << " is full, free buffer" << endl;
            Ptr<Packet> p = q_class[index]->Dequeue();
            // return false;
        }
        // call doEnqueue of base class
        if (DiffServ::DoEnqueue(p)) {
            cout << "DoEnqueue - packet is enqueued to Q" << index << endl;
            return true;
        } else {
            cout << "DoEnqueue - packet is dropped because Q" << index << " is full" << endl;
            return false;
        }

        // if (index == GetDefaultIndex()) {
        //     cout << "DoEnqueue - packet does not match any classification, drop it" << endl;
        //     return false;
        // }
        // cout << "DoEnqueue - packet belongs to Q" << index << endl;
        // if (q_class[index]->Enqueue(p)) {
        //     cout << "DoEnqueue - packet is enqueued to Q" << index << endl;
        //     return true;
        // } else {
        //     cout << "DoEnqueue - packet is dropped because Q" << index << " is full" << endl;
        //     return false;
        // }
    }

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
        cout << "Destination Port: " << destPort << endl;

        int index = GetDefaultIndex();
        for (size_t i = 0; i < q_class.size(); i++) {
            if (q_class[i]->match(p)) {
                return i;
            }
        }
        return index;
    }

    // --------Schedule---------
    Ptr<Packet> schedule() override {
        while (true) {
            // if active_list is not empty, remove head of active_list
            if (!active_list.empty()) {
                int index = active_list[0];
                active_list.erase(active_list.begin());
                deficit_counter[index] += q_class[index]->GetWeight();
                // while deficit_counter is greater than 0 and queue is not empty, packet size = size of head of queue
                while (deficit_counter[index] > 0 && !q_class[index]->IsEmpty()) {
                    Ptr<Packet> p = q_class[index]->GetHeadPacket();
                    uint32_t packet_size = p->GetSize();
                    // if deficit_counter is greater than packet size, dequeue packet and subtract packet size from deficit_counter
                    if (deficit_counter[index] > packet_size) {
                        cout << "DoDequeue - deficit_counter is greater than packet size, packet is dequeued from Q" << index << endl;
                        Ptr<Packet> p = q_class[index]->Dequeue();
                        // p = schedule();
                        deficit_counter[index] -= packet_size;
                        return p;
                    } else {
                        break;
                    }
                }
                // if queue is empty, continue
                if (q_class[index]->IsEmpty()) {
                    deficit_counter[index] = 0;
                } else {
                    // insert index to the end of active_list
                    active_list.push_back(index);
                }
            }
        }
    }
};

#endif /* DRR_H */
