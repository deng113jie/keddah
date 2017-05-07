#include "TeraSort.h"
#include "HadoopJob.h"
#include "JobPara.h"
#include "MyApp.h"
#include "CONFIG.h"
#include "YarnScheduler.h"
#include "HadoopYarn.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include <set>
#include <math.h>
#include <unistd.h>
#include <list>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TeraSort");

TeraSort::TeraSort()
{
    // ctor
}
TeraSort::TeraSort(int j, long f) : HadoopJob(j)
{

    filesize = f;
    finished_mappers = 0;
    finished_reducers = 0;
    finished_hdfswrite = 0;
    NumReplicas = 3;
    job_finished = 0;
}

TeraSort::~TeraSort()
{
    // dtor
}
void TeraSort::SetStartTime(int s)
{
    start_time = s;
}

int TeraSort::hdfs_r_nofcontainers() // that will generate traffic
{
    double mean = 28.44;
    double sd = 9.6;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_r_nofsrcpercont() // 1
{
    double mean = 1.2;
    double sd = 0.01;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_r_nofflowspersrc() // 1
{
    double mean = 1.22;
    double sd = 0.61;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_r_sizeofflow() // 128mb
{
    double mean = 128;
    double sd = 0.1;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_r_starttimeofflow() // in ms since start
{
    double mean = 287.39;
    double sd = 312.42;
    return JobPara::GetNormalRamdonVariableL1(mean, sd) / 1000; // in second
}

int TeraSort::shuffle_nofcontainers()
{
    double mean = 1.85;
    double sd = 1.06;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::shuffle_nofsrcpercont()
{
    double mean = 14.0;
    double sd = 0.46;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::shuffle_nofflowspersrc()
{
    double mean = 2.65;
    double sd = 2.14;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::shuffle_sizeofflow()
{
    double mean = 22.24;
    double sd = 12.12;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::shuffle_starttimeofflow()
{
    double mean = 295;
    double sd = 304;
    return JobPara::GetNormalRamdonVariableL1(mean, sd) / 1000; // in second
}

int TeraSort::hdfs_w_nofcontainers() // depends on the number of reducers and random factors (number of replications)
{
    return 1;
}
int TeraSort::hdfs_w_nofdstpercont()
{
    double mean = 13.31;
    double sd = 0.82;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_w_nofflowspersrc()
{
    double mean = 4.95;
    double sd = 3.43;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_w_sizeofflow() // 128m
{
    double mean = 118;
    double sd = 22;
    return JobPara::GetNormalRamdonVariableL1(mean, sd);
}
int TeraSort::hdfs_w_starttimeofflow()
{
    double mean = 346;
    double sd = 312;
    return JobPara::GetNormalRamdonVariableL1(mean, sd) / 1000; // in second
}

void TeraSort::Map_containers_thread() // this is one mapper container
{
    // TeraSort ts = *(this);

    // request resource
    int node = YarnScheduler::YarnScheduler_container_started();
    // std::cout<<node<<std::endl;
    Ptr<Node> to = HadoopYarn::getHosts().Get(node);
    // generating traffic?

    int src = this->hdfs_r_nofsrcpercont();
    // std::cout << "src " << src << "node " << node << std::endl;
    std::set<Ptr<MyApp> > apps;
    for (int i = 0; i < src; i++) // HDFS read from src to this node
    {
        int flows = this->hdfs_r_nofflowspersrc();
        // int fn = rand() % HadoopYarn::getHosts().GetN();
        // std::cout<<fn<<std::endl;
        Ptr<Node> from =
            HadoopYarn::getHosts().Get(rand() % HadoopYarn::getHosts().GetN()); // random a source to send the traffic
        // Ptr<Node> from = HadoopYarn::getHosts().Get(1);
        //	std::cout << "flows " << flows << std::endl;
        for (int j = 0; j < flows; j++) {
            Address sinkAddress(InetSocketAddress(to->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 50010));
            Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(from, TcpSocketFactory::GetTypeId());
            Ptr<MyApp> app = CreateObject<MyApp>();
            app->Setup(ns3TcpSocket,
                       sinkAddress,
                       1000,
                       this->hdfs_w_sizeofflow() * 1000,
                       DataRate(CONFIG::host_link_speed)); // as flow size is in mb
            from->AddApplication(app);
            int t = this->shuffle_starttimeofflow() + this->start_time;
         //   std::cout << "start time " << t << std::endl;
            app->SetStartTime(Seconds(t));
            apps.insert(app);
        }
    }
    // check if traffic has finished
    for (std::set<Ptr<MyApp> >::iterator it = apps.begin(); it != apps.end(); it++) {
        while (!((*it)->Finished())) {
            sleep(1); // sleep 1 second if traffic not finished
        }
    }

    // traffic done; recycle node
    this->finished_mappers++;
    this->mapperlocation.push_back(node);
    YarnScheduler::YarnScheduler_container_finished(node);
    std::cout << "Mappper " << this->finished_mappers << " on node " << node << " from job " << this->jobid
              << " finished at " << Simulator::Now().GetSeconds() << std::endl;
}

void TeraSort::Reducer_containers_thread()
{
    // TeraSort ts = *(this);
    // sending traffic
    int node = YarnScheduler::YarnScheduler_container_started();
    int src = this->shuffle_nofsrcpercont();
    Ptr<Node> to = HadoopYarn::getHosts().Get(node);
    std::cout << "Shuffle " << this->finished_reducers << " on node " << node << " from job " << this->jobid
              << "  started with " << src << " flows " << Simulator::Now().GetSeconds() << std::endl;
    std::set<Ptr<MyApp> > apps;
    for (int i = 0; i < src; i++) {
        int flows = this->shuffle_nofflowspersrc();
        Ptr<Node> from = HadoopYarn::getHosts().Get(this->mapperlocation.at(rand() % this->mapperlocation.size()));
        for (int j = 0; j < flows; j++) {
            Address sinkAddress(InetSocketAddress(to->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 13562));
            Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(from, TcpSocketFactory::GetTypeId());
            Ptr<MyApp> app = CreateObject<MyApp>();
            app->Setup(ns3TcpSocket,
                       sinkAddress,
                       1000,
                       this->shuffle_sizeofflow() * 1000,
                       DataRate(CONFIG::host_link_speed)); // as flow size is in mb
            from->AddApplication(app);
            app->SetStartTime(Seconds(this->shuffle_starttimeofflow() + this->start_time));
            apps.insert(app);
        }
    }

    // check traffic is over
    for (std::set<Ptr<MyApp> >::iterator it = apps.begin(); it != apps.end(); it++) {
        while (!((*it)->Finished())) {
            sleep(1); // sleep 1 second if traffic not finished
        }
    }
    this->finished_reducers++;
    std::cout << "Shuffle " << this->finished_reducers << " on node " << node << " from job " << this->jobid
              << " finished at " << Simulator::Now().GetSeconds() << std::endl;
    apps.clear();
    // hdfs write
    Ptr<Node> from = HadoopYarn::getHosts().Get(node);
    for (int i = 0; i < this->NumReplicas - 1; i++) { // one copy is local
        Ptr<Node> to = HadoopYarn::getHosts().Get(rand() % HadoopYarn::getHosts().GetN());
        int flows = this->hdfs_w_nofflowspersrc();
        std::cout << "HDFS write " << this->finished_reducers << " on node " << node << " from job " << this->jobid
                  << "  write to  " << to->GetId() << " fwith " << flows << " flows " << Simulator::Now().GetSeconds()
                  << std::endl;
        for (int j = 0; j < flows; j++) {
            Address sinkAddress(InetSocketAddress(to->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 50010));
            Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(from, TcpSocketFactory::GetTypeId());
            Ptr<MyApp> app = CreateObject<MyApp>();
            app->Setup(ns3TcpSocket,
                       sinkAddress,
                       1000,
                       this->hdfs_w_sizeofflow() * 1000,
                       DataRate(CONFIG::host_link_speed)); // as flow size is in mb
            from->AddApplication(app);
            app->SetStartTime(Seconds(this->hdfs_w_starttimeofflow() + this->start_time));
            apps.insert(app);
        }
        from = to;
    }
    // check traffic is over
    for (std::set<Ptr<MyApp> >::iterator it = apps.begin(); it != apps.end(); it++) {
        while (!((*it)->Finished())) {
            sleep(1); // sleep 1 second if traffic not finished
        }
    }
    this->finished_hdfswrite++;
    std::cout << "HDFS write " << this->finished_hdfswrite << " on node " << node << " from job " << this->jobid
              << " finished at " << Simulator::Now().GetSeconds() << std::endl;
}

void TeraSort::start()
{

    std::cout << "Job TeraSort strted " << this->jobid << " " << Simulator::Now().GetSeconds() << std::endl;

    int amnode = YarnScheduler::YarnScheduler_job_started(this->jobid);
    // hdfs read

    int total_mapper = 8;  // hdfs_r_nofcontainers();
    int total_reducer = 2; // shuffle_nofcontainers();

    int reducerstarted = 0;
    std::list<Ptr<SystemThread> > mapperthrds(total_mapper);
    std::list<Ptr<SystemThread> > reducerthrds(total_reducer);

    for (int i = 0; i < total_mapper; i++) {

        // start mapper container
        Ptr<SystemThread> mapper = Create<SystemThread>(MakeCallback(&TeraSort::Map_containers_thread, this));
        sleep(1); // don't flush threads together
        mapperthrds.push_front(mapper);
        mapper->Start();

        std::cout << "Mapper " << i << " from job " << this->jobid << " started " << Simulator::Now().GetSeconds()
                  << std::endl;
        if (reducerstarted <= total_reducer &&
            ((float)this->finished_mappers) / total_mapper >= CONFIG::REDUCERSTARTRATE) {
            // start reducer container

            for (; reducerstarted < total_reducer; reducerstarted++) {

                Ptr<SystemThread> reducer =
                    Create<SystemThread>(MakeCallback(&TeraSort::Reducer_containers_thread, this));
                sleep(1); // don't flush threads together
                reducerthrds.push_front(reducer);
                reducer->Start();

                std::cout << "Reducer " << reducerstarted << " from job " << this->jobid << " started "
                          << Simulator::Now().GetSeconds() << std::endl;
            }
        }
    }
    // mapper finished quite late, that's some reducers may start here
    // for (int i = 0; (float)(i-1) / total_mapper < CONFIG::REDUCERSTARTRATE; i++) {
    int t = (int)(total_mapper * CONFIG::REDUCERSTARTRATE);
    for (int i = 0; i < t; i++) {
    //    std::cout << "map stage 1 " << i << std::endl;
        mapperthrds.front()->Join();
        mapperthrds.pop_front();
    }
    //    while (((float)(this->finished_mappers)) / total_mapper < CONFIG::REDUCERSTARTRATE) {
    //      sleep(1);
    //   std::cout<< ((float) (this->finished_mappers)) / total_mapper <<"  "<<Simulator::Now().GetSeconds
    // ()<<std::endl;
    //  }
    for (; reducerstarted < total_reducer; reducerstarted++) {

        Ptr<SystemThread> reducer =
            Create<SystemThread>(MakeCallback(&TeraSort::Reducer_containers_thread, this));
        sleep(1); // don't flush threads together
        reducerthrds.push_front(reducer);
        reducer->Start();
        std::cout << "Reducer " << reducerstarted << " from job " << this->jobid << " started "
                  << Simulator::Now().GetSeconds() << std::endl;
    }

    for (int i = t; i < total_mapper; i++) {
     //   std::cout << "map stage 2 " << i << std::endl;
        mapperthrds.front()->Join();
        mapperthrds.pop_front();
    }
    for (int i = 0; i < total_reducer; i++) {
     //   std::cout << "reduce stage " << i << std::endl;
        reducerthrds.front()->Join();
        reducerthrds.pop_front();
    }

    YarnScheduler::YarnScheduler_job_finished(this->jobid, amnode);
    job_finished = 1;
}

int TeraSort::is_job_finished()
{
    return job_finished;
}
