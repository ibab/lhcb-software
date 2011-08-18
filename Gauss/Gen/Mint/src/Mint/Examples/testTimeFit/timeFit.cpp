#include "Mint/Mint/PdfAndLogL/PdfBase.h"
#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mint/Events/EventList.h"
#include "Mint/Mint/PdfAndLogL/Neg2LL.h"
#include "Mint/Mint/Minimiser/Minimiser.h"

#include <iostream>

using namespace MINT;
using namespace std;

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

int main(){

  NamedParameter<int> N("N");
  NamedParameter<double> generatedTime("generatedTime");
  EventList<double> times;

  for(int i = 0; i < N; i++){
    times.push_back(gRandom->Exp(generatedTime));
  }
  TimePdf myTimePdf;
  Neg2LL<double> fcn(&myTimePdf, &times);

  Minimiser mini(&fcn);
  mini.doFit();

  return 0;
}
