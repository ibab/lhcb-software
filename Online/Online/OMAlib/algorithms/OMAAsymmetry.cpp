// $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAAsymmetry::OMAAsymmetry(OMAlib* Env) : 
  OMAHcreatorAlg("Asymmetry", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Bin-by-bin asimmetry of two histograms (h1-h2)/(h1+h2)";
}

TH1* OMAAsymmetry::exec( const std::vector<TH1*> *sources,
                      const std::vector<float>* ,
                      std::string &outName,
                      std::string &outTitle,
                      TH1* existingHisto,
                      TH1* ) {
  TH1* outHist=NULL;
  if (! sourceVerified(sources) ) return outHist;
  if (sources->size() <2) return outHist;

  TH1* h1 = sources->at(0);
  TH1* h2 = sources->at(1);
  TH1* hasym =  (TH1*) h1->GetAsymmetry(h2);
  
  if(hasym) {    
    if (existingHisto) {
      if(existingHisto->GetNbinsX() == hasym->GetNbinsX() &&
         existingHisto->GetNbinsY() == hasym->GetNbinsY()) {
        outHist= existingHisto;
        for (int ix=1 ; ix<= hasym->GetNbinsX(); ix++) {
          for (int iy=1 ; iy<= hasym->GetNbinsY(); iy++) {
            outHist->SetBinContent(ix,iy, hasym->GetBinContent(ix,iy));
            outHist->SetBinError(ix,iy, hasym->GetBinError(ix,iy));
          }
        }
        hasym->Delete();
      }
    }
    if(!outHist) {
      outHist = hasym;
      outHist->SetName(outName.data());
      outHist->SetTitle(outTitle.data());
      outHist->SetBit(TH1::kIsAverage);
    }
  }
  return  outHist;
}

