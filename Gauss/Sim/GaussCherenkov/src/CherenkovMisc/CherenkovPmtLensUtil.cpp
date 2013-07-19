// Include files 


#include <iostream>


// local
#include "GaussCherenkov/CherenkovPmtLensUtil.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "DetDesc/DetectorElement.h"
#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "GaussRICH/RichG4GaussPathNames.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovPmtLensUtil
//
// 2012-11-15 : Sajan Easo
//-----------------------------------------------------------------------------
CherenkovPmtLensUtil* CherenkovPmtLensUtil::CherenkovPmtLensUtilInstance=0;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovPmtLensUtil::CherenkovPmtLensUtil(  ):
m_RichPmtModuleLensFlag(std::vector<bool> (300)){

  TestLensExistance();
  InitCherenkovPmtLensUtil();
  

}
//=============================================================================
// Destructor
//=============================================================================
CherenkovPmtLensUtil::~CherenkovPmtLensUtil() {} 

//=============================================================================
CherenkovPmtLensUtil* CherenkovPmtLensUtil::getInstance()
{
  if(CherenkovPmtLensUtilInstance == 0) {
    CherenkovPmtLensUtilInstance = new  CherenkovPmtLensUtil();   
  }

 return  CherenkovPmtLensUtilInstance;  
}

bool CherenkovPmtLensUtil::isPmtModuleWithLens(int aModuleCopyNum){
  bool aLensFlag=false;
    

  if(aModuleCopyNum < m_numModuleRich1 ) {

    if(m_LensExist >=1 ) {  
      aLensFlag=  m_RichPmtModuleLensFlag[aModuleCopyNum];
    }else {
      aLensFlag=false;
      
    }
  }else {
    aLensFlag=false;
  }
  
  
  return  aLensFlag;
  
}
  
bool CherenkovPmtLensUtil::isPmtWithLens(int aPmtCopyNum){
  int aModuleNum=aPmtCopyNum/16;
  return isPmtModuleWithLens(aModuleNum);
}
void CherenkovPmtLensUtil::TestLensExistance() {
   m_LensExist=0;
  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);  
  if(Rich1DE ) {
    if(Rich1DE->exists ("Rich1PmtLensPresence") ){
      m_LensExist= Rich1DE->param<int>("Rich1PmtLensPresence"  );
    }
    
  }
  
}

void  CherenkovPmtLensUtil::InitCherenkovPmtLensUtil(){

  int numRowInR1= 14;
  int numColInR1=12;
  m_numModuleRich1=numRowInR1*numColInR1 ;  // to be acquired from DB in the future.

  m_RichPmtModuleLensFlag.clear();
  m_RichPmtModuleLensFlag.resize(m_numModuleRich1);

  if(m_LensExist == 0 ) {
    for(int j=0;j<m_numModuleRich1; ++j ) {
      m_RichPmtModuleLensFlag[j]=false;
    }
    
    
  }else if(m_LensExist >=1 )   {
    
  

  for (int r=0; r<numRowInR1  ; ++r ) {
    for(int c=0; c<numColInR1; ++c ){
      int m=(r*numColInR1)+c;
      if( (c<3) || (c>8) ) {

        m_RichPmtModuleLensFlag[m]=true;
        
      }else {
        
        m_RichPmtModuleLensFlag[m]=false;
        
      }
      
    }
    
    
  }
  
  }
  
  
}
 
 
