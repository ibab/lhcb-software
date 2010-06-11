// $Id: OMACheckMinMax.cpp,v 1.1 2010-06-11 13:00:10 ggiacomo Exp $

#include <TH1F.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"

OMACheckMinMax::OMACheckMinMax(OMAlib* Env) : 
  OMACheckAlg("CheckMinMax", Env) {
  m_ninput = 1;
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(0.95);
  m_npars = 2;
  m_parnames.push_back("Min"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("Max"); m_parDefValues.push_back(+999999.);
  m_doc = "Check that all bin contents are within Min and Max with the given confidence level.";
  m_minEntriesPerBin =0; // could be used also to check low content
}

void OMACheckMinMax::exec(TH1 &Histo,
                          std::vector<float> & warn_thresholds,
                          std::vector<float> & alarm_thresholds,
                          std::vector<float> & input_pars,
                          unsigned int anaID,
                          TH1* ) {
  float confidence=m_inputDefValues[0];
  if ( input_pars.size() > 0 ) confidence = input_pars[0];
  
// get bin labels if available
  bool  hasLabels=getBinLabels(Histo);

  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  

  std::stringstream message;
  std::string hname(Histo.GetName());
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  m_k2c = content2counts(Histo);
  int na=0,nw=0;
  m_badbins.clear();

  for (int ix=1; ix<= Histo.GetNbinsX(); ix++) {
    for (int iy=1 ; iy<= Histo.GetNbinsY(); iy++) {
      if (false == checkContent(Histo,ix,iy,alarm_thresholds[0],alarm_thresholds[1],
                                confidence) ) { // alarm on
        level = OMAMessage::ALARM;
        na++;
      }
      else {
        if (false == checkContent(Histo,ix,iy,warn_thresholds[0],warn_thresholds[1],
                                  confidence) ) { // warning on
          level = OMAMessage::WARNING;
          nw++;
        }
      }
    }
  }

  if (level > OMAMessage::INFO) {
    if (na>0) {
      message << "there are "<<na<< " bins out of alarm range (" <<alarm_thresholds[0]<<
        " - "<< alarm_thresholds[1]<<")";
      if (nw) message << " and ";
    }
    if (nw) {
      if (na) message << " and ";
      else  message << " there are ";
      message <<nw << " bins out of warning range (" <<warn_thresholds[0]<<
        " - "<< warn_thresholds[1]<<")";
    }
    message << ": ";
    for (unsigned int ib=0; ib<m_badbins.size(); ib++) {
      if (ib>0) message << " ,";
      if (hasLabels) {
        message << Histo.GetXaxis()->GetBinLabel(m_badbins[ib]);
      }
      else {
        message << m_badbins[ib];
      }
      
    }
    raiseMessage( anaID,
                  level , 
                  message.str(), 
                  hname);
  }
}
  
bool OMACheckMinMax::checkContent(TH1 &Histo, int ix, int iy, float min, float max,
                                  float conf) {
  bool out = true;
  double value=Histo.GetBinContent(ix,iy);
  if( value < min || value > max ) {
    // rms out of range: check if deviation from range is significant
    int entr = (int) (m_k2c * value +0.1);
    double err=	Histo.GetBinError(ix,iy);
    double delta = 
      ( value < min ) ? (min-value)/err : (value-max)/err;
    // check probability to observe such discrepancy if real value was at threshold
    if( TMath::StudentI(delta, entr-1) > conf ) {
      out = false;     
      if (value < min && Histo.TestBit(TH1::kIsAverage) == 0 && entr<50) {
        // check that ROOT's naif poisson errors are not cheating us (we could have 0 entries in this bin -> "0" error)
        double minentries= m_k2c * min;
        double pprob=0.;
        for (int i=0; i<= entr; i++) {
          pprob += TMath::PoissonI(i,minentries);
        }
        if (pprob > (1-conf)) out = true; // compatible with Poisson fluctuation 
      }

    }
  }

  if (false == out) {
    m_badbins.push_back(Histo.GetBin(ix,iy));
  }
  return out;
}
