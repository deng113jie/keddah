#ifndef HADOOPJOB_H
#define HADOOPJOB_H

/** \brief Simulate one Hadoop TeraSort job traffic.
    Traffic volume: determined by data file size.
    Packet interval:
    Packet size:
    Nodes involved: number_of_mapper/block_size(128M)/cores_on_node

 * \param start_time;
 * \param datafilesize: size of the data file need to be sort.
 * \param nodes: nodes that the packet will be transmitted to/from.
 * \return
 *
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <map>



using namespace ns3;

class HadoopJob
{
public:
    HadoopJob();
    HadoopJob(int j, float startTime, long datafilesize, NodeContainer n);
    virtual ~HadoopJob();
    long NodeSendTraffic();
    float NodeReceiveRatio();
    void setup();
   // void CreateSessions(int send_n, int rece_n, int sess_n);
    long SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action);
    void Receive(Ptr<Node> to, int port, double action);


    int job_id;
    long datafilesize;
    int start_time;
    const static double duration=20.;
    NodeContainer nodes;

    std::map<int,long> sendtraffic ;
    std::map<int,float> receiveratio ;
};



#endif // HADOOPJOB_H
