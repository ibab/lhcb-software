#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include <sstream>

OMACheckHolesAndSpikes::OMACheckHolesAndSpikes(OMAcommon* Env) : 
  OMACheckAlg("CheckHolesAndSpikes", Env) {
  m_ninput = 2;
  m_inputNames.push_back("Reference");  m_inputDefValues.push_back(0.);
  m_inputNames.push_back("DeltaMode");  m_inputDefValues.push_back(3.);
  
  //  for (int i=1;i<=10;i++){
  //  std::stringtream ss;
  //  ss << "MaskedBin" << i;
  //  m_parnames.push_back(ss.str());
  //}
  m_npars = 2;
  m_parnames.push_back("MinDelta"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxDelta"); m_parDefValues.push_back(+999999.);
  m_doc = "Check for holes and spikes of histogram. Check is done wrt to the ref. histogram if ";
  m_doc +=  "Reference<0, or wrt a fitted polynomial of degree=Reference. Delta can be a limit on the ";
  m_doc +=  "ratio (DeltaMode=1), absolute difference (DeltaMode=2), number of sigma (DeltaMode=3).";
  m_needRef = true;
}

void OMACheckHolesAndSpikes::exec(TH1 &Histo,
                                  std::vector<float> & warn_thresholds,
                                  std::vector<float> & alarm_thresholds,
                                  std::vector<float> & input_pars,
                                  unsigned int anaID,
                                  std::string& taskName,
                                  TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars 
      || input_pars.size() <m_ninput )
    return;
  
  int ipoly= (int)(input_pars[0] + .1 );
  DeltaMode mode = static_cast<DeltaMode> ((int)(input_pars[1] + .1 ) );
  if (mode <= MinMode || mode>= MaxMode ) return;
  
  double refValues[Histo.GetNbinsX()][Histo.GetNbinsY()];
  
  if( ipoly>=0 ) { //polynomial fit
    // only 1-dim histograms are supported for the moment
    if (Histo.GetDimension() > 1) return;
    std::stringstream ss;
    ss << "pol"<<ipoly;
    Histo.Fit(ss.str().c_str(),"Q");
    TF1 *fit = Histo.GetFunction(ss.str().c_str());
    for (int ih=1 ; ih<= Histo.GetNbinsX() ; ih++) {
      refValues[ih-1][0] = fit->Eval(Histo.GetBinCenter(ih));
    }
  }
  else if(Ref) { // compare with reference 
    for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
      for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
        if( ihx <=  Ref->GetNbinsX() &&
            ihy <=  Ref->GetNbinsY() )
          refValues[ihx-1][ihy-1] = Ref->GetBinContent(ihx,ihy);
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
      double &xref=refValues[ihx-1][ihy-1];
      switch(mode) {
      case Ratio:
	// take the ratio, except for cases where both values are very small 
	delta=1.;
	if (xref < 0.1 ) {
	  if (content > 0.99)
	    delta= (xref>0.01) ? content/xref : 99.;
	}
	else {
	  if (content > 0.1) 
	    delta = content/xref;
	  else if(xref > 0.99)
	    delta= (content>0.01) ? content/xref : .01;
	}
        break;
      case AbsDiff:
        delta = content - xref;
        break;
      case SigmaDiff:
        double err = Histo.GetBinError(ihx,ihy);
        // assuming error=1.2 in case of 0 content 
        delta= (err > 0.) ? (content - xref)/err : (content - xref)/1.2;
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
  
  std::stringstream message,message2;
  std::string hname(Histo.GetName());
  if ( MinDelta < alarm_thresholds[0]) {
    message << "Hole detected for x="<<Histo.GetBinCenter(minbin)<< " delta="<<MinDelta;
    raiseMessage( anaID, OMAMessage::ALARM , message.str(), hname, taskName);
  }
  else if( MinDelta < warn_thresholds[0]) {
    message << "Hole detected for x="<<Histo.GetBinCenter(minbin)<< " delta="<<MinDelta;
    raiseMessage( anaID, OMAMessage::WARNING , message.str(), hname, taskName);
  }

  if ( MaxDelta > alarm_thresholds[1]) {
    message2 << "Spike detected for x="<<Histo.GetBinCenter(maxbin)<< " delta="<<MaxDelta;
    raiseMessage( anaID, OMAMessage::ALARM , message2.str(), hname, taskName);
  }
  else if ( MaxDelta > warn_thresholds[1]) {
    message2 << "Spike detected for x="<<Histo.GetBinCenter(maxbin)<< " delta="<<MaxDelta;
    raiseMessage( anaID, OMAMessage::WARNING , message2.str(), hname, taskName);
  }
}
