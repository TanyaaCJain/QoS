#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#pragma once

#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "diff-serv.h"
#include "filter.h"
#include <queue>

using namespace ns3;
using namespace std;

class TrafficClass : public Object {
    public:
        TrafficClass() {
            bytes = 0;
            packets = 0;
            maxPackets = 1000;
            maxBytes = 50000;
            weight = 0;
            priority_level = 0;
            isDefault = false;
        }

        static TypeId GetTypeId () {
            static TypeId tid = TypeId("TrafficClass")
            .SetParent<Object>()
            .AddConstructor<TrafficClass>();
            return tid;
        };

        bool Enqueue(Ptr<Packet> p) {
            if (packets + 1 <= maxPackets) {
                packets++;
                m_queue.push(p);
                return true;
            }
            return false;
        }

        Ptr<Packet> Dequeue() {
            if (m_queue.empty()) {
                return nullptr;
            } else {
                Ptr<Packet> p = m_queue.front();
                m_queue.pop();
                packets--;
                return p;
            }
        }

        void SetMaxPackets(uint32_t maxPackets) {
            this->maxPackets = maxPackets;
        }

        void SetPriorityLevel(uint32_t priority_level) {
            this->priority_level = priority_level;
        }

        void SetWeight(double_t weight) {
            this->weight = weight;
        }

        void SetIsDefault(bool isDefault) {
            this->isDefault = isDefault;
        }

        void SetFilters(vector<Filter*> filters) {
            this->filters = filters;
        }

        uint32_t GetMaxPackets() {
            return maxPackets;
        }

        uint32_t GetPriorityLevel() {
            return priority_level;
        }

        double_t GetWeight() {
            return weight;
        }

        Ptr<Packet> GetHeadPacket() {
            return m_queue.front();
        }

        bool GetIsDefault() {
            return isDefault;
        }

        queue<Ptr<Packet>> GetQueue() {
            return m_queue;
        }

        vector<Filter*> GetFilters() {
            return filters;
        }

        bool IsEmpty() {
            return m_queue.empty();
        }

        bool IsFull() {
            return packets == maxPackets;
        }

        bool match(Ptr<Packet> p) {

            for (size_t i = 0; i < filters.size(); i++) {
                Filter* f = filters[i];
                if (f->match(p)) {
                    return true;
                }
            }
            return false;
        }

    private:
        uint32_t bytes;
        uint32_t packets;
        uint32_t maxPackets;
        uint32_t maxBytes;
        double_t weight;
        uint32_t priority_level;
        bool isDefault;
        queue<Ptr<Packet>> m_queue;
        vector<Filter*> filters;
};

#endif /* TRAFFIC_CLASS_H */
