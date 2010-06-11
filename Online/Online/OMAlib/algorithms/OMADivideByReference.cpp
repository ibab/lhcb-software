// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMADivideByReference.cpp,v 1.2 2010-06-11 16:28:51 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"


OMADivideByReference::OMADivideByReference(OMAlib* Env) : 
  OMAHcreatorAlg("DivideByReference", Env) {
  m_ninput = 1;
  m_histSetFlag = false;
  m_npars = 1;
  m_parnames.push_back("norm"); m_parDefValues.push_back(1.);
  m_outHType = OnlineHistDBEnv::SAM;
  m_needRef = true;
  m_doc = "Divide histogram by its reference. use norm=0 for no normalization, norm=1 for normalization to same area";
  m_doc += ", norm=2 for normalization to same entries";
}

TH1* OMADivideByReference::exec( const std::vector<TH1*> *sources,
                                 const std::vector<float> *params,
                                 std::string &outName,
                                 std::string &outTitle,
                                 TH1* existingHisto,
                                 TH1* ref) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() < m_ninput) return out;
  int norm = intParam(m_parDefValues[0]);
  if (params) {
    if (params->size()>0) norm=intParam(params->at(0));
  }
  TH1* orig = sources->at(0);
  if (!ref) {
    std::string ermess = "no Reference available for histo ";
    ermess += orig->GetName();
    m_omaEnv->senderror(ermess.c_str());
    return out;
  }

  // check compatibility of histogram and its ref
  if (orig->GetDimension() != ref->GetDimension() ||
      orig->GetNbinsX() != ref->GetNbinsX() ||
      orig->GetNbinsY() != ref->GetNbinsY() ) {
    std::string ermess = orig->GetName();
    ermess += " histogram not compatible with the provided reference";
    m_omaEnv->senderror(ermess.c_str());
    return out;
  }

  double knorm=1;
  if (norm>0) {  
    if (norm ==1 && orig->GetSumOfWeights() > 0.) 
      knorm = ref->GetSumOfWeights()/orig->GetSumOfWeights();
    if (norm ==2 && orig->GetEntries() > 0.) 
      knorm = ref->GetEntries()/orig->GetEntries();
  }
  if (existingHisto) {
    out = existingHisto;
    out->Reset();
    out->Sumw2();
    for (int ix=1 ; ix<= orig->GetNbinsX(); ix++) {
      for (int iy=1 ; iy<= orig->GetNbinsY(); iy++) {
        out->SetBinContent(ix,iy, orig->GetBinContent(ix,iy) * knorm );
        out->SetBinError(ix,iy, orig->GetBinError(ix,iy) * knorm);
      }
    }
    out->SetEntries(orig->GetEntries());
  }
  else {
    out = (TH1*) orig->Clone(outName.c_str());
    out->Scale(knorm);
    out->SetNameTitle(outName.data(), outTitle.data());
  }
  
  out->Divide(ref);
  out->SetBit(TH1::kIsAverage);
  return  out;
}

