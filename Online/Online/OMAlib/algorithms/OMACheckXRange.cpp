// $Id: OMACheckXRange.cpp,v 1.8 2010-06-11 13:00:10 ggiacomo Exp $

#include <TH1F.h>
#include "OMAlib/OMAAlgorithms.h"

OMACheckXRange::OMACheckXRange(OMAlib* Env) : 
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
                          TH1*) {
  input_pars.empty(); //  avoid compil. warning 
  if(warn_thresholds.size() >= m_npars &&
     alarm_thresholds.size() >= m_npars)
    exec(Histo,warn_thresholds[0],warn_thresholds[1],
         alarm_thresholds[0],alarm_thresholds[1], anaID);
}

void OMACheckXRange::exec(TH1 &Histo,
                          float warn_min,
                          float warn_max,
                          float alarm_min,
                          float alarm_max,
                          unsigned int anaID) {
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  std::string hname(Histo.GetName());
  std::stringstream message;

  if (findBadEntries(Histo,alarm_min,alarm_max,message) ) {
    level = OMAMessage::ALARM;
  }
  else if (findBadEntries(Histo,warn_min,warn_max,message) ) {
    level = OMAMessage::WARNING;
  }
  if (level > OMAMessage::INFO) {
    raiseMessage( anaID,
                  level , 
                  message.str(), 
                  hname);
  }
}

bool OMACheckXRange::findBadEntries(TH1 &Histo,
                                     float min,
                                     float max,
                                     std::stringstream &mess) {
  bool out=false;
  static const double contTh = 0.0001;

  double be = Histo.Integral(0,Histo.FindBin(min)) +
    Histo.Integral(Histo.FindBin(max),Histo.GetNbinsX()+1);
  if (be > contTh) {
    out = true;
    mess << "There are "<< be << " entries outside the allowed range "
         << min << " to "<<max;
  }
  return out;
}

