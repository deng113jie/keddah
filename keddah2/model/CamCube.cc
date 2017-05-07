#include "CamCube.h"
#include "CONFIG.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-helper.h"

#include "arpa/inet.h"

using namespace ns3;

CamCube::CamCube(uint32_t nn) :
    n(nn)
{
    //ctor
    a=10;
    b=1;
    c=1;
    std::cout<<"Topo: Camcube nodes"<< n*n*n <<std::endl;
}

CamCube::~CamCube()
{
    //dtor
}



const char *
CamCube::GetIP()
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
	    char *buff = (char*)malloc(sizeof(char)*20);
	    memset(buff,'\0',strlen(buff));
	    sprintf(buff,"%d.%d.%d.0",a,b,c);
	   // const char*  numStr = strm.str().c_str();

	//    struct sockaddr_in sa;
	//    int result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
	//
	//    while(result!=1)
	//    {
	//        numStr= GetIP();
	//        result = inet_pton(AF_INET, numStr, &(sa.sin_addr));
	//    }
	   // std::cout<<buff<<" "<<std::endl;
	    return buff;
}

const char* ipseg;


void
CamCube::Setup(int ncore)
{
    if(n>0)
    {
        for(uint32_t s=0; s<n; s++)
        {
            // std::cout<<s<<" "<<n<<std::endl;
            NodeContainer slice1;
            for(uint32_t c=0; c<n*n; c++)
            {
                NodeContainer oncore;
                oncore.Create(1,c%ncore);
                slice1.Add(oncore);
            }

            // slice1.Create(n*n);
            stack.Install(slice1);

            CONFIG::host_link_speed_long=1000000000*(1+(1-CONFIG::LINKTRADOFF));

            for(uint32_t i=0; i<slice1.GetN(); i++)
            {
                //down
                PointToPointHelper down;
                down.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
                if(i+n<slice1.GetN())
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(down.Install(slice1.Get(i),slice1.Get(i+n)));
                        //  std::cout<<i<<" "<<i+n<<std::endl;
                    }

                }
                else
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(down.Install(slice1.Get(i),slice1.Get(i%n)));
                        //  std::cout<<i<<" "<<i%n<<std::endl;
                    }
                }
                //right
                PointToPointHelper rig;
                rig.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
                if(i%n==(n-1))
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(rig.Install(slice1.Get(i),slice1.Get(i-n+1)));
                        // std::cout<<i<<" "<<i-n+1<<std::endl;
                    }
                }
                else
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(rig.Install(slice1.Get(i),slice1.Get(i+1)));
                        //  std::cout<<i<<" "<<i+1<<std::endl;
                    }
                }
            }
            host.Add(slice1);
        }
        //forward
        for(uint32_t i=0; i<n*n*n; i++)
        {
            PointToPointHelper forw;

            forw.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
            if(i+n*n<n*n*n)
            {
                const char * ip=CamCube::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                {
                    address.SetBase(ip,"255.255.255.0");
                    address.Assign(forw.Install(host.Get(i),host.Get(i+n*n)));
                    // std::cout<<i<<" "<<i+n*n<<std::endl;
                }
            }
            else
            {
                const char * ip=CamCube::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                {
                    address.SetBase(ip,"255.255.255.0");
                    address.Assign(forw.Install(host.Get(i),host.Get(i%(n*n))));
                    //  std::cout<<i<<" "<<i%(n*n)<<std::endl;
                }
            }
        }
    }


    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue(true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}



void
CamCube::Setup()
{
    if(n>0)
    {
        for(uint32_t s=0; s<n; s++)
        {
            // std::cout<<s<<" "<<n<<std::endl;
            NodeContainer slice1;
            for(uint32_t c=0; c<n*n; c++)
            {
                NodeContainer oncore;
                oncore.Create(1);
                slice1.Add(oncore);
            }

            // slice1.Create(n*n);
            stack.Install(slice1);

            CONFIG::host_link_speed_long=1000000000*(1+(1-CONFIG::LINKTRADOFF));

            for(uint32_t i=0; i<slice1.GetN(); i++)
            {
                //down
                PointToPointHelper down;
                down.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
                if(i+n<slice1.GetN())
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(down.Install(slice1.Get(i),slice1.Get(i+n)));
                        //  std::cout<<i<<" "<<i+n<<std::endl;
                    }

                }
                else
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(down.Install(slice1.Get(i),slice1.Get(i%n)));
                        //  std::cout<<i<<" "<<i%n<<std::endl;
                    }
                }
                //right
                PointToPointHelper rig;
                rig.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
                if(i%n==(n-1))
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(rig.Install(slice1.Get(i),slice1.Get(i-n+1)));
                        // std::cout<<i<<" "<<i-n+1<<std::endl;
                    }
                }
                else
                {
                    const char * ip=CamCube::GetIP();
                    struct sockaddr_in sa;
                    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    while(result!=1)
                    {
                        ip= GetIP();
                        result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                    }
                    if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                    {
                        address.SetBase(ip,"255.255.255.0");
                        address.Assign(rig.Install(slice1.Get(i),slice1.Get(i+1)));
                        //  std::cout<<i<<" "<<i+1<<std::endl;
                    }
                }
            }
            host.Add(slice1);
        }
        //forward
        for(uint32_t i=0; i<n*n*n; i++)
        {
            PointToPointHelper forw;

            forw.SetDeviceAttribute("DataRate",  DataRateValue (CONFIG::host_link_speed_long));
            if(i+n*n<n*n*n)
            {
                const char * ip=CamCube::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                {
                    address.SetBase(ip,"255.255.255.0");
                    address.Assign(forw.Install(host.Get(i),host.Get(i+n*n)));
                    // std::cout<<i<<" "<<i+n*n<<std::endl;
                }
            }
            else
            {
                const char * ip=CamCube::GetIP();
                struct sockaddr_in sa;
                int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                while(result!=1)
                {
                    ip= GetIP();
                    result = inet_pton(AF_INET, ip, &(sa.sin_addr));
                }
                if(rand()/RAND_MAX<=CONFIG::LINKTRADOFF)
                {
                    address.SetBase(ip,"255.255.255.0");
                    address.Assign(forw.Install(host.Get(i),host.Get(i%(n*n))));
                    //  std::cout<<i<<" "<<i%(n*n)<<std::endl;
                }
            }
        }
    }


    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue(true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}
