// $Id: OMACheckDeadBins.cpp,v 1.1 2009-06-16 17:55:06 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>

OMACheckDeadBins::OMACheckDeadBins(OMAlib* Env) : 
  OMACheckAlg("CheckDeadBins", Env) {
  m_ninput = 1;
  m_inputNames.push_back("UseRef");  m_inputDefValues.push_back(1);
  m_npars = 1;
  m_parnames.push_back("Confidence");
  m_parDefValues.push_back(.95);
  m_doc = "Check for empty bins indicating a dead something. ";
  m_doc += "The expected content of each bin is taken from the reference histogram (UseRef=1, default) ";
  m_doc += "or using the average entries per bin (UseRef=0)."; 
}

void OMACheckDeadBins::exec(TH1 &Histo,
                            std::vector<float> & warn_thresholds,
                            std::vector<float> & alarm_thresholds,
                            std::vector<float> & input_pars,
                            unsigned int anaID,
                            TH1* Ref) {
  bool useRef=true;
  double expValue=0.;
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  if( input_pars.size() > 0) 
    useRef = (bool) ((int) (input_pars[0]+.1));

  if (!Ref) useRef=false;
  if (useRef) {
    if (Ref->GetNbinsX() < Histo.GetNbinsX()) useRef=false;
    if (Ref->GetNbinsY() < Histo.GetNbinsY()) useRef=false;
  }

  std::stringstream ebins;
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  std::string hname(Histo.GetName());
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
      if( Histo.GetBinContent(ihx,ihy) < OMAconstants::epsilon) {
        // empty bin, see if significant
        if (useRef) {
          expValue = Ref->GetBinContent(ihx,ihy) * Histo.Integral() / Ref->Integral();
        }
        else {
          expValue = Histo.Integral() /( Histo.GetNbinsX()*Histo.GetNbinsY() );
        }
        double zeroProb = TMath::PoissonI(0, expValue);
        if ( zeroProb < (1-warn_thresholds[0]) ) {
          // significantly empty
          ebins << " "<< ihx;
          if (Histo.GetNbinsY()>1) ebins << ","<<ihy;
          if (level == OMAMessage::INFO) level=OMAMessage::WARNING;
          if ( zeroProb < (1-alarm_thresholds[0]) ) level=OMAMessage::ALARM;
        }
      }
    }
  }
  if (level > OMAMessage::INFO) {
    std::stringstream message;
    message << "bins "<<ebins.str()<<" are empty";
    raiseMessage(anaID, level, message.str(), hname);
  }
}
