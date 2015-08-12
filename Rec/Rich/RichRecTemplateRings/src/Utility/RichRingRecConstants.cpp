// Include files 

#include <TMath.h>

// local
#include "RichRingRecConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecConstants
//
// 2007-06-06 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingRecConstants )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecConstants::RichRingRecConstants( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent ),
    m_RichParticleTrueMass(VD(0)),
    m_radiusSatRingTypical(VD(0)),
    m_angleSatRingTypical(VD(0)),  
    m_radiusScaleFactorToRadians(VD(0))
{
  declareInterface<IRichRingRecConstants>(this);
  InitRichRingRecConstantsParam();
  
}
//=============================================================================
// Destructor
//=============================================================================
RichRingRecConstants::~RichRingRecConstants() {} 

//=============================================================================
void RichRingRecConstants::InitRichRingRecConstantsParam() 
{
  m_Rich1Rich2ZLimit=5000.0;  // Z demarkation between rich1 and rich2.
  m_maxNumRich=2;    // number of rich detectors
  m_maxNumRadiator=3; // number of radiators




  m_TemplateRadiusZoomFactor.clear(); m_TemplateRadiusZoomFactor.resize(m_maxNumRadiator);
     // m_TemplateRadiusZoomFactor[0]=25.0;
     //  m_TemplateRadiusZoomFactor[0]=4.0 ; //june07
    // m_TemplateRadiusZoomFactor[1]=150.0;
     // m_TemplateRadiusZoomFactor[1]=20.0; //june07
    //  m_TemplateRadiusZoomFactor[2]=70.0;

     //  m_TemplateRadiusZoomFactor[0]=2.0;
     // m_TemplateRadiusZoomFactor[1]=10.0;
      //   m_TemplateRadiusZoomFactor[2]=10.0; //june07

  //     m_TemplateRadiusZoomFactor[0]=10.0;
     m_TemplateRadiusZoomFactor[0]=100.0;
     m_TemplateRadiusZoomFactor[1]=50.0;
     m_TemplateRadiusZoomFactor[2]=25.0;

  m_TargetRadiusZoomFactor.clear(); m_TargetRadiusZoomFactor.resize(m_maxNumRadiator);
  //  m_TargetRadiusZoomFactor[0]=2.0;
  m_TargetRadiusZoomFactor[0]=20.0;
  m_TargetRadiusZoomFactor[1]=10.0;
  m_TargetRadiusZoomFactor[2]=5.0;
  

  m_r2gHeavyPartMomThreshold=15600.0;
  m_r1gHeavyPartMomThreshold=9300.0;
  
  
  m_MinTheta=0.0;
  
  m_MaxTheta = (TMath::TwoPi())*4;

   m_NumW = 16*4;
  
   m_NumR = 1024; //std value    

  
   m_ThrCorrVect.clear(); m_ThrCorrVect.resize(m_maxNumRadiator);
   //   m_ThrCorrVect[0]= 0.01;
   m_ThrCorrVect[0]= 0.02;
   //   m_ThrCorrVect[1]= 0.037;
   //   m_ThrCorrVect[1]= 0.03;
   m_ThrCorrVect[1]= 0.02;
   //   m_ThrCorrVect[2]= 0.04;
   m_ThrCorrVect[2]= 0.03;

   m_MaxNominalRadiusForRadiator.clear();
   m_MaxNominalRadiusForRadiator.resize(m_maxNumRadiator);
   m_MaxNominalRadiusForRadiator[0]=400.0;
   m_MaxNominalRadiusForRadiator[1]=85.0;
   m_MaxNominalRadiusForRadiator[2]=160.0;

   m_RadiusSigma.clear();m_RadiusSigma.resize(m_maxNumRadiator);
   m_RadiusSigma[0]=10.0; /// used for mean radius
   // m_RadiusSigma[0]=5.0;
      // m_RadiusSigma[1]=5.0; // used for mean radius
      // m_RadiusSigma[2]=10.0; // used for mean radius
      m_RadiusSigma[1]=6.0; // new from jan 2010
      m_RadiusSigma[2]=8.0; // new from jan 2010

   m_RadiusTightSigma.clear();m_RadiusTightSigma.resize(m_maxNumRadiator);
   //    m_RadiusTightSigma[0]=7.0; // until jan2010
   //m_RadiusTightSigma[0]=4.0;
   //m_RadiusTightSigma[1]=3.0;   // until jan2010
   //m_RadiusTightSigma[1]=5.0;   // until jan2010
   m_RadiusTightSigma[2]=5.0;

   m_RadiusTightSigma[0]=20.0; // for track-assoc-with-hits. from Jan2010
   m_RadiusTightSigma[1]=20.0; // for track-assoc-with-hits. from Jan2010
   m_RadiusTightSigma[2]=20.0; // for track-assoc-with-hits. from Jan2010


   m_radiusSatRingTypical.clear();m_radiusSatRingTypical.resize(m_maxNumRadiator);
   m_angleSatRingTypical.clear(); m_angleSatRingTypical.resize(m_maxNumRadiator);
   m_radiusScaleFactorToRadians.clear(); m_radiusScaleFactorToRadians.resize(m_maxNumRadiator);
   m_radiusSatRingTypical[0]= 330.0;
   m_radiusSatRingTypical[1]= 75.0;
   m_radiusSatRingTypical[2]= 130.0;
   m_angleSatRingTypical[0] = 0.24;
   m_angleSatRingTypical[1] = 0.052;
   m_angleSatRingTypical[2] = 0.03;
   m_radiusScaleFactorToRadians[0]=m_angleSatRingTypical[0]/m_radiusSatRingTypical[0];
   m_radiusScaleFactorToRadians[1]=m_angleSatRingTypical[1]/m_radiusSatRingTypical[1];
   m_radiusScaleFactorToRadians[2]=m_angleSatRingTypical[2]/m_radiusSatRingTypical[2];
   

  setParticleTrueMass();
  setParticleCherenkovThreshold ();
  setParticleIDParam();

}
void  RichRingRecConstants::setParticleTrueMass() {
  m_RichParticleTrueMass.clear();
  m_RichNumParticleSpecies=5;
  m_RichParticleTrueMass.resize(m_RichNumParticleSpecies);
  
  m_RichParticleTrueMass[0]=0.51;  //electron
  m_RichParticleTrueMass[1]=105.66; // muon
  m_RichParticleTrueMass[2]=139.57; // pion
  m_RichParticleTrueMass[3]=493.68; // kaon
  m_RichParticleTrueMass[4]=938.27; // proton

  m_RichParticleTrueCode.clear();
  m_RichParticleTrueCode.resize(m_RichNumParticleSpecies);
  m_RichParticleTrueCode[0]=11;
  m_RichParticleTrueCode[1]=13;
  m_RichParticleTrueCode[2]=211;
  m_RichParticleTrueCode[3]=321;
  m_RichParticleTrueCode[4]=2212;
  

  m_RichLightMassBegin= 1.0;
  m_RichLightMassEnd = 300.0;
  
  m_RichHeavyMassBegin=300.0;
  
  m_RichHeavyMassEnd= 1200.0;
  

}
void RichRingRecConstants::setParticleCherenkovThreshold (){
  m_RichParticleCherenkovThreshold.clear();
  m_RichParticleCherenkovThreshold.resize(m_maxNumRadiator); // for the three radiators
  m_MinMomTrack.clear();
  m_MaxMomTrack.clear();
  m_MinMomTrack.resize(m_maxNumRadiator); // for the three radiators
  m_MaxMomTrack.resize(m_maxNumRadiator); // for the three radiators

  m_MinMomTrack[0]= 1000.0;  // in MeV
  m_MinMomTrack[1]= 1000.0;  // in MeV
  m_MinMomTrack[2]= 1000.0;  // in MeV

  m_MaxMomTrack[0]= 30000.0;  // in MeV
  m_MaxMomTrack[1]= 100000.0;  // in MeV
  m_MaxMomTrack[2]= 120000.0;  // in MeV

  for(int i=0; i<m_maxNumRadiator; ++i) {
    m_RichParticleCherenkovThreshold [i].assign(m_RichNumParticleSpecies, 0.0);
  }
  // for now only the pion values are used for all light particles
  // and Kaon for all heavy particles. This is to be modified with the correct values.
  // first index is the radiator and the second index is the particle type (0->4 for
  // eln mu pi ka p

  m_RichParticleCkvThrTolerence= 1500;
  

  m_RichParticleCherenkovThreshold [0] [0] = 600.0;
  m_RichParticleCherenkovThreshold [0] [1] = 600.0;
  m_RichParticleCherenkovThreshold [0] [2] = 600.0;
  m_RichParticleCherenkovThreshold [0] [3] = 2000.0;
  m_RichParticleCherenkovThreshold [0] [4] = 2000.0;

  m_RichParticleCherenkovThreshold [1] [0] = 2600.0;
  m_RichParticleCherenkovThreshold [1] [1] = 2600.0;
  m_RichParticleCherenkovThreshold [1] [2] = 2600.0;
  m_RichParticleCherenkovThreshold [1] [3] = 9300.0+m_RichParticleCkvThrTolerence;
  m_RichParticleCherenkovThreshold [1] [4] = 9300.0+m_RichParticleCkvThrTolerence;
  // m_RichParticleCherenkovThreshold [1] [3] = 9700.0;
  // m_RichParticleCherenkovThreshold [1] [4] = 9700.0;

  m_RichParticleCherenkovThreshold [2] [0] = 4400.0;
  m_RichParticleCherenkovThreshold [2] [1] = 4400.0;
  m_RichParticleCherenkovThreshold [2] [2] = 4400.0;
  m_RichParticleCherenkovThreshold [2] [3] = 15600.0+m_RichParticleCkvThrTolerence;
  m_RichParticleCherenkovThreshold [2] [4] = 15600.0+m_RichParticleCkvThrTolerence;
  

   

}
void RichRingRecConstants::setParticleIDParam() {
  

    m_meanRadiusDiff.clear();
    m_sigmaRadiusDiff.clear(); 
    m_meanRadiusDiff.resize(m_maxNumRadiator);
    m_sigmaRadiusDiff.resize(m_maxNumRadiator);
    m_meanRadiusDiff[0]=0.5;
    m_meanRadiusDiff[1]=3.9;
    m_meanRadiusDiff[2]=1.15;
    m_sigmaRadiusDiff[0]= 3.8;
    m_sigmaRadiusDiff[1]= 2.8;
    m_sigmaRadiusDiff[2]= 3.9;
   
  }

