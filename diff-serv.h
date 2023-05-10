#ifndef DIFF_SERV_H
#define DIFF_SERV_H

#pragma once

#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "traffic-class.h"

using namespace ns3;
using namespace std;

class DiffServ : public ns3::Queue<ns3::Packet> {
    public:
        DiffServ() {};
        ~DiffServ() override {};

        static TypeId GetTypeId () {
            static TypeId tid = TypeId("DiffServ")
            .SetParent<Queue<Packet>>()
            .AddConstructor<DiffServ>();
            return tid;
        };

        bool Enqueue(Ptr<Packet> item) override {
            return DoEnqueue(item);
        }

        Ptr<Packet> Remove() override {
            return nullptr;
        }

        Ptr<const Packet> Peek() const override {
            return nullptr; // TODO: ?
        }

        Ptr<Packet> Dequeue() override {
            return DoDequeue();
        }

        void readConfig(const string& file_path) {
            ifstream infile(file_path);
            if (!infile.is_open()) {
                cerr << "Failed to open file: " << file_path << endl;
                return;
            }

            string line;
            getline(infile, line);
            int num_queues = stoi(line);

            for (int i = 0; i < num_queues; i++) {
                getline(infile, line);
                istringstream iss(line);
                string token;
                getline(iss, token, ',');
                int max_packets = stoi(token);
                getline(iss, token, ',');
                int weight = stoi(token);
                getline(iss, token, ',');
                int priority_level = stoi(token);
                getline(iss, token, ',');
                bool is_default = (token == "true");

                TrafficClass* tc = new TrafficClass();
                q_class.push_back(tc);
                q_class[i]->SetMaxPackets(max_packets);
                q_class[i]->SetWeight(weight);
                q_class[i]->SetPriorityLevel(priority_level);
                q_class[i]->SetIsDefault(is_default);
            }

            infile.close();
        }

    private:
        using Queue<Packet>::GetContainer;
        using Queue<Packet>::DoRemove;
        using Queue<Packet>::DoPeek;

    protected:
        vector<TrafficClass*> q_class;

        vector<TrafficClass*> get_q_class() {
            return q_class;
        }

        bool DoEnqueue(Ptr<Packet> item) {
            uint32_t queue_number = classify(item);
            // check if the queue number is in the index range of q_class
            if (queue_number >= q_class.size() || queue_number < 0) {
                cout << "DoEnqueue - queue number is out of range" << endl;
                return false;
            }
            // check if the queue is full
            if (q_class[queue_number]->IsFull()) {
                cout << "DoEnqueue - queue is full" << endl;
                return false;
            }
            // enqueue
            cout << "---Enqueue - packet belongs to Q" << queue_number << endl;
            bool val = q_class[queue_number]->Enqueue(item);
            cout << "DoEnqueue - enqueue result: " << val << endl;
            return val;
            // return true;
        }

        Ptr<Packet> DoDequeue() {
            Ptr<Packet> p = schedule();
            return p;
        }

        virtual uint32_t classify(Ptr<Packet> p) {
            cout << "---classify" << endl;
            return 0;
        };

        virtual Ptr<Packet> schedule() {
            cout << "---schedule" << endl;
            return nullptr;
        };

        int GetDefaultIndex() {
            for (int i = 0; i < q_class.size(); i++) {
                if (q_class[i]->GetIsDefault()) {
                    return i;
                }
            }
            return -1;
        };

        int GetHighestPriorityAvailableIndex() {
            int highest_priority = 0;
            int highest_priority_index = -1;
            if (!q_class[0]->IsEmpty()) {
                highest_priority_index = 0;
            }
            for (int i = 1; i < q_class.size(); i++) {
                if (q_class[i]->GetPriorityLevel() > highest_priority && !q_class[i]->IsEmpty()) {
                    highest_priority = q_class[i]->GetPriorityLevel();
                    highest_priority_index = i;
                }
            }
            cout << "---GetHighestPriorityAvailableIndex " << highest_priority_index << endl;
            return highest_priority_index;
        };
};

#endif /* DIFF_SERV_H */
