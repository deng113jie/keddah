# keddah - Hadoop Traffic Generator

Installation:

1, install ns3 https://www.nsnam.org/wiki/Installation
2, create module with name dctest: ns-allinone-3.23/ns-3.23/src$ ./create-module.py dctest
3, ./waf configure
4, download code, put folder dctest into ns-allinone-3.23/ns-3.23/src
5, ./waf configure && ./waf
6, download code, put DisCompSimulator.cc into ns-allinone-3.23/ns-3.23/scratch
7, ./waf 

Run:
NS_LOG='TeraSort:Kmeans' ./waf --run "scratch/DisCompSimulator --tracejob=500 --stop_time=1300 --network=1 --job_rate=6 --ftnodnum=$i"

    --filesize:   Size of data file need to be sort. [3000000000]
    --tracejob:   Which job to trace. [100]
    --network:    The type of network: (0 star) (1 fattree) (2 dcell) (3 camcube) [0]
    --stop_time:  Number of seconds to run the simulation [1200]
    --job_rate:   Exponential mean of job submit interval [20]
    --linktrad:   the trade-off rate of link and bandwidth in cam cube [1]
    --ftnodnum:   number of nodes per pod in FatTree [8]
