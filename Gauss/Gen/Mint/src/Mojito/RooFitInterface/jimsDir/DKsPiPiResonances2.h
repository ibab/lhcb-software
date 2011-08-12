#ifndef DKsPiPiResonances_HH
#define DKsPiPiResonances_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT

#include "TMath.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "DcyAmplitude.h"
#include "DcyGSAmplitude.h"
#include "DcyNonRes.h"
#include "SpinFactorThreeBody.h"
#include "SpinFactors.h"

/*#include "KdcyAmplitude.h"
#include "Kmkrho.C"
#include "TArrayI.h"
#include "Kintgrt.h"
#include "rhofunc.h"
#include <complex>
#include "TArrayD.h"
#include "Kamp.h"
#include "KmkAmpR.C"
#include "KmkAmpI.C"*/
namespace DKsPiPiResonances{

  Double_t PrimDzMass = 1.8645;
  Double_t deg = TMath::Pi()/180.0;

  RooRealVar ScalarSpinFactor("ScalarSpinFactor", "ScalarSpinFactor", 1.0);

  RooRealVar DZeroM   ("DZeroM" , "DZeroMass", PrimDzMass);
  RooRealVar piPlusM   ("piPlusM" , "piMass", 0.13957018);
  RooRealVar piMinusM  ("piMinusM", "piMass", 0.13957018);
  RooRealVar KsM  ("KsM", "KsMass", 0.497648);

  RooRealVar Radius_r ("Radius_r", "Radius_r", 1.5);

  RooRealVar rhoA ("rhoA","rhoAmplitude", 1.0);
  RooRealVar rhoP ("rhoP","rhoPhase",   0.0  );
  RooRealVar rhoM ("rhoM","rhoMass", 0.7758);
  RooRealVar rhoW ("rhoW","rhoWidth", 0.1464);

  RooRealVar omegaA ("omegaA","omegaAmplitude", 0.0451);
  RooRealVar omegaP ("omegaP","omegaPhase",     119.1*deg);
  RooRealVar omegaM ("omegaM","omegaMass", 0.7826);
  RooRealVar omegaW ("omegaW","omegaWidth", 0.0085);

  RooRealVar sigma1A ("sigma1A","sigma1Amplitude", 1.308);
  RooRealVar sigma1P ("sigma1P","sigma1Phase",     -168.0*deg);
  RooRealVar sigma1M ("sigma1M","sigma1Mass", 0.490);
  RooRealVar sigma1W ("sigma1W","sigma1Width", 0.406);

  RooRealVar sigma2A ("sigma2A","sigma2Amplitude", 0.297);
  RooRealVar sigma2P ("sigma2P","sigma2Phase",     167.3*deg);
  RooRealVar sigma2M ("sigma2M","sigma2Mass", 1.024);
  RooRealVar sigma2W ("sigma2W","sigma2Width", 0.089);

  RooRealVar f0_980A ("f0_980A","f0_980Amplitude", 0.515);
  RooRealVar f0_980P ("f0_980P","f0_980Phase",     -150.1*deg);
  RooRealVar f0_980M ("f0_980M","f0_980Mass", 0.975);
  RooRealVar f0_980W ("f0_980W","f0_980Width", 0.044); // badly known. PDG: 40-100 MeV
  RooRealVar f2_1270A ("f2_1270A","f2_1270Amplitude", 0.799);
  RooRealVar f2_1270P ("f2_1270P","f2_1270Phase",     -28.97*deg);
  RooRealVar f2_1270M ("f2_1270M","f2_1270Mass", 1.2754);
  RooRealVar f2_1270W ("f2_1270W","f2_1270Width", 0.1851);

  RooRealVar f0_1370A ("f0_1370A","f0_1370Amplitude", 1.877);
  RooRealVar f0_1370P ("f0_1370P","f0_1370Phase",     120.4*deg);
  RooRealVar f0_1370M ("f0_1370M","f0_1370Mass", 1.434); // badly known, need to understand imag part.
  RooRealVar f0_1370W ("f0_1370W","f0_1370Width", 0.173);

