// $Id: OMAGaussFit.cpp,v 1.6 2009-04-02 10:27:25 ggiacomo Exp $

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

  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;

  std::stringstream message;
  std::string hname(Histo.GetName());
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
 
  if ( input_pars.size() > 0 )
    confidence = input_pars[0];

  
  Histo.Fit("gaus","Q");
  TF1 *fit = Histo.GetFunction("gaus");
  if (fit) {
    if (false == checkParam( fit,1,
                             alarm_thresholds[0],
                             alarm_thresholds[1],
                             confidence, message) ) { // alarm on
      level = OMAMessage::ALARM;
    }
    else {
      if (false == checkParam( fit,1,
                               warn_thresholds[0],
                               warn_thresholds[1],
                               confidence, message) ) { // warning on
        level = OMAMessage::WARNING;
      }
    }
    if (false == checkParam( fit,2,
                             alarm_thresholds[2],
                             alarm_thresholds[3],
                             confidence, message) ) { // alarm on
      level = OMAMessage::ALARM;
    }
    else {
      if (false == checkParam( fit,2,
                               warn_thresholds[2],
                               warn_thresholds[3],
                               confidence, message) ) { // warning on
        level = OMAMessage::WARNING;
      }
    }
  }

  if (level > OMAMessage::INFO) {
    raiseMessage( anaID,
                  level , 
                  message.str(), 
                  hname);
  }
}

bool OMAGaussFit::checkParam( TF1* fit,
                              int ipar,
                              float min,
                              float max,
                              float conf,
                              std::stringstream &mess) {
  bool out = true;
  int ndf= fit->GetNDF();
  double param=0., error=0.;
  if (ndf>0) {
    param= fit->GetParameter(ipar);
    error= fit->GetParError(ipar);
    if( param < min || param > max ) {
      // param out of range: check if deviation from range is significant
      double delta = 
        ( param < min ) ? (min-param)/error : (param-max)/error;
      if( StudentI(delta, ndf) > conf)  
        out = false;      
    }
  }
  if (false == out) {
    if (! mess.str().empty()) {
      mess << std::endl;
    }
    mess << fit->GetParName(ipar) << " is "<<param <<
      " +/- "<< error << " (allowed range is "
         << min << " - " << max << " )";
  }

  return out;
}

