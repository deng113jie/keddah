#ifndef KMEANS_H
#define KMEANS_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"

using namespace ns3;

class Kmeans
{
public:
    Kmeans();
    virtual ~Kmeans();
    int jobid;
    long filesize;

    int n_cluster;  //number of clusters
    int n_sample; //sample per cluster
    int n_dimension; //dimension per sample
    int rec_n;  //number of recursions

    Kmeans(int j,int n_c,int n_s, int n_d,int r);

    int starttime;
    void SetStartTime(int s);
    NodeContainer nodes;
    void SetNodeContainer(NodeContainer n);

    long totalvolume;
    long SetTotalVolume();


    double hdfs_r_volume;
    void SetHDFS_r_Volume();

    int totalnodes;
    int SetTotalNodes();
    uint32_t hdfs_rs_nodes; //HDFS read, send  rs
    void SetHDFS_rs_nodes();
    uint32_t hdfs_rr_nodes; //HDFS read, receive rr
    void SetHDFS_rr_nodes();

    uint32_t hdfs_ws_nodes;  //HDFS write, send ws
    void SetHDFS_ws_nodes();
    uint32_t hdfs_wr_nodes;  //HDFS write, receive wr
    void SetHDFS_wr_nodes();


    uint32_t hdfs_r_sess;
    void Sethdfs_r_sessions();
    uint32_t hdfs_w_sess;
    void SetHDFS_w_sess();

    double RatioPerNode(int num);

    double hdfs_stime_mean;
    double hdfs_stime_var;
    int HDFS_starttime();

    int Shuffle_starttime();

    void Setup();
    void SetupOnRec(int rec);
    long SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action);
    void Receive(Ptr<Node> to, int port, double action);


};

#endif // KMEANS_H