  RooRealVar rho1450A ("rho1450A","rho1450Amplitude",0.256);
  RooRealVar rho1450P ("rho1450P","rho1450Phase",8.1*deg);
  RooRealVar rho1450M ("rho1450M","rho1450Mass",1.406);
  RooRealVar rho1450W ("rho1450W","rho1450Width",0.455);

  RooRealVar Kstar892A ("Kstar892A","Kstar892Amplitude", 1.819);
  RooRealVar Kstar892P ("Kstar892P","Kstar892Phase",     132.3*deg);
  RooRealVar Kstar892ACs ("Kstar892ACs","Kstar892 Cab-sup Amplitude", 0.162);
  RooRealVar Kstar892PCs ("Kstar892PCs","Kstar892 Cab-sup Phase", -51.9*deg);
  RooRealVar Kstar892M ("Kstar892M","Kstar892Mass", 0.89166);
  RooRealVar Kstar892W ("Kstar892W","Kstar892Width", 0.0508);

  RooRealVar Kstar_1410A ("Kstar_1410A","Kstar_1410Amplitude", 0.270);
  RooRealVar Kstar_1410P ("Kstar_1410P","Kstar_1410Phase",     -156.5*deg);
  RooRealVar Kstar_1410M ("Kstar_1410M","Kstar_1410Mass", 1.414);
  RooRealVar Kstar_1410W ("Kstar_1410W","Kstar_1410Width", 0.232);

  RooRealVar Kstar0_1430A ("Kstar0_1430A","Kstar0_1430Amplitude", 1.793);
  RooRealVar Kstar0_1430P ("Kstar0_1430P","Kstar0_1430Phase",     18.7*deg);
  RooRealVar Kstar0_1430ACs ("Kstar0_1430ACs","Kstar0_1430 Cab-sup Amplitude", 0.0807);
  RooRealVar Kstar0_1430PCs ("Kstar0_1430PCs","Kstar0_1430 Cab-sup Phase",     70.4*deg);
  RooRealVar Kstar0_1430M ("Kstar0_1430M","Kstar0_1430Mass", 1.459);
  RooRealVar Kstar0_1430W ("Kstar0_1430W","Kstar0_1430Width", 0.175);

  
  RooRealVar Kstar2_1430A ("Kstar2_1430A","Kstar2_1430Amplitude", 1.250);
  RooRealVar Kstar2_1430P ("Kstar2_1430P","Kstar2_1430Phase",     -48.1*deg);
  RooRealVar Kstar2_1430ACs ("Kstar2_1430ACs","Kstar2_1430 Cab-sup Amplitude", 0.178);
  RooRealVar Kstar2_1430PCs ("Kstar2_1430PCs","Kstar2_1430 Cab-sup Phase",-96.13*deg);
  RooRealVar Kstar2_1430M ("Kstar2_1430M","Kstar2_1430Mass", 1.4256);
  RooRealVar Kstar2_1430W ("Kstar2_1430W","Kstar2_1430Width", 0.0985);

  RooRealVar Kstar1680A ("Kstar1680A","Kstar1680Amplitude", 1.301);
  RooRealVar Kstar1680P ("Kstar1680P","Kstar1680Phase",     170.9*deg);
  RooRealVar Kstar1680M ("Kstar1680M","Kstar1680Mass", 1.717);
  RooRealVar Kstar1680W ("Kstar1680W","Kstar1680Width", 0.322);
  
