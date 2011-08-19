// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "DKsPiPiResonances.h"
#include "Mint/SpinFactors.h"
using namespace DKsPiPiResonances;

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix
			  ){

  //Calculate spinfactors 
  
  SF_DtoVP* SF_Kstarz_892 
    = new SF_DtoVP((Prefix+"SF_Kstarz_892").c_str()
                   ,(Prefix+"SF_Kstarz_892").c_str()
                   ,Kstar892M
                   ,m13sq,m12sq,m23sq
                   ,KPlusM,piMinusM,KsM,DZeroM);

  SF_DtoTP* SF_Kstarz2_1430 
    = new SF_DtoTP((Prefix+"SF_Kstarz2_1430").c_str()
                   ,(Prefix+"SF_Kstarz2_1430").c_str()
                   ,Kstar2_1430M
                   ,m13sq,m12sq,m23sq
                   ,KPlusM,piMinusM,KsM,DZeroM);

  SF_DtoVP* SF_Kstarz_1680 
    = new SF_DtoVP((Prefix+"SF_Kstarz_1680").c_str()
                   ,(Prefix+"SF_Kstarz_1680").c_str()
                   ,Kstar1680M
                   ,m13sq,m12sq,m23sq
                   ,KPlusM,piMinusM,KsM,DZeroM);

  SF_DtoVP* SF_Kstarm_892 
    = new SF_DtoVP((Prefix+"SF_Kstarm_892").c_str()
                   ,(Prefix+"SF_Kstarm_892").c_str()
                   ,Kstar892M
                   ,m23sq,m12sq,m13sq
                   ,KsM,piMinusM,KPlusM,DZeroM);

  SF_DtoTP* SF_Kstarm2_1430 
    = new SF_DtoTP((Prefix+"SF_Kstarm2_1430").c_str()
                   ,(Prefix+"SF_Kstarm2_1430").c_str()
                   ,Kstar2_1430M
                   ,m23sq,m12sq,m23sq
                   ,KsM,piMinusM,KPlusM,DZeroM);

  SF_DtoVP* SF_Kstarm_1680 
    = new SF_DtoVP((Prefix+"SF_Kstarm_1680").c_str()
                   ,(Prefix+"SF_Kstarm_1680").c_str()
                   ,Kstar1680M
                   ,m23sq,m12sq,m23sq
                   ,KsM,piMinusM,KPlusM,DZeroM);

  // cabbibo allowed D decays
  DcyAmplitude Kstarz892((Prefix+"Kstarz892").c_str()
                        , (Prefix+"Kstarz892").c_str()
                        , *SF_Kstarz_892 //DummySpinFactor
                        , m13sq
                        , Kstarz892A, Kstarz892P
                        , Kstar892M, Kstar892W
                        , KPlusM // measured mass of 1st daughter
                        , KPlusM // nominal mass of 1st daughter
                        , PiMinusM      // measured mass of 2nd daughter
                        , PiMinusM      // nominal mass of 2nd daughter
                        , Radius_r
                        , 1        // spin of K*
                        );

  DcyAmplitude Kstarz0_1430((Prefix+"Kstarz0_1430").c_str()
			   , (Prefix+"Kstarz0_1430").c_str()
			   , ScalarSpinFactor
			   , m13sq
			   , Kstarz0_1430A, Kstarz0_1430P
			   , Kstar0_1430M, Kstar0_1430W
			   , KPlusM // measured mass of 1st daughter
			   , KPlusM // nominal mass of 1st daughter
			   , PiMinusM      // measured mass of 2nd daughter
			   , PiMinusM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 0        // spin of K*
			   );

  DcyAmplitude Kstarz2_1430((Prefix+"Kstarz2_1430").c_str()
			   , (Prefix+"Kstarz2_1430").c_str()
			   , *SF_Kstarz2_1430
			   , m13sq
			   , Kstarz2_1430A, Kstarz2_1430P
			   , Kstar2_1430M, Kstar2_1430W
			   , KPlusM // measured mass of 1st daughter
			   , KPlusM // nominal mass of 1st daughter
			   , PiMinusM      // measured mass of 2nd daughter
			   , PiMinusM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 2        // spin of K*
			   );

  DcyAmplitude Kstarz1680((Prefix+"Kstarz1680").c_str()
			 , (Prefix+"Kstarz1680").c_str()
			 , *SF_Kstarz_1680CF
			 , m13sq
			 , Kstarz1680A, Kstarz1680P
			 , Kstar1680M, Kstar1680W
			 , KPlusM // measured mass of 1st daughter
			 , KPlusM // nominal mass of 1st daughter
			 , PiMinusM      // measured mass of 2nd daughter
			 , PiMinusM      // nominal mass of 2nd daughter
			 , Radius_r
			 , 1        // spin of K*
			 );

  // cabbibo allowed D decays
  DcyAmplitude Kstarm892((Prefix+"Kstarm892").c_str()
                        , (Prefix+"Kstarm892").c_str()
                        , *SF_Kstarm_892 //DummySpinFactor
                        , m23sq
                        , Kstarm892A, Kstarm892P
                        , Kstar892M, Kstar892W
                        , KsM // measured mass of 1st daughter
                        , KsM // nominal mass of 1st daughter
                        , PiMinusM      // measured mass of 2nd daughter
                        , PiMinusM      // nominal mass of 2nd daughter
                        , Radius_r
                        , 1        // spin of K*
                        );

  DcyAmplitude Kstarm0_1430((Prefix+"Kstarm0_1430").c_str()
			   , (Prefix+"Kstarm0_1430").c_str()
			   , ScalarSpinFactor
			   , m23sq
			   , Kstarm0_1430A, Kstarm0_1430P
			   , Kstar0_1430M, Kstar0_1430W
			   , KsM // measured mass of 1st daughter
			   , KsM // nominal mass of 1st daughter
			   , PiMinusM      // measured mass of 2nd daughter
			   , PiMinusM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 0        // spin of K*
			   );

  DcyAmplitude Kstarm2_1430((Prefix+"Kstarm2_1430").c_str()
			   , (Prefix+"Kstarm2_1430").c_str()
			   , *SF_Kstarm2_1430
			   , m23sq
			   , Kstarm2_1430A, Kstarm2_1430P
			   , Kstar2_1430M, Kstar2_1430W
			   , KsM // measured mass of 1st daughter
			   , KsM // nominal mass of 1st daughter
			   , PiMinusM      // measured mass of 2nd daughter
			   , PiMinusM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 2        // spin of K*
			   );

  DcyAmplitude Kstarm1680((Prefix+"Kstarm1680").c_str()
			 , (Prefix+"Kstarm1680").c_str()
			 , *SF_Kstarm_1680CF
			 , m23sq
			 , Kstarm1680A, Kstarm1680P
			 , Kstar1680M, Kstar1680W
			 , KsM // measured mass of 1st daughter
			 , KsM // nominal mass of 1st daughter
			 , PiMinusM      // measured mass of 2nd daughter
			 , PiMinusM      // nominal mass of 2nd daughter
			 , Radius_r
			 , 1        // spin of K*
			 );


  DcyNonRes NonRes((Prefix+"NonRes").c_str()
                   ,(Prefix+"NonRes").c_str()
                   ,NonResA
                   ,NonResP
                   );
  

  RooArgList* ResonanceListPtr = new RooArgList();
  RooArgList& ResonanceList = *ResonanceListPtr;

  ResonanceList.add( *((DcyAmplitude*) Kstar892.clone()));
  ResonanceList.add( *((DcyAmplitude*) Kstar1410.clone()));
  ResonanceList.add( *((DcyAmplitude*) Kstar0_1430.clone()));
  ResonanceList.add( *((DcyAmplitude*) Kstar2_1430.clone()));
  ResonanceList.add( *((DcyAmplitude*) Kstar1680.clone()));
  ResonanceList.add( *((DcyAmplitude*) CSKstar892.clone()));
  ResonanceList.add( *((DcyAmplitude*) CSKstar0_1430.clone()));
  ResonanceList.add( *((DcyAmplitude*) CSKstar2_1430.clone()));
  ResonanceList.add( *((DcyGSAmplitude*) rho.clone()));
  ResonanceList.add( *((DcyAmplitude*) omega.clone()));
  ResonanceList.add( *((DcyAmplitude*) sigma1.clone()));
  ResonanceList.add( *((DcyAmplitude*) sigma2.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_980.clone()));
  ResonanceList.add( *((DcyAmplitude*) f2_1270.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_1370.clone()));
  ResonanceList.add( *((DcyGSAmplitude*) rho1450.clone()));
  ResonanceList.add( *((DcyNonRes*) NonRes.clone()));
  


  return ResonanceList;
}
//
