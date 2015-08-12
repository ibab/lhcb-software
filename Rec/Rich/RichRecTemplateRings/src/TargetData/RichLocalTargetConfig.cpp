// Include files 

#include <TMath.h>
#include <TRandom.h>
#include <TRandom2.h>

// local
#include "RichLocalTargetConfig.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichLocalTargetConfig
//
// 2007-06-13 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichLocalTargetConfig )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichLocalTargetConfig::RichLocalTargetConfig( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent ),
    m_rMax_Radiator(VD(0)),
    m_HitIsSelected(VB(0)) 
{
  declareInterface<IRichLocalTargetConfig>(this);
}

StatusCode  RichLocalTargetConfig::InitTargetConfigParam()
{


  m_rMinNominal =20.0;
  m_rMin= m_rMinNominal;
  
  
  m_rMinNominalR2gLowP=80.0;
  
  m_rMinNominalR2gHighP=30.0;
  
  m_rMinNominalR1gLowP=20.0;
  //m_rMinNominalR1gLowP=10.0;
  
  m_rMinNominalR1gHighP=20.0;
  
    m_rMinNominalAgel=100.0;
  //m_rMinNominalAgel=50.0;
  
  m_rMin_Shift=2.0;

  m_Maxradius_SatAgel=RConst()->MaxNominalRadiusForRadiatorValue(0);
  m_Maxradius_SatR1g=RConst()->MaxNominalRadiusForRadiatorValue(1);
  m_Maxradius_SatR2g=RConst()->MaxNominalRadiusForRadiatorValue(2);


  m_rMax_Radiator.clear();  m_rMax_Radiator.reserve(3);
  m_rMax_Radiator.push_back(m_Maxradius_SatAgel*(RConst()-> TemplateRadiusZoomFactorValue(0))  );
  m_rMax_Radiator.push_back(m_Maxradius_SatR1g*(RConst()-> TemplateRadiusZoomFactorValue(1)));
  m_rMax_Radiator.push_back(m_Maxradius_SatR2g*(RConst()-> TemplateRadiusZoomFactorValue(2)));
  
  m_rMax=m_rMax_Radiator[0]; // just a nominal value. changed for each radiator.
  
  m_rRatio =  m_rMax/m_rMin;  // just a nominal value

  m_cMinTheta = RConst()->MinTheta();
  
  m_cMaxTheta= RConst()->MaxTheta();
  
  m_cNumW= RConst()-> NumW();
  
  m_cNumR=RConst()-> NumR();


  m_RandomNumSeedForLocalPhi=new TRandom2();
  
  
return StatusCode::SUCCESS;
}
void RichLocalTargetConfig::SetRadiatorParam(int irad)
{
  
     m_rMax =  m_rMax_Radiator[irad];
   
  
}


