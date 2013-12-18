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
  m_optHorizontalRichFlag(false),
  m_NumPmtInModule(16),
  m_MaxNumModuleRich1(168),
  m_MaxNumModuleRich2(182),
  m_NumModuleInRich2ModuleArrayCol(13),
  m_Rich2MixedModuleArrayColumnSize(std::vector<int> (3)),
  m_ModuleWithGrandPMT(std::vector<bool> (350)) 
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

  // Set some defaults

  m_Rich1PmtArrayConfig=0;
  m_Rich2PmtArrayConfig=0;
  m_Rich2_UseGrandPmt=false;
  
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


          if(Rich1DE ->exists ("Rich1PMTArrayConfig") ) m_Rich1PmtArrayConfig=Rich1DE ->param<int>("Rich1PMTArrayConfig");
          if(Rich1DE ->exists ("Rich2PMTArrayConfig") ) m_Rich2PmtArrayConfig=Rich1DE ->param<int>("Rich2PMTArrayConfig");
           SetupGrandPMTConfig();

          


        } // end test rich1DE      
        
       }// end test classicRICH
       
       
        
       
    } // end test beforeMagRegion
    
    
  }
  
  if(m_Rich2PmtArrayConfig >= 1 )  {

    m_Rich2_UseGrandPmt=true;
  }
  
  
}

//=============================================================================

void CkvGeometrySetupUtil::SetupGrandPMTConfig()  {
    IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
    IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
    MsgStream CkvGeometrylog( msgSvc, "CkvGeometry" );
    SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1ClassicDeStructurePathName);

    if( !Rich1DE ){
          CkvGeometrylog<<MSG::ERROR<<"Can't find detelem for RICH1 from  "
                        <<   Rich1ClassicDeStructurePathName << "       for  CkvGeometrySetupUtil "<< endmsg;
    }else {
      
      if( Rich1DE->exists("RichTotNumPmtInModule"))  m_NumPmtInModule = Rich1DE->param<int>("RichTotNumPmtInModule") ;
      if( Rich1DE->exists("Rich1TotNumModules"))  m_MaxNumModuleRich1=Rich1DE->param<int>("Rich1TotNumModules");
      if( Rich1DE->exists("Rich2TotNumModules"))  m_MaxNumModuleRich2=Rich1DE->param<int>("Rich2TotNumModules");
      // if( Rich1DE->exists("Rich2NumberOfModulesInRow")) m_NumModuleInRich2ModuleArrayRow= ( Rich1DE->param<int>("Rich2NumberOfModulesInRow"));
      if( Rich1DE->exists("Rich2NumberOfModulesInCol"))   m_NumModuleInRich2ModuleArrayCol= ( Rich1DE->param<int>("Rich2NumberOfModulesInCol")) ;
      if(Rich1DE->exists("Rich2MixedNumModulesArraySetup") )
               m_Rich2MixedModuleArrayColumnSize = Rich1DE->param<std::vector<int> >("Rich2MixedNumModulesArraySetup");






      int m_ModuleTot= m_MaxNumModuleRich1+ m_MaxNumModuleRich2;
      // m_ModuleWithGrandPMT.resize(m_ModuleTot);
      m_ModuleWithGrandPMT.assign(m_ModuleTot,false);


      // now set the flags for modules in RICH2 according to the geometry setup

      if(m_Rich2PmtArrayConfig >= 1 ) {
        
          for (int im=m_MaxNumModuleRich1; im<m_ModuleTot; ++im) {
            if( m_Rich2PmtArrayConfig == 1 ) {
               m_ModuleWithGrandPMT[im]=true;
          
            }else if ( m_Rich2PmtArrayConfig == 2 ) {
              int imLocal=im- m_MaxNumModuleRich1;
              if(imLocal >(m_MaxNumModuleRich2/2) ) imLocal -= (m_MaxNumModuleRich2/2);
              int aColNum= imLocal/m_NumModuleInRich2ModuleArrayCol;
              int aRowNum= imLocal - aColNum*m_NumModuleInRich2ModuleArrayCol;
             if( (aRowNum < m_Rich2MixedModuleArrayColumnSize[0] ) || 
                 (aRowNum >= ( m_Rich2MixedModuleArrayColumnSize[0]+ m_Rich2MixedModuleArrayColumnSize[1] ) ) ) {
               m_ModuleWithGrandPMT[im] = true;   
             }
              
             
            }
        
            
          }// end loop over modules
          
      }// end test on arrayflag
      
      
    }// end test on Rich1DE
    
    
  
  
}









