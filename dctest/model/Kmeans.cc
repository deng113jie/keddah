#include "Kmeans.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"
#include "JobPara.h"
#include "MyApp.h"
#include "CONFIG.h"
#include <set>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Kmeans");

Kmeans::Kmeans()
{
    //ctor
}

Kmeans::~Kmeans()
{
    //dtor
}

Kmeans::Kmeans(int j,int n_c,int n_s, int n_d,int r)
{

    jobid=j;
    n_cluster=n_c;
    n_sample=n_s;
    n_dimension=n_d;

    filesize=(18+9*n_dimension)*n_sample*n_cluster;
    std::cout<< " filesize  " << filesize<<std::endl;
    rec_n=r;
}


void
Kmeans::SetStartTime(int s)
{
    starttime=s;
}

void
Kmeans::SetNodeContainer(NodeContainer n)
{
    nodes=n;
}


void
Kmeans::SetHDFS_r_Volume()
{
    double mean = 0.309;
    double variance = 0.06;
    hdfs_r_volume=totalvolume*JobPara::GetNormalRamdonVariableL1S1(mean,variance);
}



long
Kmeans::SetTotalVolume()
{

    totalvolume = (450000000+filesize*0.3)*(1+((rand()-RAND_MAX/2)%1000)/((float)10000));
    //std::cout<< " kmeans volume " << totalvolume <<std::endl;
    return totalvolume;
}


int
Kmeans::SetTotalNodes()
{
    double mean = totalvolume/128000000;
    double variance = 0.57;
    totalnodes=JobPara::GetNormalRamdonVariableL0(mean,variance);
    return totalnodes;
}



void
Kmeans::SetHDFS_rs_nodes()
{
    double mean = totalvolume/128000000*2;
    double variance = 1.2;
    hdfs_rs_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance);
}

void
Kmeans::SetHDFS_rr_nodes()
{
    double mean = totalvolume/128000000*2;
    double variance = 1.0;
    hdfs_rr_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance);

}

void
Kmeans::SetHDFS_ws_nodes()
{
    double mean = totalvolume/128000000*2;
    double variance = 0.793;
    hdfs_ws_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance)+1;
}
void
Kmeans::SetHDFS_wr_nodes()
{
     double mean = totalvolume/128000000*2;
    double variance = 0.793;
    hdfs_wr_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance)+1;
}


void
Kmeans::Sethdfs_r_sessions()
{
    double mean = totalvolume/128000000*4;
    double variance = 1.87;
    hdfs_r_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance)+1;
}

void
Kmeans::SetHDFS_w_sess()
{
    double mean = totalvolume/128000000*2;
    double variance = 0.793;
    hdfs_w_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance)+1;
}

double
Kmeans::RatioPerNode(int num)
{
    double mean = (double)1/num;
    // std::cout<<"hdfs nodes mean " << (double)1/n_num << std::endl;
    double variance = 0.01;
    return JobPara::GetNormalRamdonVariableL1S1(mean,variance);
}

int
Kmeans::HDFS_starttime()
{ //assume enough worknodes
    hdfs_stime_mean= 57.0703;
    hdfs_stime_var = 10.2331;

    return JobPara::GetNormalRamdonVariableL0(hdfs_stime_mean,hdfs_stime_var);
}

int
Kmeans::Shuffle_starttime()
{
    //assume enough worknodes
    double mean=hdfs_stime_mean*1.1;
    return JobPara::GetNormalRamdonVariableL0(mean,hdfs_stime_var);
}

void
Kmeans::Setup() //recursively running over rec_n
{
    for(int i=1; i<=rec_n; i++)
    {
        SetTotalVolume();
        SetupOnRec(i);
    }
    filesize=filesize*2.5*3.3;
    SetupOnRec(rec_n+1);
}


