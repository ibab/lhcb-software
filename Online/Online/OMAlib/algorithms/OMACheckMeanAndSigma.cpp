// $Id: OMACheckMeanAndSigma.cpp,v 1.7 2010-02-12 14:25:39 ggiacomo Exp $

#include <TH1F.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMACheckMeanAndSigma::OMACheckMeanAndSigma(OMAlib* Env) : 
  OMACheckAlg("CheckMeanAndSigma", Env) {
  m_npars = 4;
  m_parnames.push_back("MinMean"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxMean"); m_parDefValues.push_back(+999999.);
  m_parnames.push_back("MinSigma"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxSigma"); m_parDefValues.push_back(+999999.);
  m_ninput = 2;
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(0.95);
  m_inputNames.push_back("axis");       m_inputDefValues.push_back(1.);
  m_doc = "Check that the Histogram average and standard deviation of X(axis=1)";
  m_doc +=  " or Y(axis=2) variable are in the specified ranges";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMACheckMeanAndSigma::exec(TH1 &Histo,
                                std::vector<float> & warn_thresholds,
                                std::vector<float> & alarm_thresholds,
                                std::vector<float> & input_pars,
                                unsigned int anaID,
                                TH1* Ref) {
  float confidence=m_inputDefValues[0];
  int axis=(int) (m_inputDefValues[1]+0.1);
  Ref = NULL; // avoid compil. warning
  if ( input_pars.size() > 0 ) confidence = input_pars[0];
  if ( input_pars.size() > 1 ) axis = (int) (input_pars[1]+0.1);

  if( warn_thresholds.size() <4 ||  alarm_thresholds.size() <4 )
    return;
  
  
  std::stringstream message;
  std::string hname(Histo.GetName());
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  

  if (false == checkMean(Histo,axis,alarm_thresholds[0],alarm_thresholds[1],
                         confidence, message) ) { // alarm on
    level = OMAMessage::ALARM;
  }
  else {
    if (false == checkMean(Histo,axis,warn_thresholds[0],warn_thresholds[1],
                           confidence, message) ) { // warning on
      level = OMAMessage::WARNING;
    }
  }

  if (false == checkSigma(Histo,axis,alarm_thresholds[2],alarm_thresholds[3],
                         confidence, message) ) { // alarm on
    level = OMAMessage::ALARM;
  }
  else {
    if (false == checkSigma(Histo,axis,warn_thresholds[2],warn_thresholds[3],
                           confidence, message) ) { // warning on
      level = OMAMessage::WARNING;
    }
  }
  if (level > OMAMessage::INFO) {
    raiseMessage( anaID,
                  level , 
                  message.str(), 
                  hname);
  }
}

bool OMACheckMeanAndSigma::checkMean(TH1 &Histo,
                                     int axis,
                                     float min,
                                     float max,
                                     float conf,
                                     std::stringstream &mess) {
  bool out = true;
  double mean=Histo.GetMean(axis);
  if( mean < min || mean > max ) {
    // mean out of range: check if deviation from range is significant
    double err=	Histo.GetMeanError(axis);
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) > conf)  
      out = false;      
  }

  if (false == out) {
    if (! mess.str().empty()) {
      mess << std::endl;
    }
    mess << "Mean is "<<Histo.GetMean(axis) << " (allowed range is "
            << min << " - " << max << " )";
  }
  return out;
}

bool OMACheckMeanAndSigma::checkSigma(TH1 &Histo,
                                      int axis,
                                      float min,
                                      float max,
                                      float conf,
                                      std::stringstream &mess) {
  bool out = true;
  double mean=Histo.GetRMS(axis);
  if( mean < min || mean > max ) {
    // rms out of range: check if deviation from range is significant
    double err=	Histo.GetRMSError(axis);
    double delta = 
      ( mean < min ) ? (min-mean)/err : (mean-max)/err;
    if( StudentI(delta, Histo.GetEntries()-1) > conf )  
      out = false;     
  }

  if (false == out) {
    if (! mess.str().empty()) {
      mess << std::endl;
    }
    mess << "Sigma is "<<Histo.GetRMS(axis) << " (allowed range is "
            << min << " - " << max << " )";
  }
  return out;
}
