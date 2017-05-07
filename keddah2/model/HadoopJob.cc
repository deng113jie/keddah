#include "HadoopJob.h"
#include "JobPara.h"
#include "MyApp.h"
#include "CONFIG.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"

#include <math.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HadoopJob");

HadoopJob::HadoopJob()
{
    
}

HadoopJob::HadoopJob(int j)
{
     jobid=j;
}
 