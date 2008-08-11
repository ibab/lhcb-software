#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include <sstream>

OMACheckHolesAndSpikes::OMACheckHolesAndSpikes(OMAcommon* Env) : 
  OMACheckAlg("CheckHolesAndSpikes", Env) {
  m_ninput = 2;
  m_inputNames.push_back("Reference");
  m_inputNames.push_back("DeltaMode");
  //  for (int i=1;i<=10;i++){
  //  std::stringtream ss;
  //  ss << "MaskedBin" << i;
  //  m_parnames.push_back(ss.str());
  //}
  m_npars = 2;
  m_parnames.push_back("MinDelta");
  m_parnames.push_back("MaxDelta");
  m_doc = "Check for holes and spikes of histogram. Check is done wrt to the ref. histogram if ";
  m_doc +=  "Reference<0, or wrt a fitted polynomial of degree=Reference. Delta can be a limit on the ";
  m_doc +=  "ratio (DeltaMode=1), absolute difference (DeltaMode=2), number of sigma (DeltaMode=3).";
  m_needRef = true;
}

void OMACheckHolesAndSpikes::exec(TH1 &Histo,
				  std::vector<float> & warn_thresholds,
				  std::vector<float> & alarm_thresholds,
				  std::vector<float> & input_pars) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars 
      || input_pars.size() <m_ninput )
    return;
  
  int ref= (int)(input_pars[0] + .1 );
  DeltaMode mode = static_cast<DeltaMode> ((int)(input_pars[1] + .1 ) );
  if (mode <= MinMode || mode>= MaxMode ) return;
  
  double refValues[Histo.GetNbinsX()][Histo.GetNbinsY()];
  
  if( ref>=0 ) { //polynomial fit
    // only 1-dim histograms are supported for the moment
    if (Histo.GetDimension() > 1) return;
    std::stringstream ss;
    ss << "pol"<<ref;
    Histo.Fit(ss.str().c_str(),"Q");
    TF1 *fit = Histo.GetFunction(ss.str().c_str());
    for (int ih=1 ; ih<= Histo.GetNbinsX() ; ih++) {
      refValues[ih-1][0] = fit->Eval(Histo.GetBinCenter(ih));
    }
  }
  else if(m_ref) { // compare with reference 
    for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
      for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
        if( ihx <=  m_ref->GetNbinsX() &&
            ihy <=  m_ref->GetNbinsY() )
          refValues[ihx-1][ihy-1] = m_ref->GetBinContent(ihx,ihy);
      }
    }
  }
  else {
    return;
  }
  
  double MinDelta=9999., MaxDelta=-9999.;
  int minbin=-1,maxbin=-1;
  // now compare bin contents with reference
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
      double delta=0.;
      double content = Histo.GetBinContent(ihx,ihy);
      double &ref=refValues[ihx-1][ihy-1];
      switch(mode) {
      case Ratio:
        delta= (ref > 0.) ? content/ref : 999.;
        break;
      case AbsDiff:
        delta = content - ref;
        break;
      case SigmaDiff:
        double err = Histo.GetBinError(ihx,ihy);
        // assuming error=1.2 in case of 0 content 
        delta= (err > 0.) ? (content - ref)/err : (content - ref)/1.2;
        break;
      default:
        break;
      }
      if(delta < MinDelta) {
        MinDelta=delta;
        minbin = Histo.GetBin(ihx,ihy);
      }
      if(delta > MaxDelta) {
        MaxDelta=delta;
        maxbin = Histo.GetBin(ihx,ihy);
      }
    }
  }
  
  std::stringstream message;
  if ( MinDelta < alarm_thresholds[0]) {
    message << "Hole detected for bin "<<minbin;
    raiseMessage( OMAMsgInterface::ALARM , message.str(), Histo.GetName());
  }
  else if( MinDelta < warn_thresholds[0]) {
    message << "Hole detected for bin "<<minbin;
    raiseMessage( OMAMsgInterface::WARNING , message.str(), Histo.GetName());
  }

  if ( MaxDelta > alarm_thresholds[1]) {
    message << "Spike detected for bin "<<maxbin;
    raiseMessage( OMAMsgInterface::ALARM , message.str(), Histo.GetName());
  }
  else if ( MaxDelta > warn_thresholds[1]) {
    message << "Spike detected for bin "<<maxbin;
    raiseMessage( OMAMsgInterface::WARNING , message.str(), Histo.GetName());
  }
}
