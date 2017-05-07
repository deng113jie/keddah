#include "YarnScheduler.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("YarnScheduler");

std::vector< std::pair<int, int> > YarnScheduler::cpuleft;      //  cpuleft, nodeid
std::vector< std::pair<float, int> > YarnScheduler::memoryleft; // momeryleft, nodeid
    
YarnScheduler::YarnScheduler(int n)
{
    for (int i = 0; i < n; i++) {
    	YarnScheduler::cpuleft.push_back(std::make_pair(CONFIG::NODECORES, i));
    	YarnScheduler::memoryleft.push_back(std::make_pair(CONFIG::NODEMEMORY, i));
    }
}

YarnScheduler::~YarnScheduler()
{
}
