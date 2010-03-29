// $Id: OMACheckDeadBins.cpp,v 1.4 2010-03-29 14:41:48 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include <sstream>

OMACheckDeadBins::OMACheckDeadBins(OMAlib* Env) : 
  OMACheckAlg("CheckDeadBins", Env) {
  m_ninput = 1;
  m_inputNames.push_back("UseRef");  m_inputDefValues.push_back(1);
  m_npars = 1;
  m_parnames.push_back("Confidence");
  m_parDefValues.push_back(.95);
  m_needRef = true;
  m_doc = "Check for empty bins indicating a dead something. ";
  m_doc += "The expected content of each bin is taken from the reference histogram (UseRef=1, default) ";
  m_doc += "or using the average entries per bin (UseRef=0)."; 

  m_minEntries=1000;
  m_minEntriesPerBin=100;
}

void OMACheckDeadBins::exec(TH1 &Histo,
                            std::vector<float> & warn_thresholds,
                            std::vector<float> & alarm_thresholds,
                            std::vector<float> & input_pars,
                            unsigned int anaID,
                            TH1* Ref) {
  bool useRef=true;
  double expValue=0.;
  bool hasLabels=false;
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  if( input_pars.size() > 0) 
    useRef = (bool) ((int) (input_pars[0]+.1));

  if (!Ref) useRef=false;
  if (useRef) {
    if (Ref->GetNbinsX() < Histo.GetNbinsX()) useRef=false;
    if (Ref->GetNbinsY() < Histo.GetNbinsY()) useRef=false;
  }

  // get bin labels if available
  if(m_oh && m_omaEnv->dbSession() && Histo.GetDimension() == 1) {
    if (Histo.GetXaxis()) {
      if (m_oh->nXbinlabels() > 0 && 
          Histo.GetXaxis()->GetNbins() >= (int)m_oh->nXbinlabels() ) {
        hasLabels=true;
        std::string sopt;
        for (unsigned int il = 0; il < m_oh->nXbinlabels(); il++) {
          sopt = m_oh->binlabel(il,0);
          Histo.GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
  }


  std::stringstream ebins;
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  std::string hname(Histo.GetName());
  int nempty=0;
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
      if( Histo.GetBinContent(ihx,ihy) < OMAconstants::epsilon) {
        // empty bin, see if significant
        // (to avoid fake alarms, take reference - 3 sigma_reference as expected value)
        if (useRef) {
          expValue = (Ref->GetBinContent(ihx,ihy) -3.*Ref->GetBinError(ihx,ihy)) 
            * Histo.Integral() / Ref->Integral();
        }
        else {
          expValue = (Histo.Integral() -3.*TMath::Sqrt(Histo.Integral()))/
            ( Histo.GetNbinsX()*Histo.GetNbinsY() );
        }
        if (expValue<0) expValue=0.;
        double zeroProb = TMath::PoissonI(0, expValue);
        if ( zeroProb < (1-warn_thresholds[0]) ) {
          // significantly empty
          ebins << " ";
          if (hasLabels) {
            ebins << Histo.GetXaxis()->GetBinLabel(ihx);
          }
          else {
            ebins << Form("%.1f",Histo.GetBinCenter(ihx));
          }
          nempty++;
          if (Histo.GetNbinsY()>1) ebins << ","<<ihy;
          if (level == OMAMessage::INFO) level=OMAMessage::WARNING;
          if ( zeroProb < (1-alarm_thresholds[0]) ) level=OMAMessage::ALARM;
        }
      }
    }
  }
  if (level > OMAMessage::INFO) {
    std::stringstream message;
    if (nempty > 1) {
      message <<"bins at values"<<ebins.str()<<" are empty";
    }
    else {
      message <<"bin at value"<<ebins.str()<<" is empty, expecting "<<expValue;
    }
    raiseMessage(anaID, level, message.str(), hname);
  }
}

bool OMACheckDeadBins::refMissing(TH1* ref,
                                  std::vector<float> & input_pars) {
  if(input_pars.size() <m_ninput) return false; 
  bool useRef = (bool) ((int) (input_pars[0]+.1));
  return (useRef &&  !ref);
}
