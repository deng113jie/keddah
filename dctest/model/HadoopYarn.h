#ifndef HADOOPYARN_H
#define HADOOPYARN_H

#include "Cluster.h"

using namespace ns3;

class HadoopYarn
{
    public:
        HadoopYarn();
        HadoopYarn(Cluster cst);
        virtual ~HadoopYarn();

        void StartJob(long file,int total, double exp_mean);
        Cluster cst;
        static int job_id;

//        static int trace_job_id;
//        static void SetTraceJob(int i);
//        static int GetTraceJob();
};

#endif // HADOOPYARN_H
