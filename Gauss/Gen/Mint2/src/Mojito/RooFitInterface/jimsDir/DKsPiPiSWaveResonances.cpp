// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "DKsPiPiSWaveResonances.h"
#include "Mint/SpinFactors.h"
using namespace DKsPiPiSWaveResonances;

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix
			  ){

  //Calculate spinfactors 
  
  SF_DtoVP* SF_Kstar_892CF 
    = new SF_DtoVP((Prefix+"SF_Kstar_892CF").c_str()
                   ,(Prefix+"SF_Kstar_892CF").c_str()
                   ,Kstar892M
                   ,m23sq,m12sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoVP* SF_Kstar_1410CF 
    = new SF_DtoVP((Prefix+"SF_Kstar_1410CF").c_str()
                   ,(Prefix+"SF_Kstar_1410CF").c_str()
                   ,Kstar_1410M
                   ,m23sq,m12sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoTP* SF_Kstar2_1430CF 
    = new SF_DtoTP((Prefix+"SF_Kstar2_1430CF").c_str()
                   ,(Prefix+"SF_Kstar2_1430CF").c_str()
                   ,Kstar2_1430M
                   ,m23sq,m12sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoVP* SF_Kstar_1680CF 
    = new SF_DtoVP((Prefix+"SF_Kstar_1680CF").c_str()
                   ,(Prefix+"SF_Kstar_1680CF").c_str()
                   ,Kstar1680M
                   ,m23sq,m12sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoVP* SF_Kstar_892DCS 
    = new SF_DtoVP((Prefix+"SF_Kstar_892DCS").c_str()
                   ,(Prefix+"SF_Kstar_892DCS").c_str()
                   ,Kstar892M
                   ,m12sq,m23sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoTP* SF_Kstar2_1430DCS
    = new SF_DtoTP((Prefix+"SF_Kstar2_1430DCS").c_str()
                   ,(Prefix+"SF_Kstar2_1430DCS").c_str()
                   ,Kstar2_1430M
                   ,m12sq,m23sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);

  SF_DtoVP* SF_Kstar_1680DCS
    = new SF_DtoVP((Prefix+"SF_Kstar_1680DCS").c_str()
                   ,(Prefix+"SF_Kstar_1680DCS").c_str()
                   ,Kstar1680M
                   ,m12sq,m23sq,m13sq
                   ,KsM,piPlusM,piPlusM,DZeroM);
  
 
  SF_DtoVP* SF_omega
    = new SF_DtoVP((Prefix+"SF_omega").c_str()
                   ,(Prefix+"SF_omega").c_str()
                   ,omegaM
                   ,m13sq,m12sq,m23sq
                   ,piPlusM,piPlusM,KsM,DZeroM);

  SF_DtoTP* SF_f2_1270
    = new SF_DtoTP((Prefix+"SF_f2_1270").c_str()
                   ,(Prefix+"SF_f2_1270").c_str()
                   ,f2_1270M
                   ,m13sq,m12sq,m23sq
                   ,piPlusM,piPlusM,KsM,DZeroM);

  // cabbibo allowed D decays
  DcyAmplitude Kstar892((Prefix+"Kstar892").c_str()
                        , (Prefix+"Kstar892").c_str()
                        , *SF_Kstar_892CF //DummySpinFactor
                        , m23sq
                        , Kstar892A, Kstar892P
                        , Kstar892M, Kstar892W
                        , piPlusM // measured mass of 1st daughter
                        , piPlusM // nominal mass of 1st daughter
                        , KsM      // measured mass of 2nd daughter
                        , KsM      // nominal mass of 2nd daughter
                        , Radius_r
                        , 1        // spin of K*
                        );

  DcyAmplitude Kstar1410((Prefix+"Kstar1410").c_str()
			   , (Prefix+"Kstar1410").c_str()
			   , *SF_Kstar_1410CF
			   , m23sq
			   , Kstar_1410A, Kstar_1410P
			   , Kstar_1410M, Kstar_1410W
			   , piPlusM // measured mass of 1st daughter
			   , piPlusM // nominal mass of 1st daughter
			   , KsM      // measured mass of 2nd daughter
			   , KsM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 1        // spin of K*
			   );


  DcyAmplitude Kstar0_1430((Prefix+"Kstar0_1430").c_str()
			   , (Prefix+"Kstar0_1430").c_str()
			   , ScalarSpinFactor
			   , m23sq
			   , Kstar0_1430A, Kstar0_1430P
			   , Kstar0_1430M, Kstar0_1430W
			   , piPlusM // measured mass of 1st daughter
			   , piPlusM // nominal mass of 1st daughter
			   , KsM      // measured mass of 2nd daughter
			   , KsM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 0        // spin of K*
			   );

  DcyAmplitude Kstar2_1430((Prefix+"Kstar2_1430").c_str()
			   , (Prefix+"Kstar2_1430").c_str()
			   , *SF_Kstar2_1430CF
			   , m23sq
			   , Kstar2_1430A, Kstar2_1430P
			   , Kstar2_1430M, Kstar2_1430W
			   , piPlusM // measured mass of 1st daughter
			   , piPlusM // nominal mass of 1st daughter
			   , KsM      // measured mass of 2nd daughter
			   , KsM      // nominal mass of 2nd daughter
			   , Radius_r
			   , 2        // spin of K*
			   );

  DcyAmplitude Kstar1680((Prefix+"Kstar1680").c_str()
			 , (Prefix+"Kstar1680").c_str()
			 , *SF_Kstar_1680CF
			 , m23sq
			 , Kstar1680A, Kstar1680P
			 , Kstar1680M, Kstar1680W
			 , piPlusM // measured mass of 1st daughter
			 , piPlusM // nominal mass of 1st daughter
			 , KsM      // measured mass of 2nd daughter
			 , KsM      // nominal mass of 2nd daughter
			 , Radius_r
			 , 1        // spin of K*
			 );

  // cabbibo supressed D decays
  DcyAmplitude CSKstar892((Prefix+"CSKstar892").c_str()
			  , (Prefix+"CSKstar892").c_str()
        , *SF_Kstar_892DCS
			  , m12sq
			  , Kstar892ACs, Kstar892PCs
			  , Kstar892M, Kstar892W
			  , piPlusM // measured mass of 1st daughter
			  , piPlusM // nominal mass of 1st daughter
			  , KsM      // measured mass of 2nd daughter
			  , KsM      // nominal mass of 2nd daughter
			  , Radius_r
			  , 1        // spin of K*
			  );

  DcyAmplitude CSKstar0_1430((Prefix+"CSKstar0_1430").c_str()
			     , (Prefix+"CSKstar0_1430").c_str()
			     , ScalarSpinFactor
			     , m12sq
			     , Kstar0_1430ACs, Kstar0_1430PCs
			     , Kstar0_1430M, Kstar0_1430W
			     , piPlusM // measured mass of 1st daughter
			     , piPlusM // nominal mass of 1st daughter
			     , KsM      // measured mass of 2nd daughter
			     , KsM      // nominal mass of 2nd daughter
			     , Radius_r
			     , 0        // spin of K*
			     );
  DcyAmplitude CSKstar2_1430((Prefix+"CSKstar2_1430").c_str()
			     , (Prefix+"CSKstar2_1430").c_str()
           , *SF_Kstar2_1430DCS
			     , m12sq
			     , Kstar2_1430ACs, Kstar2_1430PCs
			     , Kstar2_1430M, Kstar2_1430W
			     , piPlusM // measured mass of 1st daughter
			     , piPlusM // nominal mass of 1st daughter
			     , KsM      // measured mass of 2nd daughter
			     , KsM      // nominal mass of 2nd daughter
			     , Radius_r
			     , 2        // spin of K*
			     );



  DcyGSAmplitude rho((Prefix+"rho").c_str()
                     , (Prefix+"rho").c_str()
                     ,m13sq
                     ,m12sq
                     ,m23sq
                     ,rhoA,rhoP
                     ,rhoM, rhoW
                     ,piPlusM);
  

  DcyAmplitude omega((Prefix+"omega").c_str()
		     , (Prefix+"omega").c_str()
		     , *SF_omega
		     , m13sq
		     , omegaA, omegaP
		     , omegaM, omegaW
		     , piPlusM // measured mass of 1st daughter
		     , piPlusM // nominal mass of 1st daughter
		     , piMinusM      // measured mass of 2nd daughter
		     , piMinusM      // nominal mass of 2nd daughter
		     , Radius_r
		     , 1        // spin of omega
  );

  DcyAmplitude sigma1((Prefix+"sigma1").c_str()
		      , (Prefix+"sigma1").c_str()
		      ,  ScalarSpinFactor //sigma1_SF
		      , m13sq
		      , sigma1A, sigma1P
		      , sigma1M, sigma1W
		      , piPlusM // measured mass of 1st daughter
		      , piPlusM // nominal mass of 1st daughter
		      , piMinusM      // measured mass of 2nd daughter
		      , piMinusM      // nominal mass of 2nd daughter
		      , Radius_r
		      , 0        // spin of sigma
		      );

  DcyAmplitude sigma2((Prefix+"sigma2").c_str()
		      , (Prefix+"sigma2").c_str()
		      ,  ScalarSpinFactor //sigma2_SF
		      , m13sq
		      , sigma2A, sigma2P
		      , sigma2M, sigma2W
		      , piPlusM // measured mass of 1st daughter
		      , piPlusM // nominal mass of 1st daughter
		      , piMinusM      // measured mass of 2nd daughter
		      , piMinusM      // nominal mass of 2nd daughter
		      , Radius_r
		      , 0        // spin of sigma
		      );

  DcyAmplitude f0_980((Prefix+"f0_980").c_str()
		      , (Prefix+"f0_980").c_str()
		      ,  ScalarSpinFactor //f0_980_SF
		      , m13sq
		      , f0_980A, f0_980P
		      , f0_980M, f0_980W
		      , piPlusM // measured mass of 1st daughter
		      , piPlusM // nominal mass of 1st daughter
		      , piMinusM      // measured mass of 2nd daughter
		      , piMinusM      // nominal mass of 2nd daughter
		      , Radius_r
		      , 0        // spin of f0
		      );

  DcyAmplitude f2_1270((Prefix+"f2_1270").c_str()
		       , (Prefix+"f2_1270").c_str()
		       , *SF_f2_1270
		       , m13sq
		       , f2_1270A, f2_1270P
		       , f2_1270M, f2_1270W
		       , piPlusM // measured mass of 1st daughter
		       , piPlusM // nominal mass of 1st daughter
		       , piMinusM      // measured mass of 2nd daughter
		       , piMinusM      // nominal mass of 2nd daughter
		       , Radius_r
		       , 2        // spin of f2
		       );
  DcyAmplitude f0_1370((Prefix+"f0_1370").c_str()
		       , (Prefix+"f0_1370").c_str()
		       ,  ScalarSpinFactor //f0_1370_SF
		       , m13sq
		       , f0_1370A, f0_1370P
		       , f0_1370M, f0_1370W
		       , piPlusM // measured mass of 1st daughter
		       , piPlusM // nominal mass of 1st daughter
		       , piMinusM      // measured mass of 2nd daughter
		       , piMinusM      // nominal mass of 2nd daughter
		       , Radius_r
		       , 0        // spin of rho
		       );

  DcyGSAmplitude rho1450((Prefix+"rho1450").c_str()
		       , (Prefix+"rho1450").c_str()
                         , m13sq,m12sq,m23sq
		       , rho1450A, rho1450P
		       , rho1450M, rho1450W
		       , piPlusM // measured mass of 1st daughter
		       );

  DcyNonRes NonRes((Prefix+"NonRes").c_str()
                   ,(Prefix+"NonRes").c_str()
                   ,NonResA
                   ,NonResP
                   );
  

  RooArgList* ResonanceListPtr = new RooArgList();
  RooArgList& ResonanceList = *ResonanceListPtr;

  //  ResonanceList.add( *((DcyAmplitude*) Kstar892.clone()));
  //ResonanceList.add( *((DcyAmplitude*) Kstar1410.clone()));
  //ResonanceList.add( *((DcyAmplitude*) Kstar0_1430.clone()));
  //ResonanceList.add( *((DcyAmplitude*) Kstar2_1430.clone()));
  //ResonanceList.add( *((DcyAmplitude*) Kstar1680.clone()));
  //ResonanceList.add( *((DcyAmplitude*) CSKstar892.clone()));
  //ResonanceList.add( *((DcyAmplitude*) CSKstar0_1430.clone()));
  //ResonanceList.add( *((DcyAmplitude*) CSKstar2_1430.clone()));
  //ResonanceList.add( *((DcyGSAmplitude*) rho.clone()));
  //ResonanceList.add( *((DcyAmplitude*) omega.clone()));
  ResonanceList.add( *((DcyAmplitude*) sigma1.clone()));
  ResonanceList.add( *((DcyAmplitude*) sigma2.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_980.clone()));
  //ResonanceList.add( *((DcyAmplitude*) f2_1270.clone()));
  ResonanceList.add( *((DcyAmplitude*) f0_1370.clone()));
  //ResonanceList.add( *((DcyGSAmplitude*) rho1450.clone()));
  ResonanceList.add( *((DcyNonRes*) NonRes.clone()));
  


  return ResonanceList;
}
//
