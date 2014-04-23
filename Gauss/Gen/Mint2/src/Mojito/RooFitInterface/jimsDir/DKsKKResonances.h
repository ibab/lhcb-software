#ifndef DKsKKResonances_HH
#define DKsKKResonances_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "TMath.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "DcyAmplitude.h"
#include "DcyCoupledAmplitude.h"
#include "DcyNonRes.h"

namespace DKsKKResonances{

  Double_t PrimDzMass = 1.8645;
  Double_t deg = TMath::Pi()/180.0;

  RooRealVar ScalarSpinFactor("ScalarSpinFactor", "ScalarSpinFactor", 1.0);

  RooRealVar DZeroM   ("DZeroM" , "DZeroMass", PrimDzMass);
  RooRealVar KPlusM   ("KPlusM" , "KMass", 0.493677);
  RooRealVar KMinusM  ("KMinusM", "KMass", 0.493677);
  RooRealVar KsM  ("KsM", "KsMass", 0.497648);
  RooRealVar PiZeroM  ("PiZeroM", "PiZeroMass",0.134977);
  RooRealVar PiPlusM  ("PiPlusM", "PiPlusMass",0.139570);
  RooRealVar EtaM  ("EtaM", "EtaMass",0.54751);
  
  RooRealVar Radius_r ("Radius_r", "Radius_r", 1.5);

  RooRealVar a0z_980A ("a0z_980A","a0z_980Amplitude", 1.0);
  RooRealVar a0z_980P ("a0z_980P","a0z_980Phase",   0.0  );
  RooRealVar a0z_980M ("a0z_980M","a0z_980Mass", 0.999);
  RooRealVar a0z_980Getapi ("a0z_980Getapi","a0z_980Getapi", 0.324);
  RooRealVar a0z_980GKK ("a0z_980GKK","a0z_980GKK", 0.329);
  RooRealVar a0z_980J ("a0z_980J","a0z_980J", 0);

  RooRealVar phiA ("phiA","phiAmplitude", 0.437);
  RooRealVar phiP ("phiP","phiPhase",     1.91);
  RooRealVar phiM ("phiM","phiMass", 1.01963);
  RooRealVar phiW ("phiW","phiWidth", 0.00428);

  RooRealVar a0p_980A ("a0p_980A","a0p_980Amplitude", 0.460);
  RooRealVar a0p_980P ("a0p_980P","a0p_980Phase",   3.59  );
  RooRealVar a0p_980M ("a0p_980M","a0p_980Mass", 0.999);
  RooRealVar a0p_980Getapi ("a0p_980Getapi","a0p_980Getapi", 0.324);
  RooRealVar a0p_980GKK ("a0p_980GKK","a0p_980GKK", 0.329);


  RooRealVar f0_1400A ("f0_1400A","f0_1400Amplitude", 0.435);
  RooRealVar f0_1400P ("f0_1400P","f0_1400Phase",     -2.63);
  RooRealVar f0_1400M ("f0_1400M","f0_1400Mass", 1.391);
  RooRealVar f0_1400W ("f0_1400W","f0_1400Width", 0.055);

}

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix=""
			  );
#endif
//
