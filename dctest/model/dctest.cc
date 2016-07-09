/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "dctest.h"

using namespace ns3;
using namespace std;

void print_stats(FlowMonitor::FlowStats st)
{
    cout << " Tx Bytes : " << st.txBytes << endl;
    cout << " Rx Bytes : " << st.rxBytes << endl;
    cout << " Tx Packets : " << st.txPackets << endl;
    cout << " Rx Packets : " << st.rxPackets << endl;
    cout << " Lost Packets : " << st.lostPackets << endl;

    if( st.rxPackets > 0) {
         cout << " Mean{Delay}: " << (st.delaySum.GetSeconds() / st.rxPackets);
	 cout << " Mean{jitter}: " << (st.jitterSum.GetSeconds() / (st.rxPackets - 1));
	 cout << " mean{Hop Count}: " << st.timesForwarded / st.rxPackets + 1;
    }
    if(true) {
           cout << "Delay Histogram :" << endl;
	   for(uint32_t i=0; i < st.delayHistogram.GetNBins (); i++)
	        cout << " " << i << "(" << st.delayHistogram.GetBinStart(i) << st.delayHistogram.GetBinEnd(i) << ")" << st.delayHistogram.GetBinCount(i) << endl;
	}
}

