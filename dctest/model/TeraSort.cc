#include "TeraSort.h"

#include <math.h>
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

NS_LOG_COMPONENT_DEFINE ("TeraSort");

TeraSort::TeraSort()
{
    //ctor
}
TeraSort::TeraSort(int j, long f, int c)
{
    jobid=j;
    filesize=f;
    clusize=c;
}

TeraSort::~TeraSort()
{
    //dtor
}
void
TeraSort::SetStartTime(int s)
{
    starttime=s;
}
void
TeraSort::SetNodeContainer(NodeContainer n)
{
    nodes=n;
}

void
TeraSort::SetHDFS_Volume()
{
    double mean = 0;
    double variance = 0;
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        mean = filesize/1000000000*-0.0884531+0.20007;
        variance = filesize/1000000000*-0.0433925+0.139703;
    }
    else
    {
        mean = filesize/1000000000*0.0246222+0.0864632;
        variance = filesize/1000000000*0.0115897+0.0432207;
    }

    hdfsvolume=JobPara::GetNormalRamdonVariableL0(mean,variance/1)*1000000000;
}

void
TeraSort::SetShuffle_Volume()
{
    double mean = filesize/1000000000*0.419963-0.0295886;
    double variance = filesize/1000000000*0.0153912+0.00236879;
    shufvolume=JobPara::GetNormalRamdonVariableL0(mean,variance/1)*1000000000;
}

/*
x-> gig

HDFS volume
mean f(x)=0.0414909*x+0.0214362
var f(x)=0.0166092*x+0.0373767

shuffle volume
mean f(x)=0.428614*x-0.0255268
variable f(x)=0.00937971*x+0.00932772

total source nodes
    (fixed)mapper number

hdfs source
    (normal)mean=0.042381*x+14.2133
            var=-0.0301677*x+0.785415
hdfs destination
    (fixed)mapper number
hdfs sess
    (normal)mean=-0.936905*x+28.7848
    var=0.0449321*x+1.72513
hdfs start
    (normal)mean=1.22433*x+6.97643
    var=1.70999*x+1.97468

shuffle source
    (fixed)mapper number
shuffle destination
    (fixed) 2
shuf sess
    (normal)mean=13.8161*x+0.203571
    var=0.200827*x+0.488334
shuffle start time
    (normal)mean=2.56906*x+16.0362
    var=0.791069*x-0.525854
*/


long
TeraSort::SetTotalVolume()
{

    SetHDFS_Volume();
    SetShuffle_Volume();
    totalvolume=hdfsvolume+shufvolume;
    return totalvolume;
}


int
TeraSort::SetTotalNodes()
{
//    double mean = totalvolume/128000000;
//    double variance = 0.64;
//    totalnodes=JobPara::GetNormalRamdonVariableL0(mean,variance);
//    return totalnodes;
    return filesize/CONFIG::hdfs_block_size;
}

/*
hdfs source
    (normal)mean=0.042381*x+14.2133
            var=-0.0301677*x+0.785415
*/
void
TeraSort::SetHDFS_send_nodes()

