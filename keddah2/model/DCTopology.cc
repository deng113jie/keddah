#include "DCTopology.h"


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
using namespace ns3;

DCTopology::DCTopology()
{
    //ctor
}

DCTopology::~DCTopology()
{
    //dtor
}

NodeContainer
DCTopology::GetHosts()
{
    return host;
}
