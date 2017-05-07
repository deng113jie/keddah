#ifndef HADOOPYARN_H
#define HADOOPYARN_H

#include "Cluster.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

class HadoopYarn
{
    public:
        HadoopYarn();
        HadoopYarn(Cluster cst);
        virtual ~HadoopYarn();

        std::set< Ptr<SystemThread> > StartJob(long file,int total, double exp_mean);
        void StartRecvTraffic();
        static Cluster cst;
        static int job_id;
        
        static NodeContainer getHosts(){
            return cst.getHosts();
        }

//        static int trace_job_id;
//        static void SetTraceJob(int i);
//        static int GetTraceJob();
};

#endif // HADOOPYARN_H
