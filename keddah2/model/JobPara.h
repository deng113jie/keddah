#ifndef JOBPARA_H
#define JOBPARA_H

#include "ns3/random-variable-stream.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class JobPara
{
public:
    JobPara();
    virtual ~JobPara();

    static Ptr<NormalRandomVariable> volumerv ;
    static double GetNormalRamdonVariableL1(double mean,double variance)
    {

        volumerv = CreateObject<NormalRandomVariable> ();
        volumerv->SetAttribute ("Mean", DoubleValue (mean));
        volumerv->SetAttribute ("Variance", DoubleValue (variance));

        double r=volumerv->GetValue();
        while(r<1)
        {
            r=volumerv->GetValue();
        }

        return r;
    }
    static double GetNormalRamdonVariableL1S1(double mean,double variance)
    {

        volumerv = CreateObject<NormalRandomVariable> ();
        volumerv->SetAttribute ("Mean", DoubleValue (mean));
        volumerv->SetAttribute ("Variance", DoubleValue (variance));

        double r=volumerv->GetValue();
        while(r<0 || r>1)
        {
            r=volumerv->GetValue();
        }
        return r;
    }
};

#endif // JOBPARA_H
