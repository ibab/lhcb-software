// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "DKsKKResonances.h"
#include "Mint/SpinFactors.h"
using namespace DKsKKResonances;

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix
			  ){

  //Calculate spinfactors 
  
  SF_DtoVP* SF_Phi 
    = new SF_DtoVP((Prefix+"SF_Phi").c_str()
                   ,(Prefix+"SF_Phi").c_str()
                   ,phiM
                   ,m13sq,m23sq,m12sq
                   ,KPlusM,KPlusM,KsM,DZeroM);


  // cabbibo allowed D decays
  DcyAmplitude phi((Prefix+"phi").c_str()
                        , (Prefix+"phi").c_str()
                        , *SF_Phi //DummySpinFactor
                        , m13sq
                        , phiA, phiP
                        , phiM, phiW
                        , KPlusM // measured mass of 1st daughter
                        , KPlusM // nominal mass of 1st daughter
                        , KPlusM      // measured mass of 2nd daughter
                        , KPlusM      // nominal mass of 2nd daughter
                        , Radius_r
                        , 1        // spin of K*
                        );

  DcyAmplitude f0_1400((Prefix+"f0_1400").c_str()
			   , (Prefix+"f0_1400").c_str()
			   , ScalarSpinFactor
			   , m13sq
			   , f0_1400A, f0_1400P
			   , f0_1400M, f0_1400W
			   , KPlusM // measured mass of 1st daughter
			   , KPlusM // nominal mass of 1st daughter
			   , KPlusM      // measured mass of 2nd daughter
			   , KPlusM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 0        // spin of K*
			   );

  DcyCoupledAmplitude a0z_980((Prefix+"a0z_980").c_str()
			   , (Prefix+"a0z_980").c_str()
         , m13sq
         , m23sq
         , m12sq
         , a0z_980A
         , a0z_980P
         , a0z_980M
         , a0z_980M                  
			   , a0z_980J
         , a0z_980GKK
         , a0z_980Getapi
			   , KPlusM // measured mass of 1st daughter
			   , KPlusM // nominal mass of 1st daughter
			   , KsM      // measured mass of 2nd daughter
         , DZeroM
			   , EtaM      // nominal mass of 2nd daughter
         , PiZeroM
			   );
  DcyCoupledAmplitude a0p_980((Prefix+"a0p_980").c_str()
			   , (Prefix+"a0p_980").c_str()
         , m23sq
         , m13sq
         , m12sq
         , a0p_980A
         , a0p_980P
         , a0z_980M
         , a0z_980M                  
			   , a0z_980J 
         , a0z_980GKK
         , a0z_980Getapi
         , KsM
  		   , KPlusM 
			   , KPlusM      // measured mass of 2nd daughter
         , DZeroM
			   , EtaM      // nominal mass of 2nd daughter
         , PiPlusM
			   );

  


  RooArgList* ResonanceListPtr = new RooArgList();
  RooArgList& ResonanceList = *ResonanceListPtr;

  ResonanceList.add( *((DcyCoupledAmplitude*) a0z_980.clone()));
  ResonanceList.add( *((DcyCoupledAmplitude*) a0p_980.clone()));
  ResonanceList.add( *((DcyAmplitude*) phi.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_1400.clone()));
  


  return ResonanceList;
}
//