StatusCode RichLocalTargetConfig::SelectHitsInFoi(int itk, int irad , int iRich ){
  StatusCode sc = StatusCode::SUCCESS;
  

  double tkRecMom  = tgD()->TrackRecMomValue(itk,irad);

  VD tkExpRadii = Tfm()->TrackExpectedRadii(itk,irad);

  double aRadiusSigma = RConst()->RadiusSigmaValue(irad);
    
  
  m_rMin= m_rMinNominal;


  double aFoiMinCut =  m_rMin;
  double aFoiMaxCut = m_rMax/ (RConst()-> TemplateRadiusZoomFactorValue(irad));
     
  
  if(irad ==2 ) {    // rich2 gas
    if( tkRecMom  < ( RConst()->  r2gHeavyPartMomThreshold()) ) {
      m_rMin= m_rMinNominalR2gLowP;  // valid for light particles
      
    }else {
      m_rMin=  m_rMinNominalR2gHighP; // valid for light and heavy particles
      
    }
  }else if (irad ==1 ) {
    
    if( tkRecMom  < ( RConst()->  r1gHeavyPartMomThreshold()) ) {
      m_rMin= m_rMinNominalR1gLowP;  // valid for light particles
      
    }else {
      m_rMin=  m_rMinNominalR1gHighP; // valid for light and heavy particles
      
    }  
    
  }else if (irad == 0 ) { // aerogel
    m_rMin =  m_rMinNominalAgel;
    
    
  }

  // the min max radius from the inputs from
  //  other rich reconstruction packages do not look reliable.
  // Hence using the estimates made in this package for the 
  // min and max regions.


    aFoiMinCut = m_rMin;

    aFoiMaxCut=  m_rMax/ (RConst()-> TemplateRadiusZoomFactorValue(irad));
    

    m_rRatio =  m_rMax/m_rMin  ;

   

  //debug test print
  //   debug()<<"RichRingRecSelectHits rad itk FoiMin FoiMax mrMin mrMax mrRatio minrad maxrad radsigma "
  // <<irad<<"  "<<itk
  //      <<"   "<< aFoiMinCut<<"   "<< aFoiMaxCut<<"  "<< m_rMin<<"  "<<m_rMax
  //       <<"   "<<m_rRatio<<"  "
  //          <<"   "<<tMinRadInp<<"  "<<tMaxRadInp<< "  "<<aRadiusSigma<<endmsg;
   //
  // end debug test print
 
  m_HitIsSelected.clear();
  int  aNumHitsTarget = tgD()->NumHitsTarget(iRich);
  m_HitIsSelected.assign( aNumHitsTarget,false);
  for(int i=0; i<aNumHitsTarget; ++i){
    double aradius= Tfm()->  CartToPolRadFromTHNum(i,itk,irad);

    //    if( (aradius < aFoiMaxCut+aRadiusSigma) && (aradius > aFoiMinCut-aRadiusSigma) ) {  
    //    if( (aradius < aFoiMaxCut) && (aradius > aFoiMinCut) ) {  
     if( (aradius < aFoiMaxCut-aRadiusSigma ) && (aradius > aFoiMinCut) ) {  

      // special case for aerogel: check if the hit is already used in the
      // gas ring.
      // also use selection bands in aerogel and rich2gas.
      // no selection bands to be used for any radiator. Jan 2010.

          if(irad == 1 ) {

                   m_HitIsSelected[i]=true;             
            
          }else {  // now for aerogel
            
          
           if( (irad == 0) &&  (RRslt()-> HitSelectedInRecRingsValue (i,1))) {
              // hit already used for a rich1 gas ring and not to be used for aerogel
           } else {
             // the following commented Jan 2010.
             //  int ip=0;
             //  while( ( m_HitIsSelected[i] == false ) && ip < (  RConst()-> RichNumParticleSpecies())) {
             //  double aExpRad = Tfm()-> TrackExpectedRadiusValue(itk,irad,ip);
             //  if( (aExpRad > 0.0) && (fabs(aradius-aExpRad) < aRadiusSigma) ) {
             //   m_HitIsSelected[i]=true;
             //  }
             //  ip++;
             //  }
                m_HitIsSelected[i]=true;
              
           }
           
           
          }
          
          
      
    }   
    
  }
  
  
  

  return sc;
  
}
StatusCode RichLocalTargetConfig::ConfigureTargetHitsInFoiForFF(int itk, int irad , int iRich ){
  StatusCode sc = StatusCode::SUCCESS;

  resetTargetHitLists( iRich);
  
  int  aNumHitsTarget = tgD()->NumHitsTarget(iRich);
  for(int i=0; i<aNumHitsTarget; ++i){
    if(  m_HitIsSelected[i] ) {
      double aZoom = (RConst()-> TargetRadiusZoomFactorValue(irad));
      
      double rhitZoom = ( Tfm()->  CartToPolRadFromTHNum(i,itk,irad))* aZoom;      
      double xhitZoom = ( Tfm()->  XDiffValue(i,itk,irad,iRich) )*aZoom;
      double yhitZoom = ( Tfm()->  YDiffValue(i,itk,irad,iRich) )*aZoom;
      double phihit =   ( Tfm()->  CartToPolPhiFromTHNum(  i,itk,irad) );
      
      if(RParam()->ActivateRandomPhiLocalScaling() ) {
        double aRan = ( m_RandomNumSeedForLocalPhi->Rndm())* TMath::TwoPi();
        phihit -= aRan;
        if(phihit <0 ) phihit += TMath::TwoPi();        
      }
      
      int aPhiIndex = Tfm()-> SearchIndexInVect( m_ThHitTarget,phihit);
      m_ThHitTarget.insert(m_ThHitTarget.begin()+ aPhiIndex,  phihit);
      m_XHitTarget.insert(m_XHitTarget.begin()+ aPhiIndex, xhitZoom);
      m_YHitTarget.insert(m_YHitTarget.begin()+ aPhiIndex, yhitZoom);
      m_RHitTarget.insert(m_RHitTarget.begin()+ aPhiIndex,rhitZoom);
      VD aRnPn = CartToPolNumFromRP(rhitZoom,phihit);
      m_RpnTarget.insert(m_RpnTarget.begin()+ aPhiIndex,aRnPn[0]);  
      m_PpnTarget.insert(m_PpnTarget.begin()+ aPhiIndex,aRnPn[1]);
      m_NumHitsSelectedForATrack++;
      
      
    }
    
    
  }
  


  return sc;
  
}
StatusCode RichLocalTargetConfig::EstimateMeanRadiusFromRawRadius( int itk, int irad , int iRich ){

  StatusCode sc = StatusCode::SUCCESS;

  double aRawRadius = RRslt()->TrackFoundRadiusValue(itk,irad);
  double aMeanFoundRadius=aRawRadius;
  
  double aRadiusWidth=RConst()->RadiusSigmaValue(irad);
  double aRadiusTightWidth=RConst()->RadiusTightSigmaValue(irad);
    
  double aSum=0.0;
  double aCount=0.0;
  //debug()<<" Rich MeanRadius get the meanradius "<< irad<<"  "<<itk<<"  "<<aRawRadius << "   "<<aRadiusWidth<<endmsg;
  
  if(aRawRadius > 0.0 ) {
    int  aNumHitsTarget = tgD()->NumHitsTarget(iRich);
    for(int i=0; i<aNumHitsTarget; ++i){    

      if(  m_HitIsSelected[i] ) {
        double rhit =  Tfm()->  CartToPolRadFromTHNum(i,itk,irad);       
        // debug()<<"Meanradius finder  hitindex rhit radiusdiff "<<i<<"  "<<rhit<<"  "<< rhit - aRawRadius << endmsg;
        
        if(fabs( rhit - aRawRadius ) < aRadiusWidth  ) {
          aSum += rhit;
          aCount++;               
        }        
       if(fabs( rhit - aRawRadius ) < aRadiusTightWidth  ) {
         RRslt()->setHitSelectedInRecRingsValue(i,irad,true);                         
        }        

      }      
    }
    if( aCount != 0.0) {      
      aMeanFoundRadius = aSum/aCount;      
    }
    
    
    RRslt()->setTrackFoundMeanRadiusValue(itk,irad,aMeanFoundRadius );
    RRslt()-> setTrackFoundMRNumHits(itk, irad,aCount);
    
    
  }
  
  return sc;

}

