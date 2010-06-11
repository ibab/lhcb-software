// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAEfficiency.cpp,v 1.8 2010-06-11 13:00:10 ggiacomo Exp $
#include <cmath>
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAEfficiency::OMAEfficiency(OMAlib* Env) : 
  OMAHcreatorAlg("Efficiency", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Ratio of two histograms with binomial errors";
}

TH1* OMAEfficiency::exec( const std::vector<TH1*> *sources,
                          const std::vector<float> *,
                          std::string &outName,
                          std::string &outTitle,
                          TH1* existingHisto,
                          TH1* ) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() >= 2)
    out = exec( sources->at(0), sources->at(1), outName, outTitle, existingHisto);
  return out;
}

TH1* OMAEfficiency::exec(TH1* okH,
			 TH1* allH,
			 std::string &outName,
			 std::string &outTitle,
			 TH1* existingHisto) {  
  TH1* outHist=0;
  if (existingHisto) {
    outHist= dynamic_cast<TH1D*>(existingHisto);
    if (!outHist)
      outHist= dynamic_cast<TH2D*>(existingHisto);
  }
  if (!outHist) {
    if ( 1 == okH->GetDimension()  ) 
      outHist = (TH1*) ( new TH1D (outName.data(), outTitle.data(), 
				   okH->GetNbinsX(), 
				   okH->GetXaxis()->GetXmin(),
				   okH->GetXaxis()->GetXmax()) );  
    else if(2 == okH->GetDimension() )
      outHist = (TH1*) ( new TH2D (outName.data(), outTitle.data(), 
				   okH->GetNbinsX(), 
				   okH->GetXaxis()->GetXmin(),
				   okH->GetXaxis()->GetXmax(),
				   okH->GetNbinsY(), 
				   okH->GetYaxis()->GetXmin(),
				   okH->GetYaxis()->GetXmax()) ); 
    outHist->Sumw2();
  }
  if(outHist) {
    //outHist->Divide(okH, allH, 1., 1., "B");
    // use the score confidence interval Agresti-Coull formula for more reasonable binomial errors  
    if (okH->GetNbinsX() != allH->GetNbinsX() ||
        okH->GetNbinsY() != allH->GetNbinsY() ) return NULL;
    for (int ix=1 ; ix<= allH->GetNbinsX(); ix++) {
      for (int iy=1 ; iy<= allH->GetNbinsY(); iy++) {
        double x = okH->GetBinContent(ix,iy);
        double n = allH->GetBinContent(ix,iy);
        double pstar= (x+2.)/(n+4.);
        double error = sqrt(pstar*(1-pstar)/(n+4.));
        outHist->SetBinContent(ix,iy,pstar);  
        outHist->SetBinError(ix,iy,error);  
      }
    }
    outHist->SetEntries(okH->GetEntries());
    outHist->SetBit(TH1::kIsAverage);
  }
  return  outHist;
}

