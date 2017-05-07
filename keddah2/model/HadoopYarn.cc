#include "HadoopYarn.h"
#include "JobPara.h"
#include "TeraSort.h"
#include "Kmeans.h"
#include "YarnScheduler.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/random-variable-stream.h"

#include <set>
#include <unistd.h>

using namespace ns3;

Cluster HadoopYarn::cst;

HadoopYarn::HadoopYarn() {
	// ctor
}
HadoopYarn::HadoopYarn(Cluster c) {
	HadoopYarn::cst = c;
	YarnScheduler y(c.getHosts().GetN());
}

HadoopYarn::~HadoopYarn() {
	// dtor
}
/*
 static void
 CwndTracer (uint32_t oldval, uint32_t newval)
 {
 // std::cout<<"tcp_cwnd "<< Simulator::Now().GetSeconds () <<" "<< oldval<<" "<<newval<<std::endl;
 }

 static void
 RttTracer (Time oldval, Time newval)
 {
 // std::cout<<"tcp_rtt "<< Simulator::Now().GetSeconds () <<" "<< oldval.GetMilliSeconds()<<"
 "<<newval.GetMilliSeconds()<<std::endl;
 }

 static void
 RtoTracer (Time oldval, Time newval)
 {
 // std::cout<<"tcp_rto "<<Simulator::Now ().GetSeconds ()<<" "<< oldval.GetSeconds () <<" " <<
 newval.GetSeconds()<<std::endl;
 }*/

double TxTrTotal = 0;
double TxTime = 0;
static void TxTracer(Ptr<const Packet> p) {
	TxTrTotal += p->GetSize();
	double now_sec = Simulator::Now().GetSeconds();
	if (now_sec > TxTime + 1)  //reduce the output
			{
		std::cout << "pkt_send " << now_sec << " " << TxTrTotal << std::endl;
		TxTime = now_sec;
	}
}
double RxTrTotal = 0;
double RxTime = 0;
static void RxTracer(Ptr<const Packet> p) {
	RxTrTotal += p->GetSize();
	double now_sec = Simulator::Now().GetSeconds();
	if (now_sec > RxTime + 1)  //reduce the output
			{
		std::cout << "pkt_recv " << now_sec << " " << RxTrTotal << std::endl;
		RxTime = now_sec;
	}
}

double DpTrTotal = 0;
double DpTime = 0;
static void DpTracer(Ptr<const Packet> p) {
	DpTrTotal += p->GetSize();
	double now_sec = Simulator::Now().GetSeconds();
	if (now_sec > DpTime + 1)  //reduce the output
			{
		std::cout << "pkt_drop " << now_sec << " " << DpTrTotal << std::endl;
		DpTime = now_sec;
	}
}
void HadoopYarn::StartRecvTraffic() {
	for (uint32_t i = 0; i < cst.getHosts().GetN(); i++) {
		Ptr<Node> to = cst.getHosts().Get(i);
		PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), 50010));
		ApplicationContainer sinkApps = packetSinkHelper.Install(to);
		sinkApps.Start(Seconds(1.));
		//	sinkApps.Stop(Seconds(CONFIG::SIM_STOP));
	}

	for (uint32_t i = 0; i < cst.getHosts().GetN(); i++) {
		Ptr<Node> to = cst.getHosts().Get(i);
		PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), 13562));
		ApplicationContainer sinkApps = packetSinkHelper.Install(to);
		sinkApps.Start(Seconds(1.));
		// sinkApps.Stop(Seconds(CONFIG::SIM_STOP));
	}
}

void TestFunc() {
}

std::set<Ptr<SystemThread> > HadoopYarn::StartJob(long file, int total,
		double exp_mean) {
	StartRecvTraffic();

	// Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpReno::GetTypeId()));

	// DCTCP
	/*
	 Config::SetDefault("ns3::TcpSocketBase::DCTCP", BooleanValue(true));
	 Config::SetDefault("ns3::RedQueue::UseCurrent", BooleanValue(true));
	 Config::SetDefault("ns3::RedQueue::MinTh", DoubleValue(40));
	 Config::SetDefault("ns3::RedQueue::MaxTh", DoubleValue(100));
	 Config::SetDefault("ns3::TcpSocketBase::DCTCPWeight", DoubleValue (1.0 / 16.0)); */

	double mean = exp_mean;
	double bound = 0.0;
	Ptr<ExponentialRandomVariable> start_t = CreateObject<
			ExponentialRandomVariable>();
	start_t->SetAttribute("Mean", DoubleValue(mean));
	start_t->SetAttribute("Bound", DoubleValue(bound));

	double currenttime = 3;

	std::set<Ptr<SystemThread> > threadstorun;
	for (int run = 1; run <= total; run++) {
		 //if(rand()/(float)RAND_MAX<=0.5){  //terasort
		if (1) {
			// NOTE: SystemThread can't start right away, wait 1 sec; so the prog shouldn't stop right away, wait a few
			// seconds.
			Ptr<TeraSort> tsort = CreateObject<TeraSort>(run, file);
			currenttime += 10; //start_t->GetValue();
			tsort->SetStartTime(currenttime);
			Ptr<SystemThread> pcs;
			pcs = Create<SystemThread>(
					MakeCallback(&TeraSort::start, tsort));
			//   mapper->Start();   //not working for real time simulator
			Simulator::Schedule(Seconds(currenttime), &SystemThread::Start,
					&(*pcs));
			threadstorun.insert(pcs);
			sleep(1);

		} else if (1) // kmeans
		{


			Ptr<Kmeans> kms = CreateObject<Kmeans>(run,20,1000,2,2);
						currenttime += 10; //start_t->GetValue();
						kms->SetStartTime(currenttime);
						Ptr<SystemThread> pcs;
						pcs = Create<SystemThread>(
								MakeCallback(&Kmeans::start, kms));
						//   mapper->Start();   //not working for real time simulator
						Simulator::Schedule(Seconds(currenttime), &SystemThread::Start,
								&(*pcs));
						threadstorun.insert(pcs);
						sleep(1);
		}
	}
	sleep(1);

	//    Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RWND", MakeCallback
	// (&CwndTracer));
	//
	//    Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTO", MakeCallback (&RtoTracer));
	//    Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTT", MakeCallback (&RttTracer));
	//
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacTx",
			MakeCallback(&TxTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/MacTx",
			MakeCallback(&TxTracer));

	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacRx",
			MakeCallback(&RxTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/MacRx",
			MakeCallback(&RxTracer));

	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxDrop",
			MakeCallback(&DpTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyRxDrop",
			MakeCallback(&DpTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/MacTxDrop",
			MakeCallback(&DpTracer));

	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyTxDrop",
			MakeCallback(&DpTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyRxDrop",
			MakeCallback(&DpTracer));
	Config::ConnectWithoutContext(
			"/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/MacTxDrop",
			MakeCallback(&DpTracer));
	//
	//
	//    //drop for queue
	//     Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/TxQueue/Drop", MakeCallback
	// (&DpTracer));
	//      Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/TxQueue/Drop",
	// MakeCallback (&DpTracer));
	return threadstorun;
}
