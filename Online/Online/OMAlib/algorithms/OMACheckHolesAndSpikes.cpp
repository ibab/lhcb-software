#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include <sstream>
#include <cmath>

OMACheckHolesAndSpikes::OMACheckHolesAndSpikes(OMAlib* Env) : 
  OMACheckAlg("CheckHolesAndSpikes", Env) {
  m_ninput = 3;
  m_inputNames.push_back("Reference");  m_inputDefValues.push_back(0.);
  m_inputNames.push_back("DeltaMode");  m_inputDefValues.push_back(3.);
  m_inputNames.push_back("Normalize");  m_inputDefValues.push_back(1.);

  //  for (int i=1;i<=10;i++){
  //  std::stringtream ss;
  //  ss << "MaskedBin" << i;
  //  m_parnames.push_back(ss.str());
  //}
  m_npars = 2;
  m_parnames.push_back("MinDelta"); m_parDefValues.push_back(-999999.);
  m_parnames.push_back("MaxDelta"); m_parDefValues.push_back(+999999.);
  m_doc = "Check for holes and spikes of histogram against the reference histogram (if ";
  m_doc +=  "Reference<0) or against a fitted polynomial of degree=Reference. Delta can be a limit on the ";
  m_doc +=  "ratio (DeltaMode=1), absolute difference (DeltaMode=2), number of sigma (DeltaMode=3).";
  m_doc +=  "Set Normalize to 0 if you do not want the ref histogram to be normalized to the online histogram";
  m_needRef = true;
}

void OMACheckHolesAndSpikes::exec(TH1 &Histo,
                                  std::vector<float> & warn_thresholds,
                                  std::vector<float> & alarm_thresholds,
                                  std::vector<float> & input_pars,
                                  unsigned int anaID,
                                  TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars 
      || input_pars.size() <m_ninput )
    return;
  if (Histo.GetEntries() == 0)
    return; // do nothing for empty histograms
  if (input_pars[0] < 0.) input_pars[0]= -99.;
  int ipoly= (int)(input_pars[0] + .1 );
  DeltaMode mode = static_cast<DeltaMode> ((int)(input_pars[1] + .1 ) );
  bool refNormalize = (input_pars[2] < 0.1) ? false : true;
  if (mode <= MinMode || mode>= MaxMode ) return;
  
  std::vector<double> refValues,refErrors;
  int nBx = Histo.GetNbinsX();
  int nBy = Histo.GetNbinsY();

  refValues.resize(nBx * nBy);
  refErrors.resize(nBx * nBy);

  if( ipoly>=0 ) { //polynomial fit
    // only 1-dim histograms are supported 
    if (Histo.GetDimension() > 1) return;
    std::stringstream ss;
    ss << "pol"<<ipoly;
    Histo.Fit(ss.str().c_str(),"Q");
    TF1 *fit = Histo.GetFunction(ss.str().c_str());
    for (int ih=1 ; ih<= nBx ; ih++) {
      refValues[ih-1] = fit->Eval(Histo.GetBinCenter(ih));
      refErrors[ih-1] = 0.;
    }
  }
  else if(Ref) { // compare with reference 
    // normalize Ref to current histogram if requested
    if (refNormalize) {
      Ref->Scale(Histo.Integral() / Ref->Integral() );
    }
    for (int ihx=1 ; ihx<= nBx ; ihx++) {
      for (int ihy=1 ; ihy<= nBy ; ihy++) {
        if( ihx <=  Ref->GetNbinsX() &&
            ihy <=  Ref->GetNbinsY() ) {
          refValues[(ihx-1)*nBy + (ihy-1)] = Ref->GetBinContent(ihx,ihy);
          refErrors[(ihx-1)*nBy + (ihy-1)] = Ref->GetBinError(ihx,ihy);
        }
      }
    }
  }
  else {
    return;
  }
  
  double MinDelta=9999., MaxDelta=-9999.;
  int minbin=-1,maxbin=-1;
  // now compare bin contents with reference
  for (int ihx=1 ; ihx<= nBx ; ihx++) {
    for (int ihy=1 ; ihy<= nBy ; ihy++) {
      double delta=0.,err=0.;
      double content = Histo.GetBinContent(ihx,ihy);
      double &xref=refValues[(ihx-1)*nBy +(ihy-1)];
      double &xrefer=refErrors[(ihx-1)*nBy +(ihy-1)];
      switch(mode) {
      case Ratio:
        delta= (xref>OMAconstants::epsilon) ? content/xref : 99.;
        break;
      case AbsDiff:
        delta = content - xref;
        break;
      case SigmaDiff:
        err = Histo.GetBinError(ihx,ihy);
        // assuming error=1.2 in case of 0 content 
        if (err<OMAconstants::epsilon) err=1.2;
        // add error from reference
        err = sqrt( pow(err, 2) + pow(xrefer,2));
        delta= (content - xref)/err;
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
  std::string hname(Histo.GetName());
  OMAMessage::OMAMsgLevel level=OMAMessage::INFO;
  if ( MinDelta < alarm_thresholds[0]) {
    message << "Hole detected for x="<<Histo.GetBinCenter(minbin)<< " delta="<<MinDelta;
    level= OMAMessage::ALARM;
  }
  else if( MinDelta < warn_thresholds[0]) {
    message << "Hole detected for x="<<Histo.GetBinCenter(minbin)<< " delta="<<MinDelta;
    level= OMAMessage::WARNING;
  }

  if ( MaxDelta > alarm_thresholds[1]) {
    if (level > OMAMessage::INFO) message << std::endl;
    message << "Spike detected for x="<<Histo.GetBinCenter(maxbin)<< " delta="<<MaxDelta;
    level= OMAMessage::ALARM;
  }
  else if ( MaxDelta > warn_thresholds[1]) {
    if (level > OMAMessage::INFO) message << std::endl;
    message << "Spike detected for x="<<Histo.GetBinCenter(maxbin)<< " delta="<<MaxDelta;
    level= OMAMessage::WARNING;
  }

  if(level > OMAMessage::INFO) {
    raiseMessage(anaID, level, message.str(), hname);
  }
}

bool OMACheckHolesAndSpikes::refMissing(TH1* ref,
                                       std::vector<float> & input_pars) {
  if(input_pars.size() <m_ninput) return false; 
  return (input_pars[0] < 0. &&  !ref);
}
