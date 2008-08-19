// $Id: OMACheckXRange.cpp,v 1.3 2008-08-19 22:45:32 ggiacomo Exp $

#include <TH1F.h>
#include "OMAlib/OMAAlgorithms.h"

OMACheckXRange::OMACheckXRange(OMAcommon* Env) : 
  OMACheckAlg("CheckXRange", Env) {
  m_npars = 2;
  m_parnames.push_back("Min"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("Max"); m_parDefValues.push_back(+999999.);
  m_ninput = 0;
  m_doc = "Check that all Histogram entries are within Min and Max";
}

void OMACheckXRange::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars,
                          unsigned int anaID,
                          std::string& taskName,
                          TH1* Ref) {
  input_pars.empty(); //  avoid compil. warning 
  Ref = NULL; //  avoid compil. warning 
  if(warn_thresholds.size() >= m_npars &&
     alarm_thresholds.size() >= m_npars)
    exec(Histo,warn_thresholds[0],warn_thresholds[1],
         alarm_thresholds[0],alarm_thresholds[1], anaID, taskName);
}

void OMACheckXRange::exec(TH1 &Histo,
                          float warn_min,
                          float warn_max,
                          float alarm_min,
                          float alarm_max,
                          unsigned int anaID,
                          std::string& taskName) {
  bool ok=true;
  std::string s1(" entries out of range");
  std::string s2(Histo.GetName());

  if (false == check(Histo,alarm_min,alarm_max) ) { // alarm on
    raiseMessage( anaID,
                  OMAMessage::ALARM , 
                  s1,s2, taskName);
    ok = false;
  }
  else {
    if (false == check(Histo,warn_min,warn_max) ) { // warning on
      raiseMessage( anaID,
                    OMAMessage::WARNING , 
                    s1,s2, taskName);
      ok = false;
    }
  }
}

bool OMACheckXRange::check(TH1 &Histo,
                           float min,
                           float max) {
  bool out = true;
  static const double contTh = 0.0001;
  if( Histo.Integral(0,Histo.FindBin(min)) > contTh) out=false;
  if(out)
    if( Histo.Integral(Histo.FindBin(max),Histo.GetNbinsX()+1) > contTh ) out=false;
  return out;
}
