#include "OMAlib/OMAFitFunction.h"
#include <TH1.h>
#include <TList.h>
#include <vector>

//=============================================================================
// Algorithm for HLT jpsi plot
// 2010-10-18 : Eric Van Herwijnen (adapted by GG)
// 2011-05-03 : second version
//=============================================================================


OMAFitHLTjpsi::OMAFitHLTjpsi()  :
  OMAFitFunction("HLTjpsi")
{
  m_parNames.push_back("Constant");
  m_parNames.push_back("Mean");
  m_parNames.push_back("Sigma");
  m_parNames.push_back("bkg");
  m_parNames.push_back("bkgSlope");

  m_mustInit=false;
  m_doc="custom fit for HLT jpsi plot";
  m_predef=false;

  m_funcString= "[0]*exp(-((x-[1])**2/(2*[2]**2)))+[3]+[4]*(x-[1])/[2]";
  initfun();
  ((TF1*) this)->SetRange(300,10000);
  ((TF1*) this)->SetLineColor(4);
}


OMAFitHLTjpsi::~OMAFitHLTjpsi() {} 


void OMAFitHLTjpsi::fit(TH1* histo, std::vector<float>* , std::vector<float>* fitRange) {
  double xmin=0., xmax=0.;
  if(!histo) return;
  if(histo->Integral() < 10.) return;
  if(fitRange) {
    if(fitRange->size()>1) {
      xmin=fitRange->at(0);
      xmax=fitRange->at(1);
    }
  }
  Int_t binnr = histo->GetMaximumBin();
  Double_t maxbin = histo->GetBinContent(binnr);
  ((TF1*) this)->SetParameters(maxbin,histo->GetMean(),histo->GetRMS(),0.,0.);
  histo->Fit((TF1*) this,"R","",xmin,xmax);
  return;
}

