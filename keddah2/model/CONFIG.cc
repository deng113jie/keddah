#include "CONFIG.h"
#include <string>
#include "ns3/core-module.h"


using namespace ns3;

int CONFIG::TRACE_JOB = 1;
double CONFIG::SIM_STOP=3600;
double CONFIG::JOB_RATE=20;

 const double CONFIG::stop_time=10.;
const double CONFIG::start_time=0.;
const long CONFIG::hdfs_block_size=128000000;
const int CONFIG::convergence_factor=4; //the factor for HDFS nodes to convergence
const bool CONFIG::TRACE_BACK=true;

const std::string CONFIG::host_link_speed="1000Mbps" ;
 long CONFIG::host_link_speed_long=1000000000;
const int CONFIG::csma_delay=6560;
const std::string CONFIG::trunk_link_speed ="1000Mbps";
const int CONFIG::trunk=2000;

double CONFIG::LINKTRADOFF=1;

const float CONFIG::NODEMEMORY=16; //in gb
const float CONFIG::CONTAINERMEMORY=1;
const float CONFIG::REDUCERSTARTRATE=0.75;

const int CONFIG::NODECORES=8;
const int  CONFIG::CONTAINERCORE=1;
     
         