{
    double mean=0;
    double variance=0;
    float M1 = 3.37178;
    float N1 = 4.59915;
    float M2 = 0.137458;
    float N2 = 1.08005;
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        mean = filesize/1000000000*M1+N1;
        variance = filesize/1000000000*M2+N2;
        hdfs_s_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else if(filesize <= CONFIG::convergence_factor*clusize*CONFIG::hdfs_block_size)
    {
        double b=((double)CONFIG::hdfs_block_size)/1000000000;
        double x=((double)filesize)/1000000000;
        float n = (float)clusize;
        float k = (float)CONFIG::convergence_factor;

        mean =  (n-M1*n*b-N1)/((k-1)*n*b)*x + n - (k/(k-1))*(n-M1*n*b-N1);
        //std::cout<<b<<" " <<x<<" "<<n<<" "<<k/(k-1)<<" "<<mean<<std::endl;

        variance = (n-M2*n*b-N1)/((k-1)*n*b) * x + n - k/(k-1)*(n-M2*n*b-N2);
        hdfs_s_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else
    {
        hdfs_s_nodes=clusize;
    }
}

void
TeraSort::SetHDFS_recv_nodes()
{
    double mean=0;
    double variance=0;
    float M1 = 5.89128;
    float N1 = 2.92;
    float M2 = 0.213137;
    float N2 = 0.436639;
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        mean = filesize/1000000000*M1+N1;
        variance = filesize/1000000000*M2+N2;
        hdfs_r_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else if(filesize <= CONFIG::convergence_factor*clusize*CONFIG::hdfs_block_size)
    {
        double b=((double)CONFIG::hdfs_block_size)/1000000000;
        double x=((double)filesize)/1000000000;
        float n = (float)clusize;
        float k = (float)CONFIG::convergence_factor;
        mean = (n-M1*n*b-N1)/((k-1)*n*b) * x + n - k/(k-1)*(n-M1*n*b-N1);
        // std::cout<<b<<" " <<x<<" "<<n<<" "<<k<<" "<<mean<<std::endl;

        variance = (n-M2*n*b-N2)/((k-1)*n*b) * x + n - k/(k-1)*(n-M2*n*b-N2);
        hdfs_r_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else
    {
        hdfs_r_nodes=clusize;
    }
}

void
TeraSort::SetShuffle_send_nodes()
{
//    double mean = hdfsvolume/128000000;
//    double variance = 0.793;
//    shuf_s_nodes=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance)+1;
    shuf_s_nodes=ceil(filesize/CONFIG::hdfs_block_size);

}
void
TeraSort::SetShuffle_recv_nodes()
{
    // double mean = 1.00;
    // double variance = 0.05;
    shuf_r_nodes=(uint32_t)2;
}


void
TeraSort::SetHDFS_sessions()
{
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        double mean = filesize/1000000000*22.5578+2.93786;
        double variance = filesize/1000000000*0.866516+1.28802;
        hdfs_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1)+1;
    }
    else
    {
        double mean = filesize/1000000000*12.7956+22.3338;
        double variance = filesize/1000000000*0.16695+2.59054;
        hdfs_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1)+1;
    }

    // NS_LOG_FUNCTION("TeraSort job "<<jobid<<" set hdfs sess"<<hdfs_sess);
}

void
TeraSort::SetShuf_sessions()
{
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        double mean = filesize/1000000000*247.28-120.294;
        double variance = filesize/1000000000*46.1851-21.9674;
        shuf_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1)+1;
    }
    else
    {
        double mean = filesize/1000000000*661.226-709.027;
        double variance = filesize/1000000000*43.8483-1.77191;
        shuf_sess=(uint32_t)JobPara::GetNormalRamdonVariableL0(mean,variance/1)+1;
    }
    // NS_LOG_FUNCTION("TeraSort job "<<jobid<<" set shuffle sess"<<shuf_sess);
}

double TeraSort::RatioPerNode(int num)
{
    double mean = (double)1/num;
    // std::cout<<"hdfs nodes mean " << (double)1/num << std::endl;
    double variance = 0.01;
    return JobPara::GetNormalRamdonVariableL1S1(mean,variance);
}

int
TeraSort::HDFS_starttime()
{
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        double mean=10415/1000;
        double variance =(filesize/1000000000*-1100.08+4112.23)/1000;
        //std::cout<<mean << " "<<variance<< std::endl;
        return JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else
    {
        double mean=(filesize/1000000000*625.006+8463.99)/1000;
        double variance =(filesize/1000000000*319.437+976.005)/1000;
        //std::cout<<mean << " "<<variance<< std::endl;
        return JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }

}

int
TeraSort::Shuffle_starttime()
{
    if(filesize <= clusize*CONFIG::hdfs_block_size)
    {
        double mean=19945.5775/1000;
        double variance =(filesize/1000000000*415.533+586.862)/1000;
        //std::cout<<mean << " "<<variance<< std::endl;
        return JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
    else
    {
        double mean=(filesize/1000000000*2626.69+14439.6)/1000;
        double variance =(filesize/1000000000*751.254-339.587)/1000;
        //std::cout<<mean << " "<<variance<< std::endl;
        return JobPara::GetNormalRamdonVariableL0(mean,variance/1);
    }
}


void
TeraSort::Setup()
{


    //hdfs volume
    int hdfs_port=50010;
    //hdfs send
    SetHDFS_send_nodes();

    hdfs_s_nodes=hdfs_s_nodes>nodes.GetN()?nodes.GetN():hdfs_s_nodes;
    std::map<int,double> send_map;
    for(uint32_t i =0; i<hdfs_s_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        send_map[i]=RatioPerNode(hdfs_s_nodes);
        //NS_LOG_INFO("hdfs send ratio "<<send_map[i]);
    }
    //hdfs receive
    SetHDFS_recv_nodes();
    hdfs_r_nodes=hdfs_r_nodes>nodes.GetN()?nodes.GetN():hdfs_r_nodes;
    std::map<int,double> rec_map;
    //NS_LOG_INFO("HDFS volume"<< hdfsvolume << "hdfs send nodes "<<hdfs_s_nodes << "hdfs receive nodes "<<hdfs_r_nodes);
    for(uint32_t i =0; i<hdfs_r_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        rec_map[i]=RatioPerNode(hdfs_r_nodes);
        //NS_LOG_INFO("hdfs receive ratio "<<rec_map[i]);
    }
    //hdfs session
    std::set<int> sess_map;

    long finish_time=0;
    SetHDFS_sessions();
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

                float sessratio=RatioPerNode(hdfs_sess);
                double action = starttime+HDFS_starttime();
                sess_map.insert(sender->GetId()*1000+recv->GetId());

                if(sessratio>sm->second)
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfsvolume*sm->second,action);
                    sm->second=0;
                    rm->second=rm->second-sm->second;
                    action+=t;
                    action>finish_time?finish_time=action:0;
                }
                else
                {
                    Receive(recv,hdfs_port,action);
                    long t = SendTo(sender,recv,hdfs_port,hdfsvolume*sessratio,action);
                    sm->second-=sessratio;
                    rm->second=rm->second-sessratio;
                    action+=t;
                    action>finish_time?finish_time=action:0;
                }
            }
            if(exist_sess>=hdfs_sess)
            {
                break;
            }
        }
        if(exist_sess>=hdfs_sess)
        {
            break;
        }
    }

