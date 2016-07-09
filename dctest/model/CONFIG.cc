#include "CONFIG.h"
#include <string>
#include "ns3/core-module.h"


using namespace ns3;

const std::string CONFIG::host_link_speed="1000Mbps";
const std::string CONFIG::trunk_link_speed="1000Mbps";

int CONFIG::TRACE_JOB=100;
double CONFIG::SIM_STOP=1200;
double CONFIG::JOB_RATE=20;

double CONFIG::LINKTRADOFF=1;

long CONFIG::host_link_speed_long=1000000000;


