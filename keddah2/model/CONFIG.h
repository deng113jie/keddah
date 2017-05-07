#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "ns3/core-module.h"
#include <string>

using namespace ns3;

class CONFIG
{
public:

    static const double stop_time;
    static const double start_time;
    static const long hdfs_block_size;
    static const int convergence_factor; //the factor for HDFS nodes to convergence
    static const bool TRACE_BACK;

    static const std::string host_link_speed ;
    static long host_link_speed_long ;
    static const int csma_delay;
    static const std::string trunk_link_speed ;
    static const int trunk;

    static int TRACE_JOB;
    static double SIM_STOP;

    static double JOB_RATE;

    static double LINKTRADOFF;
    
    
    static const int NODECORES;
    static const float NODEMEMORY; //in gb
    static const int CONTAINERCORE;
    static const float CONTAINERMEMORY;
    static const float REDUCERSTARTRATE;
 

//    static const char * GetIP();





};



#endif // CONFIG_H_INCLUDED
