#ifndef DKKPiPiResonances_HH
#define DKKPiPiResonances_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "DcyAmplitude.h"
#include "DbleAmplitude.h"

  /* "random" angles are randomly generated with root's random number generator:

running rootlogon.C

RooFit v2.07 -- Developed by Wouter Verkerke and David Kirkby 
                Copyright (C) 2000-2005 NIKHEF, University of California & Stanford University
                All rights reserved, please read http://roofit.sourceforge.net/license.txt

running defaultstyle.C
done defaultstyle.C
done rootlogon.C
root [0] TRandom rnd
root [1] rnd->Rndm()*360
(double)3.88494157791138264e+01
root [2] rnd->Rndm()*360
(double)2.11491022109985664e+02
root [3] rnd->Rndm()*360
(double)1.14830260276794618e+02
root [4] 

i.e. we get out: 39, 211, 115


.... and further: 53, 108, 15
as well as 55, 344, 173
209, 339, 87
302, 111, 169
175, 351, 92
262, 165, 69
  */

#define RANDOM_ANGLES
//#define RANDOM_ANGLES_II
//#define RANDOM_ANGLES_III
//#define RANDOM_ANGLES_IV
//#define RANDOM_ANGLES_V
//#define RANDOM_ANGLES_VI
//#define RANDOM_ANGLES_VII

namespace DKKpipiResonances{
  Double_t PrimDzMass = 1.8645;
  Double_t deg = TMath::Pi()/180.0;

  Double_t f_one = 0.00235698/18.0;

  Double_t CPph2       = 0*deg;

#ifdef RANDOM_ANGLES
  Double_t CPK1ph2     = 39*deg + CPph2;
  Double_t CPK1400ph2  = 211*deg + CPph2;
  Double_t CPKstph2    = 115*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_II
  Double_t CPK1ph2     = 53*deg + CPph2;
  Double_t CPK1400ph2  = 108*deg + CPph2;
  Double_t CPKstph2    = 15*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_III
  Double_t CPK1ph2     = 55*deg + CPph2;
  Double_t CPK1400ph2  = 344*deg + CPph2;
  Double_t CPKstph2    = 173*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_IV
  Double_t CPK1ph2     = 209*deg + CPph2;
  Double_t CPK1400ph2  = 339*deg + CPph2;
  Double_t CPKstph2    = 87*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_V
  Double_t CPK1ph2     = 302*deg + CPph2;
  Double_t CPK1400ph2  = 111*deg + CPph2;
  Double_t CPKstph2    = 169*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_VI
  Double_t CPK1ph2     = 175*deg + CPph2;
  Double_t CPK1400ph2  = 351*deg + CPph2;
  Double_t CPKstph2    = 92*deg + CPph2;
#else
#ifdef RANDOM_ANGLES_VII
  Double_t CPK1ph2     = 262*deg + CPph2;
  Double_t CPK1400ph2  = 165*deg + CPph2;
  Double_t CPKstph2    = 69*deg + CPph2;
#else
  Double_t CPK1ph2     = 0*deg + CPph2;
  Double_t CPK1400ph2  = 0*deg + CPph2;
  Double_t CPKstph2    = 0*deg + CPph2;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

  Double_t CPratio     = 1;

  Double_t CPf1_rate = 1./(1.0 + CPratio);
  Double_t CPf2_rate = CPratio/(1.0 + CPratio);

  Double_t sqrt_CPf1_rate = sqrt(CPf1_rate);
  Double_t sqrt_CPf2_rate = sqrt(CPf2_rate);

  Double_t fracNormFactorK1 =
    sqrt(1./(1.0 + 2.0*sqrt_CPf1_rate * sqrt_CPf2_rate * cos(CPK1ph2)));
  Double_t fracNormFactorK1400 =
    sqrt(1./(1.0 + 2.0*sqrt_CPf1_rate * sqrt_CPf2_rate * cos(CPK1400ph2)));
  Double_t fracNormFactorKst =
    sqrt(1./(1.0 + 2.0*sqrt_CPf1_rate * sqrt_CPf2_rate * cos(CPKstph2)));

  Double_t CPK1f1    = sqrt_CPf1_rate * fracNormFactorK1;    // factors to multiply the amplitudes
  Double_t CPK1f2    = sqrt_CPf2_rate * fracNormFactorK1;    // with, defined such that |A + A_cp|^2
  Double_t CPK1400f1 = sqrt_CPf1_rate * fracNormFactorK1400; // remains constant, i.e. independent
  Double_t CPK1400f2 = sqrt_CPf2_rate * fracNormFactorK1400; //  of CPratio and CPph2. 
  Double_t CPKstf1   = sqrt_CPf1_rate * fracNormFactorKst;   //  (For non-CP symmetric amplitudes.)
  Double_t CPKstf2   = sqrt_CPf2_rate * fracNormFactorKst;

  


