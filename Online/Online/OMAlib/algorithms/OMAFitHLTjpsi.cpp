//=============================================================================
// Include files 
// Online
#include "OMAlib/OMAFitFunction.h"
#include <TH1.h>
#include <TList.h>
#include <vector>

//=============================================================================
// Algorithm for HLT jpsi plot
// 2010-10-18 : Eric Van Herwijnen (adapted by GG)
//=============================================================================




OMAFitHLTjpsi::OMAFitHLTjpsi() :
  OMAFitFunction("HLTjpsi"),
  m1("hjm1","gaus",3066,3126),
  m2("hjm2","pol1",2900,3066),
  m3("hjm3","pol1",3126,3300)
{
  m_parNames.push_back("Constant");
  m_parNames.push_back("Mean");
  m_parNames.push_back("Sigma");
  m_parNames.push_back("bkgL");
  m_parNames.push_back("slopeL");
  m_parNames.push_back("bkgR");
  m_parNames.push_back("slopeR");

  m_mustInit=false;
  m_doc="custom fit for HLT jpsi plot";
  m_predef=false;

  m_funcString= "gaus(0)+pol1(3)+pol1(5)";
  initfun();
  m1.SetLineColor(0);
  m1.SetLineWidth(0);
  ((TF1*) this)->SetRange(2900,3300);
  ((TF1*) this)->SetLineColor(2);
}

OMAFitHLTjpsi::~OMAFitHLTjpsi() {
}

void OMAFitHLTjpsi::fit(TH1* histo, std::vector<float>* ) {
  if(!histo) return;
  Double_t par[7];
  histo->Fit(&m1,"R");
  histo->Fit(&m2,"NR");
  histo->Fit(&m3,"NR");
  m1.GetParameters(&par[0]);
  m2.GetParameters(&par[3]);
  m3.GetParameters(&par[5]);
  ((TF1*) this)->SetParameters(par);
  histo->Fit((TF1*) this,"R+");
  TF1* storedM1 =  histo->GetFunction("hjm1");
  if(storedM1) {
    // display final fit parameters in associated function
    storedM1->SetChisquare( ((TF1*) this)->GetChisquare() );
    storedM1->SetNDF( ((TF1*) this)->GetNDF());
    for (int ip=0; ip<3; ip++) {
        storedM1->SetParameter(ip, ((TF1*) this)->GetParameter(ip));
    }
  }
  return;
}


