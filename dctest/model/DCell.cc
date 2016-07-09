#include "DCell.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-helper.h"
#include "CONFIG.h"
#include "arpa/inet.h"



using namespace ns3;


DCell::DCell(int kk,int nn)
{
    if(n_of_server(kk,nn)>10000000)
    {
        std::cout<<"To Much servers for DCell in ns3"<<std::endl;
    }
    else
    {
        k=kk;
        n=nn;
        std::cout<<"Topo: Dcell nodes"<< DCell::n_of_server(k,n) <<std::endl;

        a=10;
        b=1;
        c=1;
    }
}


DCell::~DCell()
{
    //dtor
}

const char *
DCell::GetIP()
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
//
//    struct sockaddr_in sa;
//    int result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
//
//    while(result!=1)
//    {
//        numStr=CONFIG::GetIP();
//        result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
//    }
//    std::cout<<numStr<<" "<<result<<std::endl;
    return numStr;
}

int
DCell::n_of_server(int k,int n)
{
    if(k==0)
    {
        return n;
    }
    else
    {
        return (n_of_server(k-1,n)+1)*n_of_server(k-1,n);
    }

}

int
DCell::Cells_at_k(int k,int n)
{
    if(k==0)
    {
        return n;
    }
    else if(k==1)
    {
        return n+1;
    }
    else
    {
        return Cells_at_k(k-1,n)*(Cells_at_k(k-1,n)-1)+1;
    }
}


void
DCell::setup()
{

    if(k>0 && n>0)
    {

        NodeContainer sw;
        for(int i=k; i>0; i--)
        {
            int smallcell=Cells_at_k(i-1,n);
            if(i==k)
            {
                //std::cout<<Cells_at_k(i,n)<<std::endl;
                sw.Create((uint32_t)Cells_at_k(i,n));
                stack.Install(sw);
            }
            NodeContainer nodes;
            nodes.Create(sw.GetN()*smallcell);
            stack.Install(nodes);

            //host to switch
//            //p2p way
//            for(uint32_t j=0; j<nodes.GetN(); j++)
//            {
//
//                PointToPointHelper p2p;
//                // devices.Add(p2p.Install(nodes.Get(j),sw.Get(j/smallcell)));
//                std::cout<<"s "<<CONFIG::GetIP()<<std::endl;
//                address.SetBase(CONFIG::GetIP(),"255.255.255.0");
//
//                address.Assign(p2p.Install(nodes.Get(j),sw.Get(j/smallcell)));
//                // std::cout<<nodes.Get(j)->GetId()<<" "<<sw.Get(j/smallcell)->GetId()<<std::endl;
//            }
            //csma way
            for(uint32_t j=0; j*smallcell<nodes.GetN(); j++)
            {
                NodeContainer samesw;
                samesw.Add(sw.Get(j));
                for(uint32_t k=0; k<(uint32_t)smallcell; k++)
                {
                    //std::cout<<j<<" "<<j*smallcell+k<<std::endl;
                    samesw.Add(nodes.Get(j*smallcell+k));
                }
                CsmaHelper csma;
                csma.SetChannelAttribute ("DataRate", StringValue (CONFIG::host_link_speed));
                csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::csma_delay)));
                csma.SetQueue("ns3::RedQueue");

                const char * ip=DCell::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                address.SetBase(ip,"255.255.255.0");
                address.Assign(csma.Install(samesw));
            }

            //host to host

            for(uint32_t j=0; j<nodes.GetN(); j++)
            {
                if(j/smallcell > j%smallcell)
                {
                    // std::cout<<j/smallcell<<" "<<j%smallcell<<" "<<j%smallcell<<" "<<j/smallcell-1<<std::endl;

                    int to1=j%smallcell;
                    int to2=(j/smallcell)-1;

                    uint32_t toseq=(uint32_t)j%smallcell*smallcell+
                                   ((j/smallcell)-1);
                    if(to1>=0 && to2>=0  )
                    {
                        PointToPointHelper p2p;
                        p2p.SetQueue("ns3::RedQueue");
                        // devices.Add(p2p.Install(nodes.Get(j),nodes.Get(toseq)));
                        //std::cout<<CONFIG::GetIP()<<std::endl;
                        const char * ip=DCell::GetIP();
                        struct sockaddr_in sa;
                        int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                        while(result!=1)
                        {
                            ip= GetIP();
                            result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                        }
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(p2p.Install(nodes.Get(j),nodes.Get(toseq)));
                        //   std::cout<<nodes.Get(j)->GetId()<<" "<<nodes.Get(toseq)->GetId()<<std::endl;
                    }
                }
            }
            // swes.Add(sw);
            if(i==1)
            {
                host=nodes;
                //std::cout<<"host "<<host.GetN()<<std::endl;
            }
            else
            {
                sw=nodes;
                //std::cout<<"sw "<<sw.GetN()<<std::endl;
            }

        }

    }
    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue(true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}