  RooRealVar NonResA ("NonResA","NonResAmplitude",3.946);
  RooRealVar NonResP ("NonResP","NonResPhase",104.5*deg);
  /*
  RooRealVar rhoA ("rhoA","rhoAmplitude", 1.0);
  RooRealVar rhoP ("rhoP","rhoPhase",   0.0*deg  );
  RooRealVar rhoM ("rhoM","rhoMass", 0.7758);
  RooRealVar rhoW ("rhoW","rhoWidth", 0.1464);

  RooRealVar omegaA ("omegaA","omegaAmplitude", 0.04048);
  RooRealVar omegaP ("omegaP","omegaPhase",     122.6*deg);
  RooRealVar omegaM ("omegaM","omegaMass", 0.7826);
  RooRealVar omegaW ("omegaW","omegaWidth", 0.0085);

  RooRealVar f2_1270A ("f2_1270A","f2_1270Amplitude", 0.9369);
  RooRealVar f2_1270P ("f2_1270P","f2_1270Phase",     -26.7*deg);
  RooRealVar f2_1270M ("f2_1270M","f2_1270Mass", 1.2754);
  RooRealVar f2_1270W ("f2_1270W","f2_1270Width", 0.1851); 
  
  RooRealVar rho1450A ("rho1450A","rho1450Amplitude",0.3629);
  RooRealVar rho1450P ("rho1450P","rho1450Phase",-3.95*deg);
  RooRealVar rho1450M ("rho1450M","rho1450Mass",1.406);
  RooRealVar rho1450W ("rho1450W","rho1450Width",0.455);

  RooRealVar Kstar892A ("Kstar892A","Kstar892Amplitude", 1.7740);
  RooRealVar Kstar892P ("Kstar892P","Kstar892Phase",     129.5*deg);
  RooRealVar Kstar892ACs ("Kstar892ACs","Kstar892 Cab-sup Amplitude", 0.1788);
  RooRealVar Kstar892PCs ("Kstar892PCs","Kstar892 Cab-sup Phase", -49.2*deg);
  RooRealVar Kstar892M ("Kstar892M","Kstar892Mass", 0.89166);
  RooRealVar Kstar892W ("Kstar892W","Kstar892Width", 0.0508);
  
  RooRealVar Kstar_1410A ("Kstar_1410A","Kstar_1410Amplitude", 0.1032);
  RooRealVar Kstar_1410P ("Kstar_1410P","Kstar_1410Phase",     158.5*deg);
  RooRealVar Kstar_1410M ("Kstar_1410M","Kstar_1410Mass", 1.414);
  RooRealVar Kstar_1410W ("Kstar_1410W","Kstar_1410Width", 0.232);
  
  RooRealVar Kstar0_1430A ("Kstar0_1430A","Kstar0_1430Amplitude", 2.5492);
  RooRealVar Kstar0_1430P ("Kstar0_1430P","Kstar0_1430Phase",     -16.2*deg);
  RooRealVar Kstar0_1430ACs ("Kstar0_1430ACs","Kstar0_1430 Cab-sup Amplitude", 0.3994);
  RooRealVar Kstar0_1430PCs ("Kstar0_1430PCs","Kstar0_1430 Cab-sup Phase", -21.8*deg);
  RooRealVar Kstar0_1430M ("Kstar0_1430M","Kstar0_1430Mass", 1.412);
  RooRealVar Kstar0_1430W ("Kstar0_1430W","Kstar0_1430Width", 0.294);
  
  RooRealVar Kstar2_1430A ("Kstar2_1430A","Kstar2_1430Amplitude", 1.0989);
  RooRealVar Kstar2_1430P ("Kstar2_1430P","Kstar2_1430Phase",     -45.6*deg);
  RooRealVar Kstar2_1430ACs ("Kstar2_1430ACs","Kstar2_1430 Cab-sup Amplitude", 0.0702);
  RooRealVar Kstar2_1430PCs ("Kstar2_1430PCs","Kstar2_1430 Cab-sup Phase",-41.0*deg);
  RooRealVar Kstar2_1430M ("Kstar2_1430M","Kstar2_1430Mass", 1.4256);
  RooRealVar Kstar2_1430W ("Kstar2_1430W","Kstar2_1430Width", 0.0985);

  RooRealVar Kstar1680A ("Kstar1680A","Kstar1680Amplitude", 1.154);
  RooRealVar Kstar1680P ("Kstar1680P","Kstar1680Phase",     166.0*deg);
  RooRealVar Kstar1680M ("Kstar1680M","Kstar1680Mass", 1.677);
  RooRealVar Kstar1680W ("Kstar1680W","Kstar1680Width", 0.205);  //changed
  */  

}

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix=""
			  );
#endif
//
