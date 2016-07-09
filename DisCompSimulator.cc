#include <iostream>
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/mpi-interface.h"
#include "ns3/FatTree.h"
#include "ns3/Cluster.h"
#include "ns3/HadoopYarn.h"
#include "ns3/DCell.h"
#include "ns3/CamCube.h"
#include "ns3/StarTopo.h"
#include "ns3/CONFIG.h"


using namespace ns3;
using namespace std;


int
main (int argc, char *argv[])
{
    long filesize=3000000000;
    int network_type=0;
   // bool nullmsg = false;
    int fattree_nodesperpod=8;
    CommandLine cmd;
    cmd.AddValue ("filesize", "Size of data file need to be sort.", filesize);
    cmd.AddValue ("tracejob", "Which job to trace.", CONFIG::TRACE_JOB);
    cmd.AddValue ("network", "The type of network: (0 star) (1 fattree) (2 dcell) (3 camcube)", network_type);
    cmd.AddValue ("stop_time", "Number of seconds to run the simulation", CONFIG::SIM_STOP);
    cmd.AddValue ("job_rate", "Exponential mean of job submit interval", CONFIG::JOB_RATE);
    cmd.AddValue ("linktrad", "the trade-off rate of link and bandwidth in cam cube", CONFIG::LINKTRADOFF);
    cmd.AddValue ("ftnodnum", "number of nodes per pod in FatTree", fattree_nodesperpod);
   // cmd.AddValue ("nullmsg", "Enable the use of null-message synchronization", nullmsg);
    cmd.Parse (argc,argv);

//    if(nullmsg)
//    {
//        GlobalValue::Bind ("SimulatorImplementationType",
//                           StringValue ("ns3::NullMessageSimulatorImpl"));
//    }
//    else
//    {
//        GlobalValue::Bind ("SimulatorImplementationType",
//                           StringValue ("ns3::DistributedSimulatorImpl"));
//    }

// Enable parallel simulator with the command line arguments
    //  MpiInterface::Enable (&argc, &argv);
    //uint32_t systemId = MpiInterface::GetSystemId ();

    uint32_t systemCount =1;
//    if(MpiInterface::IsEnabled())
//    {
//        systemCount = MpiInterface::GetSize ();
//    }


    if(network_type==0)
    {
        // StarTopo star(8);
        StarTopo star(15,systemCount); //mpi approach
        Cluster cst (star);
        //Create the application
        HadoopYarn yarn (cst);
        yarn.StartJob(filesize,CONFIG::TRACE_JOB,CONFIG::JOB_RATE);
    }
    else if(network_type==1)
    {
        //core, per pod
        FatTree fattree (8,fattree_nodesperpod);
       // FatTree fattree (8,8,systemCount);
        fattree.setuplink();
        Cluster cst (fattree);
        //Create the application
        HadoopYarn yarn (cst);
        yarn.StartJob(filesize,CONFIG::TRACE_JOB,CONFIG::JOB_RATE);
    }
    else if(network_type==2)
    {
        DCell dcell(2,3);
        dcell.setup();
        //dcell.setup(systemCount);
        Cluster cst (dcell);
        //Create the application
        HadoopYarn yarn (cst);
        yarn.StartJob(filesize,CONFIG::TRACE_JOB,CONFIG::JOB_RATE);
    }
    else if(network_type==3)
    {
        CamCube cc(4);
        //cc.Setup(systemCount);
        cc.Setup();
        Cluster cst (cc);
        //Create the application
        HadoopYarn yarn (cst);
        yarn.StartJob(filesize,CONFIG::TRACE_JOB,CONFIG::JOB_RATE);
    }



    //flow monitor
   // FlowMonitorHelper flowmonHelper;
   // flowmonHelper.InstallAll();


    Simulator::Stop (Seconds (CONFIG::SIM_STOP));
    Simulator::Run ();

  //  stringstream ss;
  //  ss<< "flowmonitor_"<<network_type<<"_"<<CONFIG::JOB_RATE<<"_"<<CONFIG::LINKTRADOFF<<"_"<<fattree_nodesperpod<<".txt";
  //  flowmonHelper.SerializeToXmlFile (ss.str(), true, true);

    Simulator::Destroy ();

    return 0;
}




