#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "ns3/core-module.h"
#include <string>

using namespace ns3;

class CONFIG
{
public:

    static const double stop_time=10.;
    static const double start_time=0.;
    static const int cores_per_host=2;
    static const long hdfs_block_size=128000000;
    static const int convergence_factor=4; //the factor for HDFS nodes to convergence
    static const bool TRACE_BACK=true;

    static const std::string host_link_speed ;
    static long host_link_speed_long ;
    static const int csma_delay=6560;
    static const std::string trunk_link_speed ;
    static const int trunk=2000;

    static int TRACE_JOB;
    static double SIM_STOP;

    static double JOB_RATE;

    static double LINKTRADOFF;

//    static const char * GetIP();





};



#endif // CONFIG_H_INCLUDED
