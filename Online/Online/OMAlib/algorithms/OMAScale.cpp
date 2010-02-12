// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAScale.cpp,v 1.7 2010-02-12 14:25:39 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAScale::OMAScale(OMAlib* Env) : 
  OMAHcreatorAlg("Scale", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 1;
  m_parnames.push_back("k"); m_parDefValues.push_back(1.);
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Divide all bins of the first histogram by the content of the first bin of the second histogram";
  m_doc += " multiplied by factor k.";
  m_doc += " If the second histogram is equal to the first, the first bin is removed from output.";  
  
}

TH1* OMAScale::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string &outName,
			  std::string &outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() <2) return out;
  double k=m_parDefValues[0];
  if (params) {
    if (params->size()>0) k=params->at(0);
  }
  TH1* H = sources->at(0);
  TH1* scalefactorH = sources->at(1);
  TH1* scaledHist=existingHisto;
  bool copied=false;
  int shift=0;
  if(!scaledHist) {
    if( std::string(scalefactorH->GetName()) != std::string(H->GetName()) ||
       (dynamic_cast<TH2*>(H) != NULL)) {
      scaledHist=(TH1*) H->Clone();
      scaledHist->SetNameTitle(outName.data(),outTitle.data() );
      copied = true;
    }
    else {
      scaledHist= (TH1*) ( new TH1D 
                           (outName.data(), outTitle.data(), 
                            H->GetNbinsX()-1, 
                            H->GetXaxis()->GetBinLowEdge(2),
                            H->GetXaxis()->GetXmax()) );  
      scaledHist->Sumw2();
    }
  }  
  if (scaledHist && !copied) {
    shift=H->GetNbinsX()-scaledHist->GetNbinsX();
    for (int ib=1+shift; ib<=H->GetNbinsX(); ib++) {
      scaledHist->SetBinContent(ib-shift,  H->GetBinContent(ib));
    }
  }
  double normK= scalefactorH->GetBinContent(1) * k;
  
  if (scaledHist && normK > 0.)
    scaledHist->Scale(1./normK);
  
  return  scaledHist;
}



