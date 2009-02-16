// $Id: OMACheckMeanAndSigma.cpp,v 1.4 2009-02-16 10:38:21 ggiacomo Exp $

#include <TH1F.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMACheckMeanAndSigma::OMACheckMeanAndSigma(OMAcommon* Env) : 
  OMACheckAlg("CheckMeanAndSigma", Env) {
  m_npars = 4;
  m_parnames.push_back("MinMean"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxMean"); m_parDefValues.push_back(+999999.);
  m_parnames.push_back("MinSigma"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxSigma"); m_parDefValues.push_back(+999999.);
  m_ninput = 1;
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(0.95);
  m_doc = "Check that the Histogram average and standard deviation are in the specified ranges";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMACheckMeanAndSigma::exec(TH1 &Histo,
                                std::vector<float> & warn_thresholds,
                                std::vector<float> & alarm_thresholds,
                                std::vector<float> & input_pars,
                                unsigned int anaID,
                                TH1* Ref) {
  float confidence=0.95;
  Ref = NULL; // avoid compil. warning
  if ( !input_pars.empty() )
    confidence = input_pars[0];
  if( warn_thresholds.size() <4 ||  alarm_thresholds.size() <4 )
    return;
  std::string message(" Mean out of range");
  std::string hname(Histo.GetName());

  if (false == checkMean(Histo,alarm_thresholds[0],alarm_thresholds[1],
                         confidence) ) { // alarm on
    raiseMessage( anaID,
                  OMAMessage::ALARM , 
                  message, hname);
  }
  else {
    if (false == checkMean(Histo,warn_thresholds[0],warn_thresholds[1],
                           confidence) ) { // warning on
      raiseMessage( anaID,
                    OMAMessage::WARNING , 
                    message, hname);
    }
  }
  message=" Sigma out of range";
  if (false == checkSigma(Histo,alarm_thresholds[2],alarm_thresholds[3],
                         confidence) ) { // alarm on
    raiseMessage( anaID,
                  OMAMessage::ALARM , 
                  message, hname);
  }
  else {
    if (false == checkSigma(Histo,warn_thresholds[2],warn_thresholds[3],
                           confidence) ) { // warning on
      raiseMessage( anaID,
                    OMAMessage::WARNING , 
                    message, hname);
    }
  }
}

bool OMACheckMeanAndSigma::checkMean(TH1 &Histo,
                                     float min,
                                     float max,
                                     float conf) {
  bool out = true;
  double mean=Histo.GetMean();
  if( mean < min || mean > max ) {
    // mean out of range: check if deviation from range is significant
    double err=	Histo.GetMeanError();
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) > conf)  
      out = false;      
  }
  return out;
}

bool OMACheckMeanAndSigma::checkSigma(TH1 &Histo,
                                      float min,
                                      float max,
                                      float conf) {
  bool out = true;
  double mean=Histo.GetRMS();
  if( mean < min || mean > max ) {
    // rms out of range: check if deviation from range is significant
    double err=	Histo.GetRMSError();
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) > conf )  
      out = false;      
  }
  return out;
}
