#ifndef FILTER_H
#define FILTER_H

#pragma once

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
using namespace std;

class FilterElements {
    public:
        virtual bool match (Ptr<Packet> p) = 0;
};

class Filter {
    public:
        vector<FilterElements*> elements;

        bool match (Ptr<Packet> p) {
            for (size_t i = 0; i < elements.size(); i++) {
                if (!elements[i]->match(p)) {
                    return false;
                }
            }
            return true;
        }

        void set(vector<FilterElements*> e) {
            elements = e;
        }
};

class Source_IP_Address : public FilterElements {
    private:
        Ipv4Address value;

    public:
        bool match (Ptr<Packet> p) {
            Ptr<Packet> copy = p->Copy();

            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);

            Ipv4Header ipv4Header;
            copy->RemoveHeader(ipv4Header);

            Ipv4Address srcIp = ipv4Header.GetSource();
            cout << "Filter: Source_IP_Address, Source IP: " << srcIp << endl;

            if (value == srcIp) {
                return true;
            }
            return false;
        }

        void set (Ipv4Address ip) {
            value = ip;
        }
};

class Source_Port_Number : public FilterElements {
    private:
        uint16_t value;

    public:
        bool match (Ptr<Packet> p) {
            Ptr<Packet> copy = p->Copy();

            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);

            Ipv4Header ipv4Header;
            copy->RemoveHeader(ipv4Header);

            UdpHeader udpHeader;
            copy->PeekHeader(udpHeader);
            uint32_t srcPort = udpHeader.GetSourcePort();
            cout << "Filter:Source_Port_Number, Source Port: " << srcPort << endl;

            if (value == srcPort) {
                return true;
            }
            return false;
        }

        void set (uint16_t port) {
            value = port;
        }
};

class Source_Mask : public FilterElements {
    private:
        Ipv4Mask value;
        Ipv4Address address;
        Ipv4Mask mask;

    public:
    Source_Mask(ns3::Ipv4Address address, ns3::Ipv4Mask mask) {
        this->address = address;
        this->mask = mask;
    }

    bool match (Ptr<Packet> p) {
        ns3::Ptr<ns3::Packet> copy = p->Copy();
        ns3::PppHeader pppHeader;
        copy->RemoveHeader(pppHeader);
        ns3::Ipv4Header ipv4Header;
        copy->RemoveHeader(ipv4Header);

        return (ipv4Header.GetDestination().CombineMask(mask) == address.CombineMask(mask));
    }

};

class Destination_IP_Address : public FilterElements {
    private:
        Ipv4Address value;

    public:
        bool match (Ptr<Packet> p) {
            Ptr<Packet> copy = p->Copy();

            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);

            Ipv4Header ipv4Header;
            copy->RemoveHeader(ipv4Header);

            Ipv4Address destIp = ipv4Header.GetDestination();

            if (value == destIp) {
                return true;
            }
            return false;
        }

        void set (Ipv4Address ip) {
            value = ip;
        }
};

class Destination_Port_Number : public FilterElements {
    private:
        uint16_t value;

    public:
        Destination_Port_Number (uint16_t port) {
            set(port);
        }

        bool match (Ptr<Packet> p) {
            Ptr<Packet> copy = p->Copy();

            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);

            Ipv4Header ipv4Header;
            copy->RemoveHeader(ipv4Header);

            UdpHeader udpHeader;
            copy->PeekHeader(udpHeader);
            uint32_t destPort = udpHeader.GetDestinationPort();
            return value == destPort;

        }

        void set (uint16_t port) {
            value = port;
        }
};

class Destination_Mask : public FilterElements {
    private:
        Ipv4Mask value;
        Ipv4Address address;
        Ipv4Mask mask;

    public:
    Destination_Mask(ns3::Ipv4Address address, ns3::Ipv4Mask mask) {
        this->address = address;
        this->mask = mask;
    }

    bool match (Ptr<Packet> p) {
        ns3::Ptr<ns3::Packet> copy = p->Copy();
        ns3::PppHeader pppHeader;
        copy->RemoveHeader(pppHeader);
        ns3::Ipv4Header ipv4Header;
        copy->RemoveHeader(ipv4Header);

        return (ipv4Header.GetDestination().CombineMask(mask) == address.CombineMask(mask));
    }
};

class Protocol_Number : public FilterElements {
    private:
        uint32_t value;

    public:
        bool match (Ptr<Packet> p) {
            Ptr<Packet> copy = p->Copy();

            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);

            Ipv4Header ipv4Header;
            copy->RemoveHeader(ipv4Header);

            uint32_t protocol = ipv4Header.GetProtocol();

            if (value == protocol) {
                return true;
            }
            return false;
        }

        void set (u_int32_t protocol) {
            value = protocol;
        }
};

#endif /* FILTER_H */
