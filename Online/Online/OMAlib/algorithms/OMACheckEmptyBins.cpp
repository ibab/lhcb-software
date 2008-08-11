#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>

OMACheckEmptyBins::OMACheckEmptyBins(OMAcommon* Env) : 
  OMACheckAlg("CheckEmptyBins", Env) {
  m_ninput = 0;
  m_npars = 1;
  m_parnames.push_back("MaxNEmptyBins");
  m_doc = "Check for the number of empty bins in the histogram";
}

void OMACheckEmptyBins::exec(TH1 &Histo,
			     std::vector<float> & warn_thresholds,
			     std::vector<float> & alarm_thresholds,
			     std::vector<float> & input_pars) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  input_pars.empty(); // avoid compil. warning
  
  int NemptyBins=0;
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
      if( Histo.GetBinContent(ihx,ihy) < OMAconstants::epsilon)
	NemptyBins++;
    }
  }
  
  std::stringstream message;
  message << "Number of empty bins= "<<NemptyBins;
  
  if ( NemptyBins  > (int) (alarm_thresholds[0]+.1) ) {
    raiseMessage( OMAMsgInterface::ALARM , message.str(), Histo.GetName());
  }
  else if( NemptyBins > (int) (warn_thresholds[0]+.1)) {
    raiseMessage( OMAMsgInterface::WARNING , message.str(), Histo.GetName());
  }
}
