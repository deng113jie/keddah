#ifndef FATTREE_H
#define FATTREE_H

#include "DCTopology.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class FatTree : public DCTopology
{

    public:
        FatTree();
        FatTree(uint32_t k, uint32_t n);
        FatTree(uint32_t k, uint32_t n,int corenum);
        virtual ~FatTree();
        void setuplink();


    uint32_t k; //pod number
    uint32_t n; //host per pod
    NodeContainer edge;
    NodeContainer aggre;
    NodeContainer core;
    Ipv4AddressHelper address;

     int a;
        int b;
        int c;
        const char *  GetIP();

};

#endif // FATTREE_H
