#ifndef DCTOPOLOGY_H
#define DCTOPOLOGY_H
#include "ns3/core-module.h"
#include "ns3/network-module.h"

using namespace ns3;

class DCTopology
{
    public:
        DCTopology();
        virtual ~DCTopology();
   NodeContainer GetHosts();
   NodeContainer host;
};

#endif // DCTOPOLOGY_H