  RooRealVar DZero_M   ("DZero_M" , "DZero_Mass", PrimDzMass);
  RooRealVar piPlus_M   ("piPlus_M" , "pi_Mass", 0.13957018);
  RooRealVar piMinus_M  ("piMinus_M", "pi_Mass", 0.13957018);
  RooRealVar Kplus_M  ("Kplus_M", "Kplus_Mass", 0.493677);
  RooRealVar Kminus_M  ("Kminus_M", "Kminus_Mass", 0.493677);

  RooRealVar Radius_D ("Radius_D", "Radius_D", 5.);
  RooRealVar Radius_r ("Radius_r", "Radius_r", 1.5);

  // K1(1270) K- decay amplitudes:
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // K1(1270) K-, K1->rho(770) K+
  
  Double_t f1=18./0.00235698 * f_one * 18.0/16.0;
  RooRealVar K1_1270_rhoK_A ("K1_1270_rhoK_A","K1_1270_rhoK_Amplitude", 1*sqrt(f1)*CPK1f1);// obacht
  RooRealVar K1_1270_rhoK_P ("K1_1270_rhoK_P","K1_1270_rhoK_Phase", 0*deg);
  RooRealVar K1_1270_rhoKCP2_A ("K1_1270_rhoKCP2_A","K1_1270_rhoKCP2_Amplitude", 1*sqrt(f1)*CPK1f2);// obacht
  RooRealVar K1_1270_rhoKCP2_P ("K1_1270_rhoKCP2_P","K1_1270_rhoKCP2_Phase", 0*deg + CPK1ph2);
  // K1(1270) K-, K1->K*(1430)pi+

  Double_t f2=2./5.49543e-05 * f_one * 2./2.6 * 2./1.6;
  RooRealVar K1_1270_Kst1430pi_A("K1_1270_Kst1430pi_A"
				 ,"K1_1270_Kst1430pi_Amplitude", 0.27*sqrt(f2)*CPK1f1);
  RooRealVar K1_1270_Kst1430pi_P ("K1_1270_Kst1430pi_P"
				  ,"K1_1270_Kst1430pi_Phase", 354*deg);
  RooRealVar K1_1270_Kst1430piCP2_A("K1_1270_Kst1430piCP2_A"
				 ,"K1_1270_Kst1430piCP2_Amplitude", 0.27*sqrt(f2)*CPK1f2);
  RooRealVar K1_1270_Kst1430piCP2_P ("K1_1270_Kst1430piCP2_P"
				  ,"K1_1270_Kst1430piCP2_Phase", 354*deg + CPK1ph2);
  // K1(1270) K-, K1->K*(892)pi+
  Double_t f3=16/0.103368 * f_one * 16./22. * 16./10.;
  RooRealVar K1_1270_Kst892pi_A("K1_1270_Kst892pi_A"
				,"K1_1270_Kst892pi_Amplitude", 0.94*sqrt(f3)*CPK1f1);
  RooRealVar K1_1270_Kst892pi_P ("K1_1270_Kst892pi_P"
				 ,"K1_1270_Kst892pi_Phase", 12*deg);
  RooRealVar K1_1270_Kst892piCP2_A("K1_1270_Kst892piCP2_A"
				,"K1_1270_Kst892piCP2_Amplitude", 0.94*sqrt(f3)*CPK1f2);
  RooRealVar K1_1270_Kst892piCP2_P ("K1_1270_Kst892piCP2_P"
				 ,"K1_1270_Kst892piCP2_Phase", 12*deg + CPK1ph2);
  

  // K1(1270) K- masses and widths
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  RooRealVar K1_1270_M ("K1_1270_M","K1_1270_Mass" , 1.273);
  RooRealVar K1_1270_W ("K1_1270_W","K1_1270_Width", 0.09);
  RooRealVar rho_M ("rho_M","rho_Mass", 0.7758);
  RooRealVar rho_W ("rho_W","rho_Width", 0.1503*1.0);// obacht
  RooRealVar omega_M ("omega_M","rho_Mass", 0.78259);
  RooRealVar omega_W ("omega_W","rho_Width", 8.49e-3 * 1.0); // obacht;
  RooRealVar Kstar0_1430_M ("Kstar0_1430_M","Kstar0_1430_Mass", 1.412);
  RooRealVar Kstar0_1430_W ("Kstar0_1430_W","Kstar0_1430_Width", 0.294);
  RooRealVar Kstar892_M ("Kstar892_M","Kstar892_Mass", 0.89166);
  RooRealVar Kstar892_W ("Kstar892_W","Kstar892W_idth", 0.0508);

