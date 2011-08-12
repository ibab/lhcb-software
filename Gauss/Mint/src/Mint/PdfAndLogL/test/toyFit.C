// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "FitParameter.h"
#include "NamedParameter.h"
#include "Minimiser.h"
#include "Neg2LL.h"
#include "EventList.h"

#include "PdfBase.h"

#include "TRandom.h"

#include <iostream>

using namespace std;
using namespace MINT;

class TimePdf : public PdfBase<double>{
  FitParameter tau;
public:
  TimePdf() : tau("tau"){}
  double getVal(){
    double* evtPtr = getEvent();
    if(! evtPtr){
      cout << "timePdf: Can't get event!!"
	   << endl;
      return -9999;
    }
    double t = *(evtPtr);
    return exp(-t/tau)/tau;
  }
};


int toyFit(){

  NamedParameter<int> N("N");
  EventList<double> times;
  NamedParameter<double> generatedTime("generatedTime");

  for(int i = 0; i < N; i++){
    times.Add(gRandom->Exp(generatedTime));
  }

  TimePdf myTimePdf;

  Neg2LL<double> fcn(&myTimePdf, &times);

  Minimiser mini(&fcn);
  mini.doFit();


  cout << "Done. Fitted " << N << " events"
       << " generated with mean lifetime "
       << generatedTime
       << ". Result above."
       << endl;

  return 0;
}

int main(){
  return toyFit();
}
//
