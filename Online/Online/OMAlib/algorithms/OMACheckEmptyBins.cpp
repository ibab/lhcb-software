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
  m_parDefValues.push_back(99999.);
  m_doc = "Check for the number of empty bins in the histogram";
}

void OMACheckEmptyBins::exec(TH1 &Histo,
                             std::vector<float> & warn_thresholds,
                             std::vector<float> & alarm_thresholds,
                             std::vector<float> & input_pars,
                             unsigned int anaID,
                             std::string& taskName,
                             TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  input_pars.empty(); // avoid compil. warning
  Ref = NULL; // avoid compil. warning

  int NemptyBins=0;
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
      if( Histo.GetBinContent(ihx,ihy) < OMAconstants::epsilon)
	NemptyBins++;
    }
  }
  
  std::stringstream message;
  std::string hname(Histo.GetName());
  message << "Number of empty bins= "<<NemptyBins;
  if ( NemptyBins  > (int) (alarm_thresholds[0]+.1) ) {
    raiseMessage( anaID, OMAMessage::ALARM , message.str(), hname, taskName);
  }
  else if( NemptyBins > (int) (warn_thresholds[0]+.1)) {
    raiseMessage( anaID, OMAMessage::WARNING , message.str(), hname, taskName);
  }
}
