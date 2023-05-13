#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "factory.h"
#include "ns3/flow-monitor-helper.h"

// --------------------Default Network Topology-----------------------
//
//       10.1.1.0           10.1.2.0
// n0 -------------- n2 -------------- n1
//     40Mbps, 20ms      10Mbps, 20ms
//
// n0: UDP Echo Client
// n1: UDP Echo Server
// n2: Router
// -------------------------------------------------------------------

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("QoS");

int main(int argc, char *argv[]) {

    // ----------Set up command line arguments----------
    CommandLine cmd(__FILE__);

    std::string inputFile = "";
    cmd.AddValue ("i", "inputFile for Application", inputFile);
    cmd.Parse(argc, argv);
    std::cout << "InputFile: " << inputFile << std::endl;

    // ----------Set up QoS name ----------
    string queue_name = inputFile.substr(inputFile.find_last_of("/\\") + 1);
    queue_name = queue_name.substr(0, queue_name.find_last_of("."));

    // ----------------Set up logging-------------------
    Time::SetResolution(Time::NS); // set time resolution to nanosecond
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO); // enable logging for UDP echo client
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO); // enable logging for UDP echo server

    // -----------------Create nodes--------------------
    NodeContainer nodes;
    nodes.Create(3);

    // -------------Create links and devices------------
    PointToPointHelper clientToRouter; // link between client and router
    clientToRouter.SetDeviceAttribute("DataRate", StringValue("40Mbps"));
    clientToRouter.SetChannelAttribute("Delay", StringValue("10ms"));

    PointToPointHelper routerToServer; // link between router and server
    routerToServer.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    routerToServer.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer clientToRouterDevices; // netdevice for client-router link
    clientToRouterDevices = clientToRouter.Install(nodes.Get(0), nodes.Get(2)); // install link between node 0 and node 2

    NetDeviceContainer routerToServerDevices; // netdevice for router-server link
    routerToServerDevices = routerToServer.Install(nodes.Get(2), nodes.Get(1)); // install link between node 2 and node 1

    // -----------Install queue to router---------------
    Ptr<Node> middleNode = nodes.Get(2); // get middle node
    Ptr<PointToPointNetDevice> middleDevice = middleNode->GetDevice(1)->GetObject<PointToPointNetDevice>(); // get middle device
    CreateDiffServ(inputFile, middleDevice);

    // --------------Install protocol stack-------------
    InternetStackHelper stack;
    stack.Install(nodes); // install internet stack on all nodes

    // ----------------Assign IP addresses--------------
    Ipv4AddressHelper address1;
    Ipv4AddressHelper address2;

    address1.SetBase("10.1.1.0", "255.255.255.0"); // set IP address for client-router link

    Ipv4InterfaceContainer clientToRouterInterfaces; // interface for client-router link
    clientToRouterInterfaces = address1.Assign(clientToRouterDevices); // assign IP address to client-router link

    address2.SetBase("10.2.2.0", "255.255.255.0"); // TODO: My understanding is that we need a second IP address for the second net device. Is that correct?
    Ipv4InterfaceContainer routerToServerInterfaces; // interface for router-server link
    routerToServerInterfaces = address2.Assign(routerToServerDevices); // assign IP address to router-server link

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

   // ----------------Create applications--------------
    UdpEchoServerHelper echoServer(10000);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1)); // install server application on node 1
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(60.0));

    // Set up the UdpEchoClient applications with different ports
    UdpEchoClientHelper echoClient1 (routerToServerInterfaces.GetAddress(1), 10000);
    echoClient1.SetAttribute ("MaxPackets", UintegerValue(120000));
    echoClient1.SetAttribute("Interval", TimeValue(MicroSeconds(100)));
    echoClient1.SetAttribute ("PacketSize", UintegerValue(1024));

    UdpEchoClientHelper echoClient2 (routerToServerInterfaces.GetAddress(1), 20000);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(120000));
    echoClient2.SetAttribute("Interval", TimeValue(MicroSeconds(100)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(1024));

    if (queue_name == "spq") {
        ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0));
        clientApps1.Start(Seconds(15.0));
        clientApps1.Stop(Seconds(50.0));

        ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0));
        clientApps2.Start(Seconds(3.0));
        clientApps2.Stop(Seconds(50.0));

        AsciiTraceHelper ascii;
        clientToRouter.EnablePcap ("plot-spq-1-", clientToRouterDevices.Get(0));
        routerToServer.EnablePcap ("plot-spq-2-", routerToServerDevices.Get(1));

    } else if (queue_name == "drr") {
        UdpEchoClientHelper echoClient3 (routerToServerInterfaces.GetAddress(1), 30000);
        echoClient3.SetAttribute("MaxPackets", UintegerValue(120000));
        echoClient3.SetAttribute("Interval", TimeValue(MicroSeconds(100)));
        echoClient3.SetAttribute("PacketSize", UintegerValue(1024));

        ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0));
        clientApps1.Start(Seconds(1.0));
        clientApps1.Stop(Seconds(60.0));

        ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0));
        clientApps2.Start(Seconds(1.0));
        clientApps2.Stop(Seconds(60.0));

        ApplicationContainer clientApps3 = echoClient3.Install(nodes.Get(0));
        clientApps3.Start(Seconds(1.0));
        clientApps3.Stop(Seconds(60.0));

        AsciiTraceHelper ascii;
        clientToRouter.EnablePcap ("plot-drr-1-", clientToRouterDevices.Get(0));
        routerToServer.EnablePcap ("plot-drr-2-", routerToServerDevices.Get(1));
    } else if (queue_name == "simple") {
        ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0));
        clientApps1.Start(Seconds(3.0));
        clientApps1.Stop(Seconds(50.0));

        ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0));
        clientApps2.Start(Seconds(3.0));
        clientApps2.Stop(Seconds(50.0));

        AsciiTraceHelper ascii;
        clientToRouter.EnablePcap ("plot-simple-1-", clientToRouterDevices.Get(0));
        routerToServer.EnablePcap ("plot-simple-2-", routerToServerDevices.Get(1));
    }

    Simulator::Run();
    Simulator::Destroy();

}
