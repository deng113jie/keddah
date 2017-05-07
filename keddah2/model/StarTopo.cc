#include "StarTopo.h"
#include "CONFIG.h"

#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"


using namespace ns3;
//
//long totaldrop=0;
//double star_now_sec=0;
//double star_last_sec=0;
//static void PktDrop (Ptr<const Packet> p)
//{
//    totaldrop+=p->GetSize();
//    double star_now_sec = Simulator::Now().GetSeconds ();
//    if(star_now_sec>star_last_sec+1)  //reduce the output
//    {
//        std::cout<<"pkt drop "<< star_now_sec << totaldrop<<std::endl;
//        //  std::cout<<"pkt "<< now_sec <<" "<< total_send<<" "<<totalbytes<<std::endl;
//        star_last_sec=star_now_sec;
//    }
//}

StarTopo::StarTopo(int n)
{



    host.Create(n);
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    InternetStackHelper stack;
    stack.Install (host);

    devices=csma.Install(host);

    //easy way
    Ipv4AddressHelper address;
    address.SetBase ("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer intface;
    intface = address.Assign (devices);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>
//            ("dynamic-global-routing.routes", std::ios::out);
//    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt (Seconds (2), routingStream);

    for(uint32_t i=0; i<devices.GetN(); i++)
    {
//        devices.Get(i)->TraceConnectWithoutContext("MacTxDrop", MakeCallback (&PktDrop));
//        devices.Get(i)->TraceConnectWithoutContext("MacRxDrop", MakeCallback (&PktDrop));
//        devices.Get(i)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback (&PktDrop));
//        devices.Get(i)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback (&PktDrop));
    }
   // csma.EnablePcap("star-topo",devices.Get(1),true);

}

StarTopo::StarTopo(int n,int corenum)
{
    for(int i=0;i<corenum;i++)
    {
        NodeContainer nodes;
        nodes.Create(n/corenum,i);
        host.Add(nodes);
    }

    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    InternetStackHelper stack;
    stack.Install (host);

    devices=csma.Install(host);

    //easy way
    Ipv4AddressHelper address;
    address.SetBase ("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer intface;
    intface = address.Assign (devices);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}

StarTopo::~StarTopo()
{
    //dtor
}
