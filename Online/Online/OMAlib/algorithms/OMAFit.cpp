// $Id: OMAFit.cpp,v 1.1 2009-02-16 10:38:21 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

// special Check algorithm: fit function is specified for each analysis by the first input parameter
// no prior knowledge of output algorithms that are taken dinamically according to requested fit function
// and are defined in dedicated OMAFitFunction objects
OMAFit::OMAFit(OMAcommon* Env) : 
  OMACheckAlg("Fit", Env) {
  m_npars = 1;
  m_parnames.push_back("Min_chi2_prob"); m_parDefValues.push_back(0.);

  m_ninput = 2;
  m_inputNames.push_back("Fit_function"); m_inputDefValues.push_back(1);
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(.95);
  m_doc = "Fit histogram with a given function and check chi2 and/or output parameters";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMAFit::exec(TH1 &Histo,
                  std::vector<float> & warn_thresholds,
                  std::vector<float> & alarm_thresholds,
                  std::vector<float> & input_pars,
                  unsigned int anaID,
                  TH1* Ref) {
  warn_thresholds.size();
  alarm_thresholds.size();
  input_pars.size();
  anaID=anaID;
  Ref=Ref;
  // to be implemented
}


