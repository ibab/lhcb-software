// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/EventList.h"

#include "Mint/PdfBase.h"

#include "TRandom.h"

#include <iostream>

#include <omp.h>
#include <cstdio>

#include <ctime>

using namespace std;
using namespace MINT;

class TimePdf : public PdfBase<double>{
  FitParameter tau;
public:
  TimePdf() : tau("tau"){}
  double getVal(double& t){
      
    //printf("Hello from TimePdf at thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
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

  time_t startTime = time(0);

  Neg2LL fcn(myTimePdf, times);

  Minimiser mini(&fcn);
  mini.doFit();

  time_t difftime = time(0) - startTime;

  cout << "Done. Fitted " << N << " events"
       << " generated with mean lifetime "
       << generatedTime
       << ". Result above."
       << "\n This took " << difftime << " s."
       << endl;

  return 0;
}

int main(){
  return toyFit();
}
//
