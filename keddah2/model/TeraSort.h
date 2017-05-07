#ifndef TERASORT_H
#define TERASORT_H

#include "HadoopJob.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"


using namespace ns3;

class TeraSort  : public HadoopJob
{
public:
    TeraSort();
 
    long filesize;

    TeraSort(int j, long f);
    virtual ~TeraSort();
    void SetStartTime(int s);

    //hdfs input
    int hdfs_r_nofcontainers();
    int hdfs_r_nofsrcpercont();
    int hdfs_r_nofflowspersrc();
    int hdfs_r_sizeofflow();
    int hdfs_r_starttimeofflow();
    
    int shuffle_nofcontainers();
    int shuffle_nofsrcpercont();
    int shuffle_nofflowspersrc();
    int shuffle_sizeofflow();
    int shuffle_starttimeofflow();
    
    int hdfs_w_nofcontainers();
    int hdfs_w_nofdstpercont();
    int hdfs_w_nofflowspersrc();
    int hdfs_w_sizeofflow();
    int hdfs_w_starttimeofflow();

    void start();
    void Map_containers_thread();
    void Reducer_containers_thread();
    int is_job_finished();

 //  long SendTo(Ptr<Node> from , Ptr<Node> to,  int port , uint32_t bytes, double action);
 //   void Receive(Ptr<Node> to, int port, double action);

};

#endif // TERASORT_H
