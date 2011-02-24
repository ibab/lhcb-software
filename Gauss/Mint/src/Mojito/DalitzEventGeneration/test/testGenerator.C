
#include "SignalGenerator.h"
#include "FromFileGenerator.h"

#include "NamedParameter.h"
#include "DalitzEventPattern.h"
#include "DalitzEventList.h"
#include "DalitzHistoSet.h"
#include "IReturnReal.h"
#include "counted_ptr.h"

#include "TRandom.h"

#include <iostream>

using namespace MINT;
using namespace std;

class mumsMomentum : virtual public IReturnReal{
  double _mean;
public:
  mumsMomentum(double m) : _mean(m){}

  virtual double RealVal(){
    return gRandom->Exp(_mean);
  }
};

int testGenerator(){
  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 321, -211, 211);
  NamedParameter<double> meanMomentum("Mean Momentum", 10.0);
  DalitzEventPattern pdg(EventPattern.getVector());

  DalitzEventList eventList;
  SignalGenerator sg(pdg);
  FromFileGenerator ffg("events.root", &sg);

  counted_ptr<IReturnReal> mp3Ptr(new mumsMomentum(meanMomentum));
  ffg.setMothers3Momentum(mp3Ptr);

  ffg.FillEventList(eventList, Nevents);

  DalitzHistoSet datH = eventList.histoSet();
  eventList.save("eventList.root");
  datH.save("plotsFromEventList.root");

  cout << "testGenerator done." << endl;
  return 0;
}

int main(){
  return testGenerator();
}
