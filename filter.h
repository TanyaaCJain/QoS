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
            cout << "Source IP: " << srcIp << endl;

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
            cout << "Source Port: " << srcPort << endl;

            if (value == srcPort) {
                return true;
            }
            return false;
        }

        void set (uint16_t port) {
            value = port;
        }
};

// class Source_Mask : public FilterElements {
//     private:
//         Ipv4Mask value;

//     public:
//         bool match (Ptr<Packet> p) {
//             Ptr<Packet> copy = p->Copy();

//             PppHeader pppHeader;
//             copy->RemoveHeader(pppHeader);

//             Ipv4Header ipv4Header;
//             copy->RemoveHeader(ipv4Header);



//             Ipv4Mask srcMask = ipv4Header.GetSourceMaskAddress();
//             cout << "Source Mask: " << srcMask << endl;

//             if (value == srcMask) {
//                 return true;
//             }
//             return false;
//         }

//         void set (Ipv4Mask mask) {
//             value = mask;
//         }
// };

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
            cout << "Destination IP: " << destIp << endl;

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
            cout << "Destination Port: " << destPort << endl;

            if (value == destPort) {
                return true;
            }
            return false;
        }

        void set (uint16_t port) {
            value = port;
        }
};

// class Destination_Mask : public FilterElements {
//     private:
//         Ipv4Mask value;

//     public:
//         bool match (Ptr<Packet> p) {
//             Ptr<Packet> copy = p->Copy();

//             PppHeader pppHeader;
//             copy->RemoveHeader(pppHeader);

//             Ipv4Header ipv4Header;
//             copy->RemoveHeader(ipv4Header);

//             Ipv4Mask destMask = ipv4Header.GetDestinationMask();
//             cout << "Destination Mask: " << destMask << endl;

//             if (value == destMask) {
//                 return true;
//             }
//             return false;
//         }

//         void set (Ipv4Mask mask)  {
//             value = mask;
//         }
// };

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
            cout << "Protocol: " << protocol << endl;

            if (value == protocol) {
                return true;
            }
            return false;
        }

        void set (u_int32_t protocol) {
            value = protocol;
        }
};
// } // namespace ns3

#endif /* FILTER_H */