void
DCell::setup(int corenum)
{

    if(k>0 && n>0)
    {

        NodeContainer sw;
        for(int i=k; i>0; i--)
        {
            int smallcell=Cells_at_k(i-1,n);
            if(i==k)
            {
                //std::cout<<Cells_at_k(i,n)<<std::endl;
                sw.Create((uint32_t)Cells_at_k(i,n));
                stack.Install(sw);
            }
            NodeContainer nodes;
            if(i==1)
            {
                for(int j=0; j<corenum; j++)
                {
                    NodeContainer oncore;
                    oncore.Create(sw.GetN()*smallcell/corenum,j);
                    nodes.Add(oncore);
                }
                stack.Install(nodes);
            }
            else
            {
                nodes.Create(sw.GetN()*smallcell);
                stack.Install(nodes);
            }


            //host to switch
//            //p2p way
//            for(uint32_t j=0; j<nodes.GetN(); j++)
//            {
//
//                PointToPointHelper p2p;
//                // devices.Add(p2p.Install(nodes.Get(j),sw.Get(j/smallcell)));
//                std::cout<<"s "<<CONFIG::GetIP()<<std::endl;
//                address.SetBase(CONFIG::GetIP(),"255.255.255.0");
//
//                address.Assign(p2p.Install(nodes.Get(j),sw.Get(j/smallcell)));
//                // std::cout<<nodes.Get(j)->GetId()<<" "<<sw.Get(j/smallcell)->GetId()<<std::endl;
//            }
            //csma way
            for(uint32_t j=0; j*smallcell<nodes.GetN(); j++)
            {
                NodeContainer samesw;
                samesw.Add(sw.Get(j));
                for(uint32_t k=0; k<(uint32_t)smallcell; k++)
                {
                    //std::cout<<j<<" "<<j*smallcell+k<<std::endl;
                    samesw.Add(nodes.Get(j*smallcell+k));
                }
                CsmaHelper csma;
                csma.SetChannelAttribute ("DataRate", StringValue (CONFIG::host_link_speed));
                csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (CONFIG::csma_delay)));

                const char * ip=DCell::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                address.SetBase(ip,"255.255.255.0");
                address.Assign(csma.Install(samesw));
            }

            //host to host

            for(uint32_t j=0; j<nodes.GetN(); j++)
            {
                if(j/smallcell > j%smallcell)
                {
                    // std::cout<<j/smallcell<<" "<<j%smallcell<<" "<<j%smallcell<<" "<<j/smallcell-1<<std::endl;

                    int to1=j%smallcell;
                    int to2=(j/smallcell)-1;

                    uint32_t toseq=(uint32_t)j%smallcell*smallcell+
                                   ((j/smallcell)-1);
                    if(to1>=0 && to2>=0  )
                    {
                        PointToPointHelper p2p;
                        // devices.Add(p2p.Install(nodes.Get(j),nodes.Get(toseq)));
                        //std::cout<<CONFIG::GetIP()<<std::endl;
                        const char * ip=DCell::GetIP();
                        struct sockaddr_in sa;
                        int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                        while(result!=1)
                        {
                            ip= GetIP();
                            result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                        }
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(p2p.Install(nodes.Get(j),nodes.Get(toseq)));
                        //   std::cout<<nodes.Get(j)->GetId()<<" "<<nodes.Get(toseq)->GetId()<<std::endl;
                    }
                }
            }
            // swes.Add(sw);
            if(i==1)
            {
                host=nodes;
                //std::cout<<"host "<<host.GetN()<<std::endl;
            }
            else
            {
                sw=nodes;
                //std::cout<<"sw "<<sw.GetN()<<std::endl;
            }

        }

    }
    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue(true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}


