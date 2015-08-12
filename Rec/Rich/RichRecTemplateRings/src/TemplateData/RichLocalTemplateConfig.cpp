// Include files 

// local
#include "RichLocalTemplateConfig.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichLocalTemplateConfig
//
// 2007-06-14 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichLocalTemplateConfig )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichLocalTemplateConfig::RichLocalTemplateConfig( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  :  RichRingRecToolBase ( type, name , parent )
{
  declareInterface<IRichLocalTemplateConfig>(this);

}

void RichLocalTemplateConfig::resetTemplateHitLists(int rad )
{
  int aNumhit =  tmD()->NumHitsInTemplateValue(rad);
  m_RpnTemplate.clear(); m_RpnTemplate.reserve(aNumhit);
  m_PpnTemplate.clear(); m_PpnTemplate.reserve(aNumhit);  
 
 

  m_RHitScaledTemplate.clear();  m_RHitScaledTemplate.assign(aNumhit,0.0);       
  //  m_ThHitScaledTemplate.clear(); m_ThHitScaledTemplate.assign(aNumhit,0.0);
  
  m_RpnScaledTemplate.clear();   m_RpnScaledTemplate.assign(aNumhit,0.0);
  m_PpnScaledTemplate.clear();   m_PpnScaledTemplate.assign(aNumhit,0.0);  
 
}
StatusCode RichLocalTemplateConfig::ConfigureTemplateHitsForFF(int rad)
{

  resetTemplateHitLists(rad);  
  for(int ir=0; ir<  ( tmD()->NumHitsInTemplateValue(rad)) ; ++ir) {
    double aRadius = tmD()->RHitTemplateValue(ir, rad);
    double aPhi = tmD()->ThHitTemplateValue(ir, rad);
    VD aRnPn = RLocTgC()->CartToPolNumFromRP( aRadius, aPhi);
    m_RpnTemplate.push_back( aRnPn[0]);
    m_PpnTemplate.push_back( aRnPn[1]);
  }
 
  return  StatusCode::SUCCESS;
  
}
StatusCode RichLocalTemplateConfig::ScaleTemplateHits( VI aVec, int itk , int irad ){

  double aScaleFact = RLocTgC()->GetScaleFromLog(aVec);
  int aNumhit =  tmD()->NumHitsInTemplateValue(irad);
  double aZoom = (RConst()-> TargetRadiusZoomFactorValue(irad));
  double aSum=0.0;
  if(aVec[2]> 0 ) {

    m_PpnScaledTemplate= m_PpnTemplate;
    
    for(int i=0; i<aNumhit ; ++i ) {
      m_RHitScaledTemplate[i]= m_RpnTemplate[i]-  aVec[0];
      m_RHitScaledTemplate[i]= ( tmD()->RHitTemplateValue(i,irad) )/( aScaleFact*  aZoom);
      aSum+= m_RHitScaledTemplate[i];      
    }
    
    
  }
  
  double aMean= aSum/aNumhit;

  // info()<<" current found radius "<<irad<<"  "<<itk<<"  "<<aMean<<endmsg;
  

  RRslt()->setTrackFoundRadiusValue(itk,irad,aMean);

   
return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
RichLocalTemplateConfig::~RichLocalTemplateConfig() {} 

//=============================================================================