void RichLocalTargetConfig::resetTargetHitLists(int iRich)
{
  int aNumData = tgD()->NumHitsTarget(iRich);
  
  m_XHitTarget.clear();  m_XHitTarget.reserve( aNumData);
  m_YHitTarget.clear();  m_YHitTarget.reserve( aNumData);
  m_RHitTarget.clear();  m_RHitTarget.reserve( aNumData);
  m_ThHitTarget.clear(); m_ThHitTarget.reserve( aNumData);
  m_RpnTarget.clear();   m_RpnTarget.reserve( aNumData);
  m_PpnTarget.clear();   m_PpnTarget.reserve( aNumData);
  m_NumHitsSelectedForATrack=0;
  
  
}

VD RichLocalTargetConfig::CartToPolNumFromRP(double R, double P){

  double arR= (R/ m_rMin);
  if(   arR < 1.0 ) arR=1.0;
  double  pp = log(arR);
  double RVal = (m_cNumR -1 ) * pp/ log (m_rRatio);
  double ThVal =  (m_cNumW-1)* P/ (TMath::TwoPi());
  VD c(2);
  c[0]= RVal ;
  c[1] = ThVal;  
  return c;  
}

double RichLocalTargetConfig::GetScaleFromLog ( VI aVec ) {
  double aScale=1.0;
   if( aVec[2] > 0 ) {
    double aLogRShift= 1.0* aVec[0];
    double aLogRatio= aLogRShift *   log (m_rRatio) / (m_cNumR -1 );
    aScale = exp(aLogRatio);
  }
 
  return aScale;

}
//=============================================================================
// Destructor
//=============================================================================
RichLocalTargetConfig::~RichLocalTargetConfig() {} 

//=============================================================================
