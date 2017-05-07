#ifndef CAMCUBE_H
#define CAMCUBE_H

#include "DCTopology.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"

using namespace ns3;


class CamCube : public DCTopology
{
    public:
        CamCube(uint32_t n);
        virtual ~CamCube();
        void Setup();
        void Setup(int ncore);

        uint32_t n;

        InternetStackHelper stack;
        Ipv4AddressHelper address;

        int a;
        int b;
        int c;
        const char *  GetIP();
};

#endif // CAMCUBE_H
