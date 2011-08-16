
#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/NamedParameter.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/IReturnReal.h"
#include "Mint/counted_ptr.h"

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

  DalitzEventList eventList, eventListWeighted;
  SignalGenerator sg(pdg);
  SignalGenerator sgWeighted(pdg);
  sgWeighted.setWeighted();

  FromFileGenerator ffg("events.root", &sg);

  counted_ptr<IReturnReal> mp3Ptr(new mumsMomentum(meanMomentum));
  ffg.setMothers3Momentum(mp3Ptr);

  ffg.FillEventList(eventList, Nevents);

  sgWeighted.FillEventList(eventListWeighted, 10*Nevents);

  DalitzHistoSet datH = eventList.histoSet();
  eventList.save("eventList.root");
  datH.save("plotsFromEventList.root");

  DalitzHistoSet datHWeighted = eventListWeighted.histoSet();
  eventListWeighted.save("eventListWeighted.root");
  datHWeighted.save("plotsFromEventListWeighted.root");

  datHWeighted.drawWithFit(datH, "plots_");

  cout << "testGenerator done." << endl;
  return 0;
}
int weightCheck(){
  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 321, -211, 211);
  NamedParameter<double> meanMomentum("Mean Momentum", 10.0);
  DalitzEventPattern pdg(EventPattern.getVector());

  DalitzEventList eventListWeighted;

  SignalGenerator sgWeighted(pdg);
  sgWeighted.setWeighted();

  sgWeighted.FillEventList(eventListWeighted, Nevents);

  DalitzHistoSet datH         = eventListWeighted.histoSet();
  DalitzHistoSet datHWeighted = eventListWeighted.weightedHistoSet();
  eventListWeighted.save("eventListWeighted.root");
  datH.save("plotsFromEventList.root");
  datHWeighted.save("plotsFromEventListWeighted.root");

  datHWeighted.drawWithFitNorm(datH, "plots_");

  DalitzHistoSet ratio(datHWeighted/datH);
  ratio.draw("ratioHistos");

  cout << "testGenerator done." << endl;
  return 0;
}

int main(){
  return weightCheck();
  //  return testGenerator();
}
