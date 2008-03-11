// $Id: OMACheckMeanAndSigma.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $

#include <TH1F.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMACheckMeanAndSigma::OMACheckMeanAndSigma() : OMACheckAlg("CheckMeanAndSigma") {
  m_npars = 4;
  m_parnames.push_back("MinMean");
  m_parnames.push_back("MaxMean");
  m_parnames.push_back("MinSigma");
  m_parnames.push_back("MaxSigma");
  m_ninput = 1;
  m_inputNames.push_back("significance");
  m_doc = "Check that the Histogram average and standard deviation are in the specified ranges";
  m_doc +=  " with a given normal significance (default is 0.95)";
}

void OMACheckMeanAndSigma::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars) {
  float significance=0.95;
  if ( !input_pars.empty() )
    significance = input_pars[0];
  if( warn_thresholds.size() <4 ||  alarm_thresholds.size() <4 )
    return;

  if (false == checkMean(Histo,alarm_thresholds[0],alarm_thresholds[1],
                         significance) ) { // alarm on
    raiseMessage( ALARM , 
                  " Mean out of range",
                  Histo.GetName());
  }
  else {
    if (false == checkMean(Histo,warn_thresholds[0],warn_thresholds[1],
                           significance) ) { // warning on
      raiseMessage( WARNING , 
                    " Mean out of range",
                    Histo.GetName());
    }
  }
  if (false == checkSigma(Histo,alarm_thresholds[2],alarm_thresholds[3],
                         significance) ) { // alarm on
    raiseMessage( ALARM , 
                  " Sigma out of range",
                  Histo.GetName());
  }
  else {
    if (false == checkSigma(Histo,warn_thresholds[2],warn_thresholds[3],
                           significance) ) { // warning on
      raiseMessage( WARNING , 
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