  // K1(1400) K-:
  Double_t f4=22./ 0.0239218 * f_one * 22./30.2 * 22./16;
  RooRealVar K1_1400_A ("K1_1400_A","K1_1400_Amplitude", 1.18*sqrt(f4)*CPK1400f1);
  RooRealVar K1_1400_P ("K1_1400_P","K1_1400_Phase", 259*deg);
  RooRealVar K1_1400CP2_A ("K1_1400CP2_A","K1_1400CP2_Amplitude", 1.18*sqrt(f4)*CPK1400f2);
  RooRealVar K1_1400CP2_P ("K1_1400CP2_P","K1_1400CP2_Phase", 259*deg + CPK1400ph2);

  RooRealVar K1_1400_M ("K1_1400_M","K1_1400_Mass" , 1.402);
  RooRealVar K1_1400_W ("K1_1400_W","K1_1400_Width", 0.174);

  // K*892 K*~(892)
  Double_t f5=3./0.108142 * f_one * 3.0/3.5;
  RooRealVar Kstar892Kstar892_A ("Kstar892Kstar892_A"
				 ,"Kstar892Kstar892_Amplitude", 0.39*sqrt(f5));
  RooRealVar Kstar892Kstar892_P ("Kstar892Kstar892_P"
				 ,"Kstar892Kstar892_Phase", 28*deg);
  //(got mass/width of K*892 already)

  // phi rho
  Double_t f6= 29./0.811271 * f_one * 29./38. * 29.0/42.0 * 29./17.;
  RooRealVar phi_rho_A ("phi_rho_A","phi_rho_Amplitude", 1.30*sqrt(f6));//obacht!!!
  RooRealVar phi_rho_P ("phi_rho_P","phi_rho_Phase", (49 + 0)*deg); // obacht
  RooRealVar phi_M ("phi_M","phi_Mass" , 1.020);
  RooRealVar phi_W ("phi_W","phi_Width", 0.00426);
  // (got rho mass already)

  // rho K+ K- (non-res)
  Double_t f7= 2./0.000123957 * f_one * 2.0/0.1;
  RooRealVar rho_omega_A ("rho_A","rho_Amplitude", 0.33*sqrt(f7)); // obacht
  RooRealVar rho_omega_P ("rho_P","rho_Phase", 278*deg);

  // rho-omega interference
  RooRealVar rho_by_rho_A ("rho_by_rho_A"
			     ,"|A_rho/A_rho|", 1.0/(1. + 3.9));
  RooRealVar rho_by_rho_P ("rho_by_rho_P","arg(A_rho/A_rho)", 0);

  RooRealVar omega_by_rho_A ("omega_by_rho_A"
			     ,"|A_omega/A_rho|", sqrt(3.9)/(1. + 3.9));
  RooRealVar omega_by_rho_P ("omega_by_rho_P","arg(A_omega/A_rho)", 0);

  // phi pi+ pi- (non-res)
  Double_t f8= 1./0.00130702*f_one * 1.0/1.3 ;
  RooRealVar phi_A ("phi_A","phi_Amplitude", 0.30*sqrt(f8));
  RooRealVar phi_P ("phi_P","phi_Phase", 163*deg);

  // K*(892) K+ pi-
  Double_t f9= 11./0.00336539 * f_one * 11./13.5;
  RooRealVar Kstar892_A ("Kstar892_A","Kstar892_Amplitude", 0.83*sqrt(f9)*CPKstf1);
  RooRealVar Kstar892_P ("Kstar892_P","Kstar892_Phase",     234*deg);
  RooRealVar Kstar892CP2_A ("Kstar892CP2_A","Kstar892CP2_Amplitude", 0.83*sqrt(f9)*CPKstf2);
  RooRealVar Kstar892CP2_P ("Kstar892CP2_P","Kstar892CP2_Phase",     234*deg + CPKstph2);

  // f0_980 pi+ pi-
  Double_t f10= 15 /0.000331692 * f_one * 15.0/18.3;
  RooRealVar f0_980_A ("f0_980_A","f0_980_Amplitude", 0.91*sqrt(f10));
  RooRealVar f0_980_P ("f0_980_P","f0_980_Phase",     240*deg);
  RooRealVar f0_980_M ("f0_980_M","f0_980_Mass", 0.980);
  RooRealVar f0_980_W ("f0_980_W","f0_980_Width", 0.07); // badly known. PDG: 40-100 MeV
  RooArgList& makeDKKpipiResonance(RooAbsReal& m12sq
				   , RooAbsReal& m13sq
				   , RooAbsReal& m14sq
				   , RooAbsReal& m23sq
				   , RooAbsReal& m24sq
				   , RooAbsReal& m34sq
				   , RooAbsReal& m123sq
				   , RooAbsReal& m124sq
				   , RooAbsReal& m134sq
				   , RooAbsReal& m234sq
				   , RooAbsReal& m0
				   , RooAbsReal& m1
				   , RooAbsReal& m2
				   , RooAbsReal& m3
				   , RooAbsReal& m4
				   , std::string Prefix=""
				   );
  
};

#endif
//
