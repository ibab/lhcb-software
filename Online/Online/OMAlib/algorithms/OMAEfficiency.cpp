// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAEfficiency.cpp,v 1.5 2009-06-11 15:17:31 ggiacomo Exp $
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
			  const std::vector<float> *params,
			  std::string &outName,
			  std::string &outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  params=params; // avoid compil. warning
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
  }
  if(outHist) {
    outHist->Sumw2();
    outHist->Divide(okH, allH, 1., 1., "B");
  }
  
  return  outHist;
}

