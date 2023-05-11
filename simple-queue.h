#ifndef SIMPLE_QUEUE_H
#define SIMPLE_QUEUE_H

#pragma once

#include "diff-serv.h"
#include <iostream>
#include <unistd.h>

class SimpleQueue : public DiffServ {
public:
    SimpleQueue() {}
    ~SimpleQueue() {};

    static ns3::TypeId GetTypeId() {
        static ns3::TypeId tid = ns3::TypeId("SimpleQueue")
            .SetParent<DiffServ>()
            .AddConstructor<SimpleQueue>();
        return tid;
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
        cout << "Destination Port: " << destPort << endl;

        if (destPort == 10000) {
            return 0; // packet belongs to Q1
        } else if (destPort == 20000) {
            return 1; // packet belongs to Q2 (not going to be served in sechedule function)
        } else {
            return 2; // packet does not match any classification, drop it
        }
    }

    // --------Schedule---------
    Ptr<const Packet> schedule()  override {
        if (q_class[0]->IsEmpty()) {
            cout << "Schedule - Q1 is empty" << endl;
            return nullptr;
        }
        cout << "Schedule - Q1 is not empty, return the first packet in Q1" << endl;
        Ptr<Packet> p = q_class[0]->Dequeue();
        return p;
    }
};

#endif /* SIMPLE_QUEUE_H */
