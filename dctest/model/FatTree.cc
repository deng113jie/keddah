#include "FatTree.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-helper.h"
#include "CONFIG.h"
#include <arpa/inet.h>

using namespace ns3;
//using namespace std;

FatTree::FatTree()
{
    //ctor
}

FatTree::FatTree(uint32_t k, uint32_t n) :
    k(k),
    n(n)
{
    edge.Create(k*2);
    aggre.Create(k*2);
    core.Create((k/2)*(k/2));
    host.Create(k*n);

    InternetStackHelper stack;
    stack.Install (host);
    stack.Install (edge);
    stack.Install (aggre);
    stack.Install (core);

    std::cout<<"Topo: FatTree nodes"<< host.GetN()<<std::endl;
    a=10;
    b=1;
    c=1;
}

FatTree::FatTree(uint32_t k, uint32_t n, int corenum) :
    k(k),
    n(n)
{
    for(int i=0;i<corenum;i++)
    {
        NodeContainer edgen;
        edgen.Create((k*2)/corenum,i);
        edge.Add(edgen);

        NodeContainer aggren;
        aggren.Create((k*2)/corenum,i);
        aggre.Add(aggren);

        NodeContainer coren;
        coren.Create(((k/2)*(k/2))/corenum,i);
        core.Add(coren);

        NodeContainer hostn;
        hostn.Create((k*n)/corenum,i);
        host.Add(hostn);
    }

   // edge.Create(k*2);
   // aggre.Create(k*2);
   // core.Create((k/2)*(k/2));
   // host.Create(k*n);

    InternetStackHelper stack;
    stack.Install (host);
    stack.Install (edge);
    stack.Install (aggre);
    stack.Install (core);

    std::cout<<"Topo: FatTree nodes"<< host.GetN()<<std::endl;
    a=10;
    b=1;
    c=1;
}


FatTree::~FatTree()
{
    //dtor
}

const char *
FatTree::GetIP()
{
    if(c==254)
    {
        b++;
        c=1;
    }
    else
    {
        c++;
    }
    if(b==254)
    {
        a++;
        b=1;
    }
    std::ostringstream strm;
    strm << a <<'.'<< b<<'.'<< c<<".0";
    const char*  numStr = strm.str().c_str();

//    struct sockaddr_in sa;
//    int result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
//
//    while(result!=1)
//    {
//        numStr= GetIP();
//        result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
//    }
//    std::cout<<numStr<<" "<<result<<std::endl;
    return numStr;
}


void
FatTree::setuplink()
{
    //host to edge
    for(uint32_t i=0; i<host.GetN();)
    {
        CsmaHelper csma;
        csma.SetChannelAttribute ("DataRate", StringValue (CONFIG::host_link_speed));
        csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::csma_delay)));
        uint32_t edge_sw=i/(n/2);
        NodeContainer nodes;
        nodes.Add(edge.Get(edge_sw));
        uint32_t j=0;
        for(; j<n/2; j++)
        {
            nodes.Add(host.Get(i+j));
        }
        const char * ip=FatTree::GetIP();
        struct sockaddr_in sa;
        int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

        while(result!=1)
        {
            ip= GetIP();
            result = inet_pton(AF_INET, ip, &(sa.sin_addr));
        }

        address.SetBase (ip, "255.255.255.0");
        address.Assign(csma.Install(nodes));
        i=i+j;
    }

    //edge to aggre
    for(uint32_t i=0; i<aggre.GetN(); i++)
    {
        if(i%2==0)
        {
            PointToPointHelper link1 ;
            link1.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
            // link1.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));

            const char * ip=FatTree::GetIP();
            struct sockaddr_in sa;
            int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

            while(result!=1)
            {
                ip= GetIP();
                result = inet_pton(AF_INET, ip, &(sa.sin_addr));
            }
            address.SetBase (ip, "255.255.255.0");
            address.Assign(link1.Install(edge.Get(i),aggre.Get(i)));

            PointToPointHelper link2 ;
            link2.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
            // link2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));
            //devices.Add(link2.Install(edge.Get(i),aggre.Get(i+1)));
            ip=FatTree::GetIP();

            result = inet_pton(AF_INET, ip, &(sa.sin_addr));

            while(result!=1)
            {
                ip= GetIP();
                result = inet_pton(AF_INET, ip, &(sa.sin_addr));
            }
            address.SetBase (ip, "255.255.255.0");
            address.Assign(link2.Install(edge.Get(i),aggre.Get(i+1)));
        }
        else
        {
            PointToPointHelper link1 ;
            link1.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
            // link1.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));


            const char *  ip=FatTree::GetIP();
            struct sockaddr_in sa;
            int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

            while(result!=1)
            {
                ip= GetIP();
                result = inet_pton(AF_INET, ip, &(sa.sin_addr));
            }
            address.SetBase (ip, "255.255.255.0");
            address.Assign(link1.Install(edge.Get(i),aggre.Get(i)));

            PointToPointHelper link2 ;
            link2.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
            // link2.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));

            ip=FatTree::GetIP();

            result = inet_pton(AF_INET, ip, &(sa.sin_addr));

            while(result!=1)
            {
                ip= GetIP();
                result = inet_pton(AF_INET, ip, &(sa.sin_addr));
            }
            address.SetBase (ip, "255.255.255.0");
            address.Assign(link2.Install(edge.Get(i),aggre.Get(i-1)));
        }
    }

    //aggre to core
    for(uint32_t i=0; i<core.GetN(); i++)
    {
        if(i<core.GetN()/2)
        {
            //even aggre node
            for(uint32_t j=0; j<aggre.GetN(); j+=2)
            {
                PointToPointHelper link ;
                link.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
                // link.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));

                const char * ip=FatTree::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                address.SetBase (ip, "255.255.255.0");
                address.Assign(link.Install(core.Get(i),aggre.Get(j)));
            }
        }
        else
        {
            //odd aggre node
            for(uint32_t j=1; j<aggre.GetN(); j+=2)
            {
                PointToPointHelper link ;
                link.SetDeviceAttribute ("DataRate", StringValue (CONFIG::trunk_link_speed));
                // link.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::trunk_delay)));

                const char * ip=FatTree::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                address.SetBase (ip, "255.255.255.0");
                address.Assign(link.Install(core.Get(i),aggre.Get(j)));
            }
        }
    }

    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue(true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}





