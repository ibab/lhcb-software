// Include files 
//#include "DetDesc/ILVolume.h"

#include "DetDesc/IPVolume.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "DetDesc/IGeometryInfo.h"




// local
#include "GaussCherenkov/CkvGeometrySetupUtil.h"
#include "GaussRICH/RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CkvGeometrySetupUtil
//
// 2012-12-12 : Sajan Easo
//-----------------------------------------------------------------------------
CkvGeometrySetupUtil* CkvGeometrySetupUtil::CkvGeometrySetupUtilInstance=0;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvGeometrySetupUtil::CkvGeometrySetupUtil() : 
  m_superRichFlag(false),
  m_classicTwoRichFlag(true),
  m_horizontalRich1Flag(false),
  m_optVerticalRichFlag(false),
  m_optHorizontalRichFlag(false) 
{

  InitCkvGeometrySuperRichFlag();
  InitCkvGeometryTwoRichFlags();
  
  
}
//=============================================================================
// Destructor
//=============================================================================
CkvGeometrySetupUtil::~CkvGeometrySetupUtil() {} 

//=============================================================================
CkvGeometrySetupUtil* CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance()
{
  if(CkvGeometrySetupUtilInstance == 0 ) {
      
    CkvGeometrySetupUtilInstance = new CkvGeometrySetupUtil();
  }
  return   CkvGeometrySetupUtilInstance;
}
//=============================================================================

void CkvGeometrySetupUtil::InitCkvGeometrySuperRichFlag(){


  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvGeometrylog( msgSvc, "CkvGeometry" );
  SmartDataPtr<DetectorElement> AfterMagDe(detSvc,RichAfterMagnetRegionStructurePathName );
  if(! AfterMagDe ) {
    CkvGeometrylog<<MSG::ERROR<<"Can't find detelem for AfterMagnet region from  "
                 << RichAfterMagnetRegionStructurePathName<< endmsg;
    
    
  }else {
    const IPVolume* aSuperRichVol=AfterMagDe->geometry()->lvolume()->pvolume("pvSuperRichMaster" );
    if(aSuperRichVol) m_superRichFlag=true; 
  }
  
  
  
}
//=============================================================================
void CkvGeometrySetupUtil::InitCkvGeometryTwoRichFlags(){
  if(m_superRichFlag ) {
    m_classicTwoRichFlag=false;
    m_horizontalRich1Flag=false;
    m_optVerticalRichFlag=false;
    m_optHorizontalRichFlag=false;
    
  }else {
    IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
    IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
    MsgStream CkvGeometrylog( msgSvc, "CkvGeometry" );
    SmartDataPtr<DetectorElement> BeforeMagDe(detSvc,RichBeforeMagnetRegionStructurePathName );
    if(! BeforeMagDe ) {
       CkvGeometrylog<<MSG::ERROR<<"Can't find detelem for BeforeMagnet region from  "
                 << RichBeforeMagnetRegionStructurePathName<< endmsg;
    
    }else {
  
       const IPVolume* aClassicRichVol=BeforeMagDe->geometry()->lvolume()->pvolume("pvRich1Master" );
       if( aClassicRichVol ) {
        SmartDataPtr<DetectorElement> Rich1DE (  detSvc, Rich1ClassicDeStructurePathName );
        if(!Rich1DE ) {
             CkvGeometrylog<<MSG::ERROR<<"Can't find detelem for RICH1 region from  "
                                        <<   Rich1ClassicDeStructurePathName << endmsg;
         
        }else {
          double aSphMirRad= Rich1DE ->param<double>( "Rich1Mirror1NominalRadiusC"   );
          double aSphMirrXCOCPos = 0.0;
          if(Rich1DE ->exists("Rich1Mirror1NominalCCX" )) aSphMirrXCOCPos = Rich1DE ->param<double>("Rich1Mirror1NominalCCX");

          if(aSphMirRad < 3000.0)  {  
            if(aSphMirrXCOCPos < 300.0 ) {
              
              m_classicTwoRichFlag=true;
            }else {
              m_horizontalRich1Flag=true;
              
            }
            
            
            
          }else {
            if(aSphMirrXCOCPos < 300.0 ) {  
              m_optVerticalRichFlag=true;
            }else {
              m_optHorizontalRichFlag=true;
              
            } 
          } // end test on ROC   
          
        } // end test rich1DE      
        
       }// end test classicRICH
       
       
        
       
    } // end test beforeMagRegion
    
    
  }
  
  
}

//=============================================================================










