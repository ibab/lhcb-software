// $Id: OMACheckXRange.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $

#include <TH1F.h>
#include "OMAlib/OMAAlgorithms.h"

OMACheckXRange::OMACheckXRange() : OMACheckAlg("CheckXRange") {
  m_npars = 2;
  m_parnames.push_back("Min");
  m_parnames.push_back("Max");
  m_ninput = 0;
  m_doc = "Check that all Histogram entries are within Min and Max";
}

void OMACheckXRange::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars) {
  input_pars.empty(); //  avoid compil. warning 
  if(warn_thresholds.size() >= m_npars &&
     alarm_thresholds.size() >= m_npars)
    exec(Histo,warn_thresholds[0],warn_thresholds[1],
         alarm_thresholds[0],alarm_thresholds[1]);
}

void OMACheckXRange::exec(TH1 &Histo,
		       float warn_min,
		       float warn_max,
		       float alarm_min,
		       float alarm_max) {
  bool ok=true;
  if (false == check(Histo,alarm_min,alarm_max) ) { // alarm on
    raiseMessage( ALARM , 
                  " entries out of range",
                  Histo.GetName());
    ok = false;
  }
  else {
    if (false == check(Histo,warn_min,warn_max) ) { // warning on
      raiseMessage( WARNING , 
                    " entries out of range",
                    Histo.GetName());
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
    if( Histo.Integral(Histo.FindBin(max),Histo.GetNbinsX()+1) > contTh) out=false;
  return out;
}
