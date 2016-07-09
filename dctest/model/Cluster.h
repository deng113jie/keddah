#ifndef CLUSTER_H
#define CLUSTER_H

#include "DCTopology.h"
#include "FatTree.h"
#include "DCell.h"
#include "CamCube.h"
#include "StarTopo.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;


class Cluster
{
    public:
        Cluster();
        Cluster(DCTopology t);
        Cluster(FatTree fattree);
        Cluster(DCell dcell);
        Cluster(CamCube dcell);
        Cluster(StarTopo star);

        virtual ~Cluster();

        NodeContainer getHosts();

        DCTopology topo;
};

#endif // CLUSTER_H
