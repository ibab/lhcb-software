// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "TMath.h"

void Babar06AmpAndPhaseCalc()
{
  double ReAmp[17] =
    {
      -1.223,
      1.698,
      0.834,
      -0.248,
      -1.285,
      0.0997,
      0.027,
      -0.019,
      1.000,
      -0.02194,
      0.699,
      0.253,
      -0.99,
      -0.4465,
      -0.95,
      -1.28,
      -0.290
    };
  
  double ImAmp[17] =
    {
      1.3461,
      0.576,
      -0.931,
      -0.108,
      0.205,
      -0.1271,
      0.076,
      -0.177,
      0,
      0.03942,
      -0.387,
      0.036,
      3.82,
      -0.2572,
      1.619,
      -0.273,
      0.0655
    };
  

  for (int i=0;i<17;i++){
    Double_t amp=sqrt(ReAmp[i]*ReAmp[i]+ImAmp[i]*ImAmp[i]);
    Double_t tanphase = fabs(ImAmp[i]/ReAmp[i]);
    Double_t phase = atan(tanphase)*180/TMath::Pi();
    
    if (ImAmp[i]>0.0 && ReAmp[i]<0.0) phase=180.0-phase;
    if (ImAmp[i]<0.0 && ReAmp[i]<0.0) phase=phase-180.0;
    if (ImAmp[i]<0.0 && ReAmp[i]>0.0) phase=-1.0*phase;
    

    cout << "Reaonance[" << i<< "] Amp = " << amp << " Phase = " << phase << endl;
    

  }
  
  return;
  
}
