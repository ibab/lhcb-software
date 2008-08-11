#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>

OMACompareToReference::OMACompareToReference(OMAcommon* Env) : 
  OMACheckAlg("CompareToReference", Env) {
  m_ninput = 1;
  m_inputNames.push_back("Test");
  m_npars = 1;
  m_parnames.push_back("Min_p-value");
  m_doc = "Compares histogram to reference performing a Kolmogorov (Test=1, default) or chi2 (Test=2) test.";
  m_needRef = true;
}

void OMACompareToReference::exec(TH1 &Histo,
			     std::vector<float> & warn_thresholds,
			     std::vector<float> & alarm_thresholds,
			     std::vector<float> & input_pars) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars)
    return;
  int test =1;
  double pvalue=1.;
  if(input_pars.size() > 0)
    test = (int) (input_pars[0]+0.1);
  
  if(m_ref){
    if (test ==1 ) {
      pvalue = Histo.Chi2Test(m_ref);
    }
    else if (test == 2  ) {
      pvalue = Histo.KolmogorovTest(m_ref);
    }
  }
  else {
    return;
  }
  
  std::stringstream message;
  message << "Comparison Test p-value= "<<  pvalue;
  if ( pvalue  < alarm_thresholds[0] ) {
    raiseMessage( OMAMsgInterface::ALARM , message.str(), Histo.GetName());
  }
  else if( pvalue < warn_thresholds[0] ) {
    raiseMessage( OMAMsgInterface::WARNING , message.str(), Histo.GetName());
  }
}
