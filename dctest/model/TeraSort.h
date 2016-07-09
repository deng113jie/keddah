#ifndef TERASORT_H
#define TERASORT_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"

using namespace ns3;

class TeraSort
{
public:
    TeraSort();
    int jobid;
    long filesize;
    int  clusize;
    TeraSort(int j, long f,int c);
    virtual ~TeraSort();
    int starttime;
    void SetStartTime(int s);
    NodeContainer nodes;
    void SetNodeContainer(NodeContainer n);



    long totalvolume;
    long SetTotalVolume();

    double hdfsvolume;
    void SetHDFS_Volume();

    double shufvolume;
    void SetShuffle_Volume();

    int totalnodes;
    int SetTotalNodes();
    uint32_t hdfs_s_nodes;
    void SetHDFS_send_nodes();
    uint32_t hdfs_r_nodes;
    void SetHDFS_recv_nodes();
    uint32_t shuf_s_nodes;
    void SetShuffle_send_nodes();
    uint32_t shuf_r_nodes;
    void SetShuffle_recv_nodes();

    uint32_t hdfs_sess;
    void SetHDFS_sessions();
    uint32_t shuf_sess;
    void SetShuf_sessions();

    double RatioPerNode(int num);

   // double hdfs_stime_mean;
  //  double hdfs_stime_var;
    int HDFS_starttime();

    int Shuffle_starttime();

    void Setup();
   long SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action);
    void Receive(Ptr<Node> to, int port, double action);


};

#endif // TERASORT_H
