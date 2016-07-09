#include "HadoopJob.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"
#include "CONFIG.h"
#include "JobPara.h"
#include "HadoopYarn.h"
#include "MyApp.h"
#include <map>
#include <stdlib.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("HadoopJob");

double totalbytes=0;



HadoopJob::HadoopJob()
{
    //ctor

}

HadoopJob::HadoopJob(int j,float s,long d, NodeContainer n)
{
    job_id=j;
    start_time=s;
    datafilesize=d;
    nodes=n;
    NS_LOG_INFO("job "<<job_id<<" file size "<<datafilesize<<
                " nodes "<< nodes.GetN()<<" time "<<s);

}

HadoopJob::~HadoopJob()
{
    //dtor
}


void HadoopJob::setup()
{
    //hdfs


//    for(uint32_t i=0; i<nodes.GetN(); i++)
//    {
//        for(uint32_t j=0; j<nodes.Get(i)->GetNDevices(); j++)
//        {
//            //  nodes.Get(i)->GetDevice(j)->TraceConnectWithoutContext("MacRx", MakeCallback (&PktRecv));
//            nodes.Get(i)->GetDevice(j)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback (&PktDrop));
//
//            //  nodes.Get(i)->GetDevice(j)->TraceConnectWithoutContext("MacTx", MakeCallback (&PktSend));
//
//            nodes.Get(i)->GetDevice(j)->TraceConnectWithoutContext("MacTxDrop", MakeCallback (&PktDrop));
//            nodes.Get(i)->GetDevice(j)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback (&PktDrop));
//        }
//    }
}

void
HadoopJob::Receive(Ptr<Node> to, int port, double action)
{
}

long
HadoopJob::SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action)
{
//    if(job_id!=CONFIG::TRACE_JOB)
//    {
//        return 0;
//    }


    NS_LOG_INFO(job_id<<" from "<< from->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal () << " to "<< to->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ()  << " size "<< bytes <<" "<<port<<" "<<action<<" ends "<<bytes*8/CONFIG::host_link_speed_long);

//receive
    Address sinkAddress (InetSocketAddress (to->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal (), port));

    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = packetSinkHelper.Install (to);
    sinkApps.Start (Seconds (action-1));
    sinkApps.Stop(Seconds(CONFIG::SIM_STOP));
    //sinkApps.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback (&SinkRecv));

    //send 1
//    OnOffHelper onoff = OnOffHelper ("ns3::TcpSocketFactory", sinkAddress);
//    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
//    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
//
//    /* off by Erlang distribution
//    Ptr<ErlangRandomVariable> x = CreateObject<ErlangRandomVariable> ();
//    x->SetAttribute ("K", IntegerValue (20/4));
//    x->SetAttribute ("Lambda", DoubleValue (3));
//    onoff.SetAttribute ("OffTime", ns3::PointerValue(x));
//    */
//    onoff.SetAttribute("MaxBytes",ns3::UintegerValue(bytes));
//    onoff.SetAttribute ("PacketSize", StringValue("1050"));
//    onoff.SetAttribute ("DataRate", StringValue(CONFIG::host_link_speed));
//    ApplicationContainer app = onoff.Install(from);
//    app.Start(Seconds (action));
//    app.Stop(Seconds(CONFIG::SIM_STOP));
//  //  app.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback (&PktSend));

    //send 2
    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (from, TcpSocketFactory::GetTypeId ());
    Ptr<MyApp> app = CreateObject<MyApp> ();
    app->Setup (ns3TcpSocket, sinkAddress, 1000, bytes/1000, DataRate (CONFIG::host_link_speed));
    from->AddApplication (app);
    app->SetStartTime (Seconds (action));


    totalbytes+=bytes;

    return bytes/CONFIG::host_link_speed_long;

}






