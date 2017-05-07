#ifndef KMEANS_H
#define KMEANS_H

#include "HadoopJob.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"

using namespace ns3;

class Kmeans: public HadoopJob {
public:
	Kmeans();
	Kmeans(int j, int n_c, int n_s, int n_d, int r);
	virtual ~Kmeans();

	long filesize;

	int n_cluster;  //number of clusters
	int n_sample; //sample per cluster
	int n_dimension; //dimension per sample
	int rec_n;  //number of recursions

	void SetStartTime(int s);

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

};

#endif // KMEANS_H