double RichRingRecConstants::RichParticleTrueMassValue(int isp ) 
{
  double aMass = -140.0;
  if( (isp >= 0)  &&  (isp <  m_RichNumParticleSpecies) ) {
    aMass =  m_RichParticleTrueMass[isp]; 
  }
  return aMass;
  
}
int RichRingRecConstants::RichParticleTypeFromTruePidCodeValue(int acode ){
  int asp=2;
  
  int isp=0;  
  bool aFound= false;
  
  while(isp <  m_RichNumParticleSpecies &&  (! aFound ) ) {
    if( abs(acode) ==   m_RichParticleTrueCode[isp] ){
      asp = isp;
      aFound=true;
    }
    isp++;
    
  }
  return asp;
}

int RichRingRecConstants::estimatePidFromMassAlone( double amass) 
{
  int aestimate=2;  
  if( amass < 0.0 ) {
    aestimate=-1;
    
  }else if(amass < m_RichParticleTrueMass [0] )  { 
      aestimate=0;
      
  }else if(amass >  m_RichParticleTrueMass [4] )  { 
      aestimate=4; 
  }else {
     bool estfound=false;
     int i=0;
     while ( (i< (m_RichNumParticleSpecies-1)) && !(estfound) ) {
      double alow= m_RichParticleTrueMass[i];
      double ahigh= m_RichParticleTrueMass[i+1];
      if( (amass >= alow) &&  (amass <= ahigh) ) {
        if( (amass- alow) < (ahigh-amass) ) {
          aestimate=i;
        }else {
          aestimate=i+1;
        }        
        estfound=true;        
      }
      
      i++;      
     }   
  }
  return aestimate;
}
