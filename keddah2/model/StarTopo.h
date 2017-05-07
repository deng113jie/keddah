#ifndef STARTOPO_H
#define STARTOPO_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "DCTopology.h"

using namespace ns3;


class StarTopo : public DCTopology
{
    public:
        StarTopo(int n);
        virtual ~StarTopo();
        StarTopo(int n,int corenum);

    NetDeviceContainer devices;
};

#endif // STARTOPO_H
