// $Id: OMACheckMeanAndSigma.cpp,v 1.2 2008-08-11 08:05:15 ggiacomo Exp $

#include <TH1F.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMACheckMeanAndSigma::OMACheckMeanAndSigma(OMAcommon* Env) : 
  OMACheckAlg("CheckMeanAndSigma", Env) {
  m_npars = 4;
  m_parnames.push_back("MinMean");
  m_parnames.push_back("MaxMean");
  m_parnames.push_back("MinSigma");
  m_parnames.push_back("MaxSigma");
  m_ninput = 1;
  m_inputNames.push_back("confidence");
  m_doc = "Check that the Histogram average and standard deviation are in the specified ranges";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMACheckMeanAndSigma::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars) {
  float confidence=0.95;
  if ( !input_pars.empty() )
    confidence = input_pars[0];
  if( warn_thresholds.size() <4 ||  alarm_thresholds.size() <4 )
    return;

  if (false == checkMean(Histo,alarm_thresholds[0],alarm_thresholds[1],
                         confidence) ) { // alarm on
    raiseMessage( OMAMsgInterface::ALARM , 
                  " Mean out of range",
                  Histo.GetName());
  }
  else {
    if (false == checkMean(Histo,warn_thresholds[0],warn_thresholds[1],
                           confidence) ) { // warning on
      raiseMessage( OMAMsgInterface::WARNING , 
                    " Mean out of range",
                    Histo.GetName());
    }
  }
  if (false == checkSigma(Histo,alarm_thresholds[2],alarm_thresholds[3],
                         confidence) ) { // alarm on
    raiseMessage( OMAMsgInterface::ALARM , 
                  " Sigma out of range",
                  Histo.GetName());
  }
  else {
    if (false == checkSigma(Histo,warn_thresholds[2],warn_thresholds[3],
                           confidence) ) { // warning on
      raiseMessage( OMAMsgInterface::WARNING , 
                    " Sigma out of range",
                    Histo.GetName());
    }
  }
}

bool OMACheckMeanAndSigma::checkMean(TH1 &Histo,
                                     float min,
                                     float max,
                                     float sig) {
  bool out = true;
  double mean=Histo.GetMean();
  if( mean < min || mean > max ) {
    // mean out of range: check if deviation from range is significant
    double err=	Histo.GetMeanError();
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) < sig)  
      out = false;      
  }
  return out;
}

bool OMACheckMeanAndSigma::checkSigma(TH1 &Histo,
                                      float min,
                                      float max,
                                      float sig) {
  bool out = true;
  double mean=Histo.GetRMS();
  if( mean < min || mean > max ) {
    // rms out of range: check if deviation from range is significant
    double err=	Histo.GetRMSError();
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) < sig )  
      out = false;      
  }
  return out;
}