//shuffle

    int shuf_port = 13562;
    // long shufvolume = totalvolume-hdfsvolume;
    SetShuffle_send_nodes();
    SetShuffle_recv_nodes();
    shuf_s_nodes=shuf_s_nodes>nodes.GetN()?nodes.GetN():shuf_s_nodes;
    shuf_r_nodes=shuf_r_nodes>nodes.GetN()?nodes.GetN():shuf_r_nodes;
    //NS_LOG_INFO("shuffle volume "<<shufvolume<<" shuffle send nodes "<< shuf_s_nodes << " receive nodes "<< shuf_r_nodes);
    send_map.clear();
    for(uint32_t i =0; i<shuf_s_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        send_map[i]=RatioPerNode(shuf_s_nodes);
        // NS_LOG_INFO("shuffle send ratio "<<send_map[i]);
    }

    rec_map.clear();
    for(uint32_t i =0; i<shuf_r_nodes; i++)
    {
        // Receive(nodes.Get(i),hdfs_port,start_time);
        rec_map[i]=RatioPerNode(shuf_r_nodes);
        //   NS_LOG_INFO("shuffle receive ratio "<<rec_map[i]);
    }
    sess_map.clear();
    SetShuf_sessions();
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
                float sessratio=RatioPerNode(shuf_sess);

                sess_map.insert(sender->GetId()*1000+recv->GetId());
                //double action = starttime + Shuffle_starttime()+finish_time;
                double action = starttime + Shuffle_starttime();
                if(sessratio>sm->second)
                {
                    Receive(recv,shuf_port,action);
                    SendTo(sender,recv,shuf_port,shufvolume*sm->second,action);
                    sm->second=0;
                    rm->second=rm->second-sm->second;
                }
                else
                {
                    Receive(recv,shuf_port,action);
                    SendTo(sender,recv,shuf_port,shufvolume*sessratio,action);
                    sm->second-=sessratio;
                    rm->second=rm->second-sessratio;
                }
            }
            if(exist_sess>=shuf_sess)
            {
                break;
            }
        }
        if(exist_sess>=shuf_sess)
        {
            break;
        }
    }
    NS_LOG_INFO("TeraSort job "<<jobid<<" file_size "<<filesize<<" hdfs_size "<<hdfsvolume <<" shuf_size "<<shufvolume<<
                " nodes "<< nodes.GetN()<<" hdfs_s "<<hdfs_s_nodes<<" hdfs_r "<<hdfs_r_nodes<<" shuf_s "<<shuf_s_nodes<<" shuf_r "<<shuf_r_nodes <<" hdfs_flows "<<hdfs_sess<<" shuf_flows "<<shuf_sess<< " time "<<starttime);
}

void
TeraSort::Receive(Ptr<Node> to, int port, double action)
{
    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = packetSinkHelper.Install (to);
    sinkApps.Start (Seconds (action-1));
    sinkApps.Stop(Seconds(CONFIG::SIM_STOP));
}

long
TeraSort::SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action)
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
    //socket,address,size,pkt_num,rate
    app->Setup (ns3TcpSocket, sinkAddress, 1000, bytes/1000, DataRate (CONFIG::host_link_speed));
    from->AddApplication (app);
    app->SetStartTime (Seconds (action));

    return bytes/CONFIG::host_link_speed_long;
}
