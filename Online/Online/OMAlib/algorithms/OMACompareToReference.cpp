#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>

OMACompareToReference::OMACompareToReference(OMAcommon* Env) : 
  OMACheckAlg("CompareToReference", Env) {
  m_ninput = 1;
  m_inputNames.push_back("Test"); m_inputDefValues.push_back(1.);
  m_npars = 1;
  m_parnames.push_back("Min_p-value"); m_parDefValues.push_back(.01);
  m_doc = "Compares histogram to reference performing a Kolmogorov (Test=1, default) or chi2 (Test=2) test.";
  m_needRef = true;
}

void OMACompareToReference::exec(TH1 &Histo,
                                 std::vector<float> & warn_thresholds,
                                 std::vector<float> & alarm_thresholds,
                                 std::vector<float> & input_pars,
                                 unsigned int anaID,
                                 TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars)
    return;
  if(Histo.GetEntries() < m_minEntries) return; // too few hits for this test
  int test =1;
  double pvalue=1.;
  if(input_pars.size() > 0)
    test = (int) (input_pars[0]+0.1);
  
  if(Ref){
    if (test ==1 ) {
      pvalue = Histo.Chi2Test(Ref);
    }
    else if (test == 2  ) {
      pvalue = Histo.KolmogorovTest(Ref);
    }
  }
  else {
    return;
  }
  
  std::stringstream message;
  std::string hname(Histo.GetName());
  message << "Comparison Test p-value= "<<  pvalue;
  raiseMessage( anaID,
                ( pvalue < warn_thresholds[0] ),
                ( pvalue  < alarm_thresholds[0] ),
                message.str(), 
                hname);
}
