#ifndef DCELL_H
#define DCELL_H

#include "DCTopology.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class DCell : public DCTopology
{
public:
    DCell(int k,int n);
    virtual ~DCell();
    void setup();
    void setup(int corenum);

    int Cells_at_k(int k,int n);
    int n_of_server(int k,int n);
    int k;
    int n;

    //hosts is in DCTopology
    //  NodeContainer swes;
    // NetDeviceContainer devices;
    InternetStackHelper stack;
    Ipv4AddressHelper address;

    int a;
    int b;
    int c;
    const char *  GetIP();

};

#endif // DCELL_H
