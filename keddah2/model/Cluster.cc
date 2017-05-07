#include "Cluster.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

DCTopology Cluster::topo;
Cluster::Cluster()
{
    // ctor
}

Cluster::Cluster(DCTopology t)
{
    Cluster::topo = t;
}

Cluster::Cluster(FatTree t)
{
    Cluster::topo = t;
}

Cluster::Cluster(DCell d)
{
    Cluster::topo = d;
}
Cluster::Cluster(CamCube c)
{
    Cluster::topo = c;
}
Cluster::Cluster(StarTopo s)
{
    Cluster::topo = s;
}

Cluster::~Cluster()
{
    // dtor
}
