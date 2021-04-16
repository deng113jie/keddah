# keddah - Hadoop Network Traffic Generator in NS3

When you run network simulation in ns3, the traffic workload is is key to evaluate the network performance. Using Keddah, you can replay network traffic that extracted from a Hadoop cluster. For details, please refer to paper:

<br />@article{deng2019keddah,
 <br /> title={Keddah: network evaluation powered by simulating distributed application traffic},
<br />  author={Deng, Jie and Tyson, Gareth and Cuadrado, Felix and Uhlig, Steve},
<br />  journal={ACM Transactions on Modeling and Computer Simulation (TOMACS)},
<br />  volume={29},
<br />  number={3},
<br />  pages={1--25},
 <br /> year={2019},
 <br /> publisher={ACM New York, NY, USA}
<br />}

To use Keddah: 
<br />1, install ns3 https://www.nsnam.org/wiki/Installation 
<br />2, create module with name dctest without the code:   ns-allinone-3.23/ns-3.23/src$ ./create-module.py dctest
<br />3, configure the module： ./waf configure 
<br />4, download Keddah code, put folder dctest into ns-allinone-3.23/ns-3.23/src , replace the folder
<br />5, compile the module again: ./waf configure && ./waf
<br />6, download the experiment DisCompSimulator.cc into ns-allinone-3.23/ns-3.23/scratch
<br />7, compile again: ./waf 

<br />Run:
<br />NS_LOG='TeraSort:Kmeans' ./waf --run "scratch/DisCompSimulator --tracejob=500 --stop_time=1300 --network=1 --job_rate=6 --ftnodnum=20"

    --filesize:   Size of data file need to be sort. [3000000000]
    --tracejob:   Which job to trace. [100]
    --network:    The type of network: (0 star) (1 fattree) (2 dcell) (3 camcube) [0]
    --stop_time:  Number of seconds to run the simulation [1200]
    --job_rate:   Exponential mean of job submit interval [20]
    --linktrad:   the trade-off rate of link and bandwidth in cam cube [1]
    --ftnodnum:   number of nodes per pod in FatTree [8]
