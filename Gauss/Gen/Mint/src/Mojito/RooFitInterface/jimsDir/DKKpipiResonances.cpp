// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#include "DKKpipiResonances.h"

#include "rhoOmega.h"
#include "DbleAmplitudeWithRho.h"
#include "Mint/SpinFactors.h"
#include <iostream>
#include "RooFormulaVar.h"

using namespace DKKpipiResonances;
using namespace std;
// numbering:

// D -> K+ K- pi+ pi-
// 0    1  2  3   4
//
RooArgList& DKKpipiResonances::makeDKKpipiResonance (RooAbsReal& m12sq
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
						     , std::string Prefix
						     ){


  RooFormulaVar* m12Ptr = new RooFormulaVar( (Prefix + "m12").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m12sq));
  RooFormulaVar& m12 = *m12Ptr;
  RooFormulaVar* m13Ptr = new RooFormulaVar( (Prefix + "m13").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m13sq));
  RooFormulaVar& m13 = *m13Ptr;
  RooFormulaVar* m14Ptr = new RooFormulaVar( (Prefix + "m14").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m14sq));
  RooFormulaVar& m14 = *m14Ptr;
  RooFormulaVar* m23Ptr = new RooFormulaVar( (Prefix + "m23").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m23sq));
  RooFormulaVar& m23 = *m23Ptr;
  RooFormulaVar* m24Ptr = new RooFormulaVar( (Prefix + "m24").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m24sq));
  RooFormulaVar& m24 = *m24Ptr;
  RooFormulaVar* m34Ptr = new RooFormulaVar( (Prefix + "m34").c_str()
					     , "sqrt(@0)"
					     , RooArgList(m34sq));
  RooFormulaVar& m34 = *m34Ptr;

  cout << "declaring 1st amplitude " << endl;  

  // (D->K1(1270) K-)L=1, (K1-> rho(770)K+)L=0, (rho->pipi)L=1
  SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1270_rhoK 
    = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1270_rhoK").c_str()
				   , (Prefix+"SF_K1_1270_rhoK").c_str()
				   , K1_1270_M
				   , rho_M
				   , 2 // particle 2 (K-) has role of P1
				   , 1
				   , 4
				   , 3
				   , m234sq, m12sq, m23sq, m34sq, m123sq
				   , m0, m1, m2, m3, m4
				   );
  
  
  DbleAmplitudeWithRho K1_1270_rhoK((Prefix+"K1_1270_rhoK").c_str()
				    , (Prefix+"K1_1270_rhoK").c_str()
				    , *SF_K1_1270_rhoK
				    , m134sq
				    , m34sq
				    , K1_1270_rhoK_A
				    , K1_1270_rhoK_P
				    , K1_1270_M
				    , K1_1270_W
				    , rho_M   // nominal mass of 1st dgt of K1
				    , m34     // mass of 1st dgt of K1
				    , Kplus_M // nominal mass of 2nd dgt of K1
				    , Kplus_M // mass of 2nd dgt of K1
				    , Radius_r     // "radius" of K1 resonance"
				    , 0
				    , rho_M
				    , rho_W
				    , omega_M
				    , omega_W
				    , omega_by_rho_A
				    , omega_by_rho_P
				    , piPlus_M  // nominal mass of 1st dgt of rho
				    , piPlus_M  // mass of 1st dgt of rho
				    , piMinus_M // nominal mass of 2nd dgt of rho
				    , piMinus_M // mass of 2nd dgt of rho
				    , Radius_r
				    , 1
				    );
  
  SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1270_rhoKCP2
    = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1270_rhoKCP2").c_str()
				   , (Prefix+"SF_K1_1270_rhoKCP2").c_str()
				   , K1_1270_M
				   , rho_M
				   , 2 // particle 2 (K-) has role of P1
				   , 1
				   , 4
				   , 3
				   , m134sq, m12sq, m14sq, m34sq, m124sq
				   , m0, m1, m2, m3, m4
				   );
  
  
  DbleAmplitudeWithRho K1_1270_rhoKCP2((Prefix+"K1_1270_rhoKCP2").c_str()
				    , (Prefix+"K1_1270_rhoKCP2").c_str()
				    , *SF_K1_1270_rhoKCP2
				    , m234sq
				    , m34sq
				    , K1_1270_rhoKCP2_A
				    , K1_1270_rhoKCP2_P
				    , K1_1270_M
				    , K1_1270_W
				    , rho_M   // nominal mass of 1st dgt of K1
				    , m34     // mass of 1st dgt of K1
				    , Kplus_M // nominal mass of 2nd dgt of K1
				    , Kplus_M // mass of 2nd dgt of K1
				    , Radius_r     // "radius" of K1 resonance"
				    , 0
				    , rho_M
				    , rho_W
				    , omega_M
				    , omega_W
				    , omega_by_rho_A
				    , omega_by_rho_P
				    , piPlus_M  // nominal mass of 1st dgt of rho
				    , piPlus_M  // mass of 1st dgt of rho
				    , piMinus_M // nominal mass of 2nd dgt of rho
				    , piMinus_M // mass of 2nd dgt of rho
				    , Radius_r
				    , 1
				    );
  
  /*
  DbleAmplitude K1_1270_rhoK((Prefix+"K1_1270_rhoK").c_str()
			     , (Prefix+"K1_1270_rhoK").c_str()
			     , *SF_K1_1270_rhoK
			     , m134sq
			     , m34sq
			     , K1_1270_rhoK_A
			     , K1_1270_rhoK_P
			     , K1_1270_M
			     , K1_1270_W
			     , rho_M   // nominal mass of 1st dgt of K1
			     , m34     // mass of 1st dgt of K1
			     , Kplus_M // nominal mass of 2nd dgt of K1
			     , Kplus_M // mass of 2nd dgt of K1
			     , Radius_r     // "radius" of K1 resonance"
			     , 0
			     , rho_M
			     , rho_W
			     , piPlus_M  // nominal mass of 1st dgt of rho
			     , piPlus_M  // mass of 1st dgt of rho
			     , piMinus_M // nominal mass of 2nd dgt of rho
			     , piMinus_M // mass of 2nd dgt of rho
			     , Radius_r
			     , 1
			     );

  */
  cout << " ...2nd" << endl;

  // (D-> K1(1270)+ K-)L=1, (K1->K0*(1430) pi+)L=1, (K0*(1430)->K+pi-)L=0
  SF_DtoAP1_AtoSP2_StoP3P4* SF_K1_1270_Kst1430pi 
    = new SF_DtoAP1_AtoSP2_StoP3P4((Prefix+"SF_K1_1270_Kst1430pi").c_str()
				   , (Prefix+"SF_K1_1270_Kst1430pi").c_str()
				   , K1_1270_M
				   , 2, 3, 1, 4
				   , m234sq, m12sq, m23sq, m34sq, m123sq
				   , m0, m1, m2, m3, m4
				   );
  
  DbleAmplitude K1_1270_Kst1430pi((Prefix+"K1_1270_Kst1430pi").c_str()
				  , (Prefix+"K1_1270_Kst1430pi").c_str()
				  , *SF_K1_1270_Kst1430pi
				  , m134sq
				  , m14sq
				  , K1_1270_Kst1430pi_A
				  , K1_1270_Kst1430pi_P
				  , K1_1270_M
				  , K1_1270_W
				  , Kstar0_1430_M // nominal K1 dgt 1
				  //, m14 // obacht-nominal K1(above) problematic
				  , m14 // K1 dgt 1
				  , piPlus_M      // nominal K1 dgt 2
				  , piPlus_M      // K1 dgt 2
				  , Radius_r      // K1 "Radius"
				  , 1
				  , Kstar0_1430_M
				  , Kstar0_1430_W
				  , Kplus_M
				  , Kplus_M
				  , piMinus_M
				  , piMinus_M
				  , Radius_r
				  , 0
				  );

  SF_DtoAP1_AtoSP2_StoP3P4* SF_K1_1270_Kst1430piCP2
    = new SF_DtoAP1_AtoSP2_StoP3P4((Prefix+"SF_K1_1270_Kst1430piCP2").c_str()
				   , (Prefix+"SF_K1_1270_Kst1430piCP2").c_str()
				   , K1_1270_M
				   , 2, 3, 1, 4
				   , m134sq, m12sq, m14sq, m34sq, m124sq
				   , m0, m1, m2, m3, m4
				   );
  
  DbleAmplitude K1_1270_Kst1430piCP2((Prefix+"K1_1270_Kst1430piCP2").c_str()
				  , (Prefix+"K1_1270_Kst1430piCP2").c_str()
				  , *SF_K1_1270_Kst1430piCP2
				  , m234sq
				  , m23sq
				  , K1_1270_Kst1430piCP2_A
				  , K1_1270_Kst1430piCP2_P
				  , K1_1270_M
				  , K1_1270_W
				  , Kstar0_1430_M // nominal K1 dgt 1
				  //, m14 // obacht-nominal K1(above) problematic
				  , m23 // K1 dgt 1
				  , piPlus_M      // nominal K1 dgt 2
				  , piPlus_M      // K1 dgt 2
				  , Radius_r      // K1 "Radius"
				  , 1
				  , Kstar0_1430_M
				  , Kstar0_1430_W
				  , Kplus_M
				  , Kplus_M
				  , piMinus_M
				  , piMinus_M
				  , Radius_r
				  , 0
				  );


  // (D->K1(1270)+ K-)L=1, (K1->K*(892)^0 pi+)L=0, (K*->K+ pi-)L=1
   SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1270_Kst892pi
     = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1270_Kst892pi").c_str()
				    , (Prefix+"SF_K1_1270_Kst892pi").c_str()
				    , K1_1270_M
				    , Kstar892_M
				    , 2 // particle 2 (K-) has role of P1
				    , 3
				    , 4
				    , 1
				    , m234sq, m12sq, m23sq, m34sq, m123sq
				    , m0, m1, m2, m3, m4
				    );

   DbleAmplitude K1_1270_Kst892pi((Prefix+"K1_1270_Kst892pi").c_str()
				  , (Prefix+"K1_1270_Kst892pi").c_str()
				  , *SF_K1_1270_Kst892pi
				  , m134sq
				  , m14sq
				  , K1_1270_Kst892pi_A
				  , K1_1270_Kst892pi_P
				  , K1_1270_M
				  , K1_1270_W
				  , Kstar892_M
				  , m14
				  , piPlus_M
				  , piPlus_M
				  , Radius_r
				  , 0
				  , Kstar892_M
				  , Kstar892_W
				  , Kplus_M
				  , Kplus_M
				  , piMinus_M
				  , piMinus_M
				  , Radius_r
				  , 1
				  );

  // (D->K1(1270)+ K-)L=1, (K1->K*(892)^0 pi+)L=0, (K*->K+ pi-)L=1
   SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1270_Kst892piCP2
     = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1270_Kst892piCP2").c_str()
				    , (Prefix+"SF_K1_1270_Kst892piCP2").c_str()
				    , K1_1270_M
				    , Kstar892_M
				    , 2 // particle 2 (K-) has role of P1
				    , 3
				    , 4
				    , 1
				    , m134sq, m12sq, m14sq, m34sq, m124sq
				    , m0, m1, m2, m3, m4
				    );

   DbleAmplitude K1_1270_Kst892piCP2((Prefix+"K1_1270_Kst892piCP2").c_str()
				  , (Prefix+"K1_1270_Kst892piCP2").c_str()
				  , *SF_K1_1270_Kst892piCP2
				  , m234sq
				  , m23sq
				  , K1_1270_Kst892piCP2_A
				  , K1_1270_Kst892piCP2_P
				  , K1_1270_M
				  , K1_1270_W
				  , Kstar892_M
				  , m23
				  , piPlus_M
				  , piPlus_M
				  , Radius_r
				  , 0
				  , Kstar892_M
				  , Kstar892_W
				  , Kplus_M
				  , Kplus_M
				  , piMinus_M
				  , piMinus_M
				  , Radius_r
				  , 1
				  );


   // (D-> K1(1400)+ K-)L=1, (K1->K*(892) pi+)L=0, (K*->K+pi-)L=1
   SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1400
     = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1400").c_str()
				    , (Prefix+"SF_K1_1400").c_str()
				    , K1_1400_M
				    , Kstar892_M
				    , 2 // particle 2 (K-) has role of P1
				    , 3 // pi+ is P2
				    , 1
				    , 4
				    , m234sq, m12sq, m23sq, m34sq, m123sq
				    , m0, m1, m2, m3, m4
				    );


   DbleAmplitude K1_1400((Prefix+"K1_1400").c_str()
			 , (Prefix+"K1_1400").c_str()
			 , *SF_K1_1400
			 , m134sq
			 , m14sq
			 , K1_1400_A, K1_1400_P
			 , K1_1400_M, K1_1400_W
			 , Kstar892_M
			 , m14
			 , piPlus_M
			 , piPlus_M
			 , Radius_r
			 , 0
			 , Kstar892_M
			 , Kstar892_W
			 , Kplus_M
			 , Kplus_M
			 , piMinus_M
			 , piMinus_M
			 , Radius_r
			 , 1
			 );

   // (D-> K1(1400)+ K-)L=1, (K1->K*(892) pi+)L=0, (K*->K+pi-)L=1
   SF_DtoAP1_AtoVP2_VtoP3P4* SF_K1_1400CP2
     = new SF_DtoAP1_AtoVP2_VtoP3P4((Prefix+"SF_K1_1400CP2").c_str()
				    , (Prefix+"SF_K1_1400CP2").c_str()
				    , K1_1400_M
				    , Kstar892_M
				    , 2 // particle 2 (K-) has role of P1
				    , 3 // pi+ is P2
				    , 1
				    , 4
				    , m234sq, m12sq, m23sq, m34sq, m123sq
				    , m0, m1, m2, m3, m4
				    );


   DbleAmplitude K1_1400CP2((Prefix+"K1_1400CP2").c_str()
			 , (Prefix+"K1_1400CP2").c_str()
			 , *SF_K1_1400CP2
			 , m234sq
			 , m23sq
			 , K1_1400CP2_A, K1_1400CP2_P
			 , K1_1400_M, K1_1400_W
			 , Kstar892_M
			 , m23
			 , piPlus_M
			 , piPlus_M
			 , Radius_r
			 , 0
			 , Kstar892_M
			 , Kstar892_W
			 , Kplus_M
			 , Kplus_M
			 , piMinus_M
			 , piMinus_M
			 , Radius_r
			 , 1
			 );


   // (D->K*(892)^0 K*(892)~^0)L=*0*,1,2; (K*->Kpi)L=1
   SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_Kstar892Kstar892_S
     = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_Kstar892Kstar892_S").c_str()
					  , (Prefix+"SF_Kstar892Kstar892_S").c_str()
					  , Kstar892_M// Kstar
					  , Kstar892_M// Kstar-bar
					  , 1
					  , 4
					  , 2
					  , 3
					  , m234sq, m12sq, m23sq, m34sq, m123sq
					  , m0, m1, m2, m3, m4
					  );
   
  DbleAmplitude Kstar892Kstar892((Prefix+"Kstar892Kstar892").c_str()
				 , (Prefix+"Kstar892Kstar892").c_str()
				 , *SF_Kstar892Kstar892_S
				 , m14sq
				 , m23sq
				 , Kstar892Kstar892_A
				 , Kstar892Kstar892_P
				 , Kstar892_M
				 , Kstar892_W
				 , Kplus_M
				 , Kplus_M
				 , piMinus_M
				 , piMinus_M
				 , Radius_r
				 , 1
				 , Kstar892_M
				 , Kstar892_W
				 , Kminus_M
				 , Kminus_M
				 , piPlus_M
				 , piPlus_M
				 , Radius_r
				 , 1
				 );

  // (D -> phi(1020) rho(770))L=*0*,1,2; (phi->KK)L=1, (rho->pipi)L=1
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_phi_rho_S 
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_phi_rho_S").c_str()
					 , (Prefix+"SF_phi_rho_S").c_str()
					 , phi_M
					 , rho_M
					 , 1
					 , 2
					 , 3
					 , 4
					 , m234sq, m12sq, m23sq, m34sq, m123sq
					 , m0, m1, m2, m3, m4
					 );

  
  DbleAmplitudeWithRho phi_rho((Prefix+"phi_rho").c_str()
			       , (Prefix+"phi_rho").c_str()
			       , *SF_phi_rho_S
			       , m12sq
			       , m34sq
			       , phi_rho_A
			       , phi_rho_P
			       , phi_M
			       , phi_W
			       , Kplus_M
			       , Kplus_M
			       , Kminus_M
			       , Kminus_M
			       , Radius_r
			       , 1
			       , rho_M
			       , rho_W
			       , omega_M
			       , omega_W
			       , omega_by_rho_A
			       , omega_by_rho_P
			       , piPlus_M
			       , piPlus_M
			       , piMinus_M
			       , piMinus_M
			       , Radius_r
			       , 1
			       );
  /*
  DbleAmplitude phi_rho((Prefix+"phi_rho").c_str()
			, (Prefix+"phi_rho").c_str()
			, *SF_phi_rho_S
			, m12sq
			, m34sq
			, phi_rho_A
			, phi_rho_P
			, phi_M
			, phi_W
			, Kplus_M
			, Kplus_M
			, Kminus_M
			, Kminus_M
			, Radius_r
			, 1
			, rho_M
			, rho_W
			, piPlus_M
			, piPlus_M
			, piMinus_M
			, piMinus_M
			, Radius_r
			, 1
			);
  */
  // (D-> rho(770) K+ K-)L=0, (rho->pipi)L=1
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_rho
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_rho").c_str()
				       , (Prefix+"SF_rho").c_str()
				       , rho_M
				       , m34
				       , 1
				       , 2
				       , 4
				       , 3
				       , m234sq, m12sq, m23sq, m34sq, m123sq
				       , m0, m1, m2, m3, m4
				       );
    
  /* old, standard BW rho:*/
  DcyAmplitude* rho_BW 
    = new DcyAmplitude((Prefix+"rho_BW").c_str()
		       , (Prefix+"rho_BW").c_str()
		       , *SF_rho
		       , m34sq
		       , rho_by_rho_A, rho_by_rho_P
		       , rho_M, rho_W
		       , piPlus_M
		       , piPlus_M
		       , piMinus_M
		       , piMinus_M
		       , Radius_r
		       , 1
		       );

  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_omega
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_omega").c_str()
				       , (Prefix+"SF_omega").c_str()
				       , omega_M
				       , m34
				       , 1
				       , 2
				       , 3
				       , 4
				       , m234sq, m12sq, m23sq, m34sq, m123sq
				       , m0, m1, m2, m3, m4
				       );

  DcyAmplitude* omega_BW 
    = new DcyAmplitude((Prefix+"omega_BW").c_str()
		       , (Prefix+"omega_BW").c_str()
		       , *SF_omega
		       , m34sq
		       , omega_by_rho_A
		       , omega_by_rho_P
		       , omega_M, omega_W
		       , piPlus_M
		       , piPlus_M
		       , piMinus_M
		       , piMinus_M
		       , Radius_r
		       , 1
		       );

  rhoOmega rho_omega((Prefix+"rho").c_str()
	       , (Prefix+"rho").c_str()
	       , rho_omega_A, rho_omega_P
	       , *rho_BW
	       , *omega_BW
	       );


  // D->phi(1020) pi+ pi-, phi->KK
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_phi
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_phi").c_str()
				       , (Prefix+"SF_phi").c_str()
				       , phi_M
				       , m34
				       , 1
				       , 2
				       , 3
				       , 4
				       , m234sq, m12sq, m23sq, m34sq, m123sq
				       , m0, m1, m2, m3, m4
				       );

  DcyAmplitude phi((Prefix+"phi").c_str()
		   , (Prefix+"phi").c_str()
		   , *SF_phi
		   , m12sq
		   , phi_A, phi_P
		   , phi_M, phi_W
		   , Kplus_M
		   , Kplus_M
		   , Kminus_M
		   , Kminus_M
		   , Radius_r
		   , 1
		   );

  // D->K*(892) K+ pi-, K*->K- pi+
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_Kstar892
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_Kstar892").c_str()
				   , (Prefix+"SF_Kstar892").c_str()
				       , Kstar892_M
				       , m14
				       , 2
				       , 3
				       , 1
				       , 4
				       , m234sq, m12sq, m23sq, m34sq, m123sq
				       , m0, m1, m2, m3, m4
				       );
  
  DcyAmplitude Kstar892((Prefix+"Kstar892").c_str()
			, (Prefix+"Kstar892").c_str()
			, *SF_Kstar892
			, m23sq
			, Kstar892_A, Kstar892_P
			, Kstar892_M, Kstar892_W
			, Kminus_M
			, Kminus_M
			, piPlus_M
			, piPlus_M
			, Radius_r
			, 1
			);
  
  SF_DtoV1V2_V1toP1P2_V2toP3P4_S* SF_Kstar892CP2
    = new SF_DtoV1V2_V1toP1P2_V2toP3P4_S((Prefix+"SF_Kstar892CP2").c_str()
				   , (Prefix+"SF_Kstar892CP2").c_str()
				       , Kstar892_M
				       , m23
				       , 2
				       , 3
				       , 1
				       , 4
				       , m134sq, m12sq, m14sq, m34sq, m124sq
				       , m0, m1, m2, m3, m4
				       );
  
  DcyAmplitude Kstar892CP2((Prefix+"Kstar892CP2").c_str()
			, (Prefix+"Kstar892CP2").c_str()
			, *SF_Kstar892CP2
			, m14sq
			, Kstar892CP2_A, Kstar892CP2_P
			, Kstar892_M, Kstar892_W
			, Kminus_M
			, Kminus_M
			, piPlus_M
			, piPlus_M
			, Radius_r
			, 1
			);
  

  // D->f_0(980) pi+ pi-, f_0->KK 
  // above according to FOCUS table. Assume this is wrong.
  // more likely: D->f_0(980) K+ K-, f_0 ->pi+pi-
  SF_DtoVS_VtoP1P2_StoP3P4* SF_f0_980
    = new SF_DtoVS_VtoP1P2_StoP3P4((Prefix+"SF_f0_980").c_str()
				   , (Prefix+"SF_f0_980").c_str()
				   , m12
				   , 3
				   , 4
				   , 1
				   , 2
				   , m234sq, m12sq, m23sq, m34sq, m123sq
				   , m0, m1, m2, m3, m4
				   );

 
  DcyAmplitude f0_980((Prefix+"f0_980").c_str()
		      , (Prefix+"f0_980").c_str()
		      , *SF_f0_980
		      , m34sq
		      , f0_980_A, f0_980_P
		      , f0_980_M, f0_980_W
		      , piPlus_M
		      , piPlus_M
		      , piMinus_M
		      , piMinus_M
		      , Radius_r
		      , 0
		      );

  cout << " and last! " << endl;
  RooArgList* ResonanceListPtr = new RooArgList();
  RooArgList& ResonanceList = *ResonanceListPtr;


  bool noDbl=false;
  cout << " declared Resonance List " << endl;

  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_rhoK.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_Kst1430pi.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_Kst892pi.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1400.clone()));

  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_rhoKCP2.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_Kst1430piCP2.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1270_Kst892piCP2.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) K1_1400CP2.clone()));

  ResonanceList.add( *((DcyAmplitude*) Kstar892.clone()));
  ResonanceList.add( *((DcyAmplitude*) Kstar892CP2.clone()));


  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) Kstar892Kstar892.clone()));
  if(! noDbl)ResonanceList.add( *((DbleAmplitude*) phi_rho.clone()));
  ResonanceList.add( *((DcyAmplitude*) rho_omega.clone()));
  ResonanceList.add( *((DcyAmplitude*) phi.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_980.clone()));

  cout << " filled the Resonance List!! " << endl;

  return ResonanceList;
}
//
