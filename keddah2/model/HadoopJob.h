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
 
#include "MyApp.h"
#include "CONFIG.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <vector>

using namespace ns3;

class HadoopJob : public ns3::Object
{
public:
    HadoopJob();
    HadoopJob(int j);

    NodeContainer nodes;
    int totalnodes;

    int jobid;
    int start_time;

    std::vector<int> mapperlocation;
    int total_reducer;
    int NumReplicas;
    int finished_mappers; // for when to start reducer
    int finished_reducers;
    int finished_hdfswrite;
    int job_finished;


    static Ptr<MyApp> StartSendTraffic(Ptr<Node> from, Ptr<Node> to, int port, int size, int time)
    {
        Address sinkAddress(InetSocketAddress(to->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port));
        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(from, UdpSocketFactory::GetTypeId());
        Ptr<MyApp> app = CreateObject<MyApp>();
        app->Setup(
            ns3TcpSocket, sinkAddress, 1000, size * 1000, DataRate(CONFIG::host_link_speed)); // as flow size is in mb
        from->AddApplication(app);
        app->SetStartTime(Seconds(time));
        app->SetStopTime(Seconds(time+100));
        return app;
    }
};

#endif // HADOOPJOB_H
