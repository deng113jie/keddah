#include "Cluster.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

Cluster::Cluster()
{
    //ctor

}

Cluster::Cluster(DCTopology t):
    topo(t)
{

}

Cluster::Cluster(FatTree t):
    topo(t)
{

}

Cluster::Cluster(DCell d):
    topo(d)
{

}
Cluster::Cluster(CamCube c):
    topo(c)
{

}
Cluster::Cluster(StarTopo s):
    topo(s)
{

}

Cluster::~Cluster()
{
    //dtor
}

NodeContainer
Cluster::getHosts()
{
    return topo.GetHosts();
}

