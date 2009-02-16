// $Id: OMAGaussFit.cpp,v 1.5 2009-02-16 10:38:21 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
using namespace TMath;

OMAGaussFit::OMAGaussFit(OMAcommon* Env) : 
  OMACheckAlg("GaussFit", Env) {
  m_npars = 4;
  m_parnames.push_back("MinMean"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxMean"); m_parDefValues.push_back(+999999.);
  m_parnames.push_back("MinSigma"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxSigma"); m_parDefValues.push_back(+999999.);
  m_ninput = 1;
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(.95);
  m_doc = "Fit histogram with a gaussian and check that average and sigma are in the specified ranges";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMAGaussFit::exec(TH1 &Histo,
                       std::vector<float> & warn_thresholds,
                       std::vector<float> & alarm_thresholds,
                       std::vector<float> & input_pars,
                       unsigned int anaID,
                       TH1* Ref) {
  float confidence=0.95;
  Ref = NULL; // avoid compil. warning
  std::string message(" Mean out of range");
  std::string hname(Histo.GetName());

  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;

  if ( input_pars.size() > 0 )
    confidence = input_pars[0];

  
  Histo.Fit("gaus","Q");
  TF1 *fit = Histo.GetFunction("gaus");
  if (fit) {
    if (false == checkParam( fit,1,
                             alarm_thresholds[0],
                             alarm_thresholds[1],
                             confidence) ) { // alarm on
    raiseMessage( anaID,
                  OMAMessage::ALARM , 
		  message, hname);
    }
    else {
      if (false == checkParam( fit,1,
                               warn_thresholds[0],
                               warn_thresholds[1],
                               confidence) ) { // warning on
        raiseMessage( anaID,
                      OMAMessage::WARNING , 
                      message, hname);
      }
    }
    message=" Sigma out of range";
    if (false == checkParam( fit,2,
                             alarm_thresholds[2],
                             alarm_thresholds[3],
                             confidence) ) { // alarm on
      raiseMessage( anaID,
                    OMAMessage::ALARM , 
		    message, hname);
    }
    else {
      if (false == checkParam( fit,2,
                               warn_thresholds[2],
                               warn_thresholds[3],
                               confidence) ) { // warning on
        raiseMessage( anaID,
                      OMAMessage::WARNING , 
		      message, hname);
      }
    }
  }
}

bool OMAGaussFit::checkParam( TF1* fit,
                              int ipar,
                              float min,
                              float max,
                              float conf) {
  bool out = true;
  int ndf= fit->GetNDF();
  if (ndf>0) {
    double param= fit->GetParameter(ipar);
    double error= fit->GetParError(ipar);
    if( param < min || param > max ) {
      // param out of range: check if deviation from range is significant
      double delta = 
        ( param < min ) ? (min-param)/error : (param-max)/error;
      if( StudentI(delta, ndf) > conf)  
        out = false;      
    }
  }
  return out;
}

