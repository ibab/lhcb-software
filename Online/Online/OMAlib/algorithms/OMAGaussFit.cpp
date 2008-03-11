// $Id: OMAGaussFit.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMAGaussFit::OMAGaussFit() : OMACheckAlg("GaussFit") {
  m_npars = 4;
  m_parnames.push_back("MinMean");
  m_parnames.push_back("MaxMean");
  m_parnames.push_back("MinSigma");
  m_parnames.push_back("MaxSigma");
  m_ninput = 1;
  m_inputNames.push_back("significance");
  m_doc = "Fit histogram with a gaussian and check that average and sigma are in the specified ranges";
  m_doc +=  " with a given normal significance (default is 0.95)";
}

void OMAGaussFit::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars) {
  float significance=0.95;

  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;

  if ( input_pars.size() > 0 )
    significance = input_pars[0];

  Histo.Fit("gaus");
  TF1 *fit = Histo.GetFunction("gaus");
  if (fit) {
    if (false == checkParam( fit,1,
                             alarm_thresholds[0],
                             alarm_thresholds[1],
                             significance) ) { // alarm on
    raiseMessage( ALARM , 
                  " Mean out of range",
                  Histo.GetName());
    }
    else {
      if (false == checkParam( fit,1,
                               warn_thresholds[0],
                               warn_thresholds[1],
                               significance) ) { // warning on
        raiseMessage( WARNING , 
                      " Mean out of range",
                      Histo.GetName());
      }
    }
    if (false == checkParam( fit,2,
                             alarm_thresholds[2],
                             alarm_thresholds[3],
                             significance) ) { // alarm on
      raiseMessage( ALARM , 
                    " Sigma out of range",
                    Histo.GetName());
    }
    else {
      if (false == checkParam( fit,2,
                               warn_thresholds[2],
                               warn_thresholds[3],
                               significance) ) { // warning on
        raiseMessage( WARNING , 
                      " Sigma out of range",
                      Histo.GetName());
      }
    }
  }
}

bool OMAGaussFit::checkParam( TF1* fit,
                              int ipar,
                              float min,
                              float max,
                              float sig) {
  bool out = true;
  int ndf= fit->GetNDF();
  double param= fit->GetParameter(ipar);
  double error= fit->GetParError(ipar);
  if( param < min || param > max ) {
    // param out of range: check if deviation from range is significant
    double delta = 
      ( param < min ) ? (min-param)/error : (param-max)/error;
    if( StudentI(delta, ndf) < sig)  
      out = false;      
  }
  return out;
}

