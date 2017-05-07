#ifndef YARNSCHEDULER_H
#define YARNSCHEDULER_H

#include "CONFIG.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/node.h"

#include <vector>

using namespace ns3;



class YarnScheduler
{
public:
    YarnScheduler(int n);
    ~YarnScheduler();
    // return node id for running the ApplicationMaster
    static int YarnScheduler_job_started(int jobid)
    {
    	//NS_LOG_INFO("job " << jobid << " started");
        
        return YarnScheduler_container_started();
    }
    // collected the AM resource
    static int YarnScheduler_job_finished(int jobid,int amnode)
    {
        YarnScheduler_container_finished(amnode);
     //   NS_LOG_INFO("job " << jobid << " finished ");
        return 1;
    }
    // return node if for running the container
    static int YarnScheduler_container_started()
    {

        int node = 0;
        while (node == 0) { // waiting for resource to refresh
            // giving a node
            for (std::vector< std::pair<int, int> >::reverse_iterator itc = cpuleft.rbegin();itc != cpuleft.rend(); itc++) {
                if (node != 0) {
                    break;
                } 
                if (itc->first >= CONFIG::CONTAINERCORE) {
                    // check memory
                    for (std::vector< std::pair<float, int> >::reverse_iterator itm = memoryleft.rbegin();
                         itm != memoryleft.rend();
                         itm++) {
                        if (itc->second == itm->second && itm->first >= CONFIG::CONTAINERMEMORY) {
                            node = itc->second;
                            itc->first -= CONFIG::CONTAINERCORE;
                            itm->first -= CONFIG::CONTAINERMEMORY;
                            break;
                        }
                    }
                }
            }
        }
        // sorting
        std::sort(cpuleft.begin(), cpuleft.end());
        std::sort(memoryleft.begin(), memoryleft.end());
        return node;
    }
    static int YarnScheduler_container_finished(int node)
    {
        // collect the node; second value is the node id
        for (std::vector< std::pair<int, int> >::iterator it = cpuleft.begin(); it != cpuleft.end(); it++) {
            if (it->second == node) {
                it->first += CONFIG::CONTAINERCORE;
                break;
            }
        }
        for (std::vector< std::pair<float, int> >::iterator it = memoryleft.begin(); it != memoryleft.end(); it++) {
            if (it->second == node) {
                it->first += CONFIG::CONTAINERMEMORY;
                break;
            }
        }
        // sorting
        std::sort(cpuleft.begin(), cpuleft.end());
        std::sort(memoryleft.begin(), memoryleft.end());
        return 1;
    }

    /* use vector of pair for sorting purposes
     */
    static std::vector< std::pair<int, int> > cpuleft;      //  cpuleft, nodeid
    static std::vector< std::pair<float, int> > memoryleft; // momeryleft, nodeid
};

#endif // YARNSCHEDULER_H
