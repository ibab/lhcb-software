// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "TMath.h"

void Babar05AmpAndPhaseCalc()
{
  double ReAmp[17] =
    {
      -1.143,
      2.309,
      0.626,
      -0.202,
      -0.70,
      0.1085,
      0.280,
      -0.033,
      1.000,
      -0.0207,
      0.860,
      0.347,
      -3.24,
      -0.3986,
      -0.24,
      -1.333,
      -0.263
    };
  
  double ImAmp[17] =
    {
      1.338,
      -0.636,
      -0.939,
       0.049,
      -0.44,
      -0.1294,
      0.025,
      -0.078,
      0,
      0.03306,
      -0.386,
      0.264,
      3.69,
      -0.205,
      1.34,
      -0.127,
      0.015
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