void
Kmeans::SetupOnRec(int rec)  //the traffic for each recursion
{

    NS_LOG_INFO("kmeans job "<<jobid<<" rec "<< rec <<" total volume "<<totalvolume <<
                " nodes "<< nodes.GetN()<<" time "<<starttime);

    //hdfs volume
    int hdfs_port=50010;
    SetHDFS_r_Volume();
    NS_LOG_INFO("hdfs volume "<<hdfs_r_volume);
    //hdfs send
    SetHDFS_rs_nodes();

    hdfs_rs_nodes=hdfs_rs_nodes>nodes.GetN()?nodes.GetN():hdfs_rs_nodes;
    std::map<int,double> send_map;
    NS_LOG_INFO("hdfs send nodes "<<hdfs_rs_nodes);
    for(uint32_t i =0; i<hdfs_rs_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        send_map[i]=RatioPerNode(hdfs_rs_nodes);
        NS_LOG_INFO("hdfs send ratio "<<send_map[i]);
    }
    //hdfs receive
    SetHDFS_rs_nodes();
    hdfs_rr_nodes=hdfs_rr_nodes>nodes.GetN()?nodes.GetN():hdfs_rr_nodes;
    std::map<int,double> rec_map;
    NS_LOG_INFO("hdfs receive nodes "<<hdfs_rr_nodes);
    for(uint32_t i =0; i<hdfs_rr_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        rec_map[i]=RatioPerNode(hdfs_rr_nodes);
        NS_LOG_INFO("hdfs receive ratio "<<rec_map[i]);
    }
    //hdfs session
    std::set<int> sess_map;

    long finish_time=0;
    Sethdfs_r_sessions();
    uint32_t exist_sess=0;

    for(std::map<int,double>::iterator sm = send_map.begin(); sm!=send_map.end(); sm++)
    {
        Ptr<Node> sender;
        Ptr<Node> recv;
        //sender
        sender=nodes.Get(sm->first);

        for(std::map<int,double>::iterator rm = rec_map.begin(); rm!=rec_map.end(); rm++)
        {

            recv=nodes.Get(nodes.GetN()-(rm->first)-1);
            //receiver
            if(sender->GetId()!=recv->GetId() &&
                    sm->second>0 && rm ->second >0
                    && sess_map.find(sender->GetId()*1000+recv->GetId())==sess_map.end() )
            {
                exist_sess++;
                float sessratio=RatioPerNode(hdfs_r_sess);
                double action = starttime+HDFS_starttime();
                sess_map.insert(sender->GetId()*1000+recv->GetId());

                if(sessratio>sm->second)
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfs_r_volume*sm->second,action);
                    sm->second=0;
                    rm->second=rm->second-sm->second;
                    action+=t;
                    action>finish_time?finish_time=action:0;
                }
                else
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfs_r_volume*sessratio,action);
                    sm->second-=sessratio;
                    rm->second=rm->second-sessratio;
                    action+=t;
                    action>finish_time?finish_time=action:0;
                }
            }
            if(exist_sess>=hdfs_r_sess)
            {
                break;
            }
        }
        if(exist_sess>=hdfs_r_sess)
        {
            break;
        }
    }

//shuffle

    //int shuf_port = 13562;
    int shuf_finish_time=0;
    long hdfs_w_volume = totalvolume-hdfs_r_volume;

    SetHDFS_ws_nodes();
    SetHDFS_wr_nodes();
    hdfs_ws_nodes=hdfs_ws_nodes>nodes.GetN()?nodes.GetN():hdfs_ws_nodes;
    hdfs_wr_nodes=hdfs_wr_nodes>nodes.GetN()?nodes.GetN():hdfs_wr_nodes;
    NS_LOG_INFO("hdfs write volume "<<hdfs_w_volume<<" hdfs write nodes "<< hdfs_ws_nodes << " receive nodes "<< hdfs_wr_nodes);
    send_map.clear();
    for(uint32_t i =0; i<hdfs_ws_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        send_map[i]=RatioPerNode(hdfs_ws_nodes);
        NS_LOG_INFO("shuffle send ratio "<<send_map[i]);
    }

    rec_map.clear();
    for(uint32_t i =0; i<hdfs_wr_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        rec_map[i]=RatioPerNode(hdfs_wr_nodes);
        NS_LOG_INFO("shuffle receive ratio "<<rec_map[i]);
    }
    sess_map.clear();
    SetHDFS_w_sess();
    exist_sess=0;

    for(std::map<int,double>::iterator sm = send_map.begin(); sm!=send_map.end(); sm++)
    {
        Ptr<Node> sender;
        Ptr<Node> recv;
        //sender
        sender=nodes.Get(nodes.GetN()-(sm->first)-1);
        for(std::map<int,double>::iterator rm = rec_map.begin(); rm!=rec_map.end(); rm++)
        {
            recv= nodes.Get(rm->first);
            //receiver
            if(sender->GetId()!=recv->GetId() &&
                    sm->second>0 && rm ->second >0
                    && sess_map.find(sender->GetId()*1000+recv->GetId())==sess_map.end() )
            {
                exist_sess++;
                float sessratio=RatioPerNode(hdfs_w_sess);

                sess_map.insert(sender->GetId()*1000+recv->GetId());
                double action = finish_time + (double)(rand()%100)/100*20;
                if(sessratio>sm->second)
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfs_w_volume*sm->second,action);
                    sm->second=0;
                    rm->second=rm->second-sm->second;
                    shuf_finish_time=action+t;

                }
                else
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfs_w_volume*sessratio,action);
                    sm->second-=sessratio;
                    rm->second=rm->second-sessratio;
                    shuf_finish_time=action+t;

                }
            }
            if(exist_sess>=hdfs_w_sess)
            {
                break;
            }
        }
        if(exist_sess>=hdfs_w_sess)
        {
            break;
        }
    }
    starttime=shuf_finish_time+20;
}


void
Kmeans::Receive(Ptr<Node> to, int port, double action)
{
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = packetSinkHelper.Install (to);
    sinkApps.Start (Seconds (action-1));
    sinkApps.Stop(Seconds(CONFIG::SIM_STOP));
}

long
Kmeans::SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action)
{
//    if(job_id!=CONFIG::TRACE_JOB)
//    {
//        return 0;
//    }


    NS_LOG_INFO(jobid<<" from "<< from->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal () << " to "<< to->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ()  << " size "<< bytes <<" "<<port<<" "<<action<<" ends "<<bytes*8/CONFIG::host_link_speed_long);


    Address sinkAddress (InetSocketAddress (to->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal (), port));


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

    return bytes/CONFIG::host_link_speed_long;
}

