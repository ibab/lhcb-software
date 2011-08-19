#include "Mint/PdfBase.h"
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/EventList.h"
#include "Mint/Neg2LL.h"
#include "Mint/Minimiser.h"

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
