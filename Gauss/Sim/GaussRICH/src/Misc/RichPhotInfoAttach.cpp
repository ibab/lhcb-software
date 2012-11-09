// $Id: RichPhotInfoAttach.cpp,v 1.1 2008-01-21 16:59:01 seaso Exp $
// Include files 

#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichPEInfo.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussTools/GaussTrackInformation.h"
// local
#include "GaussRICH/RichPhotInfoAttach.h"

void setRichHpdQwPcReflInfo(const G4Track& aPhotonTk,G4int aFlag){




   G4VUserTrackInformation* aUserTrackInfo=aPhotonTk.GetUserInformation();
          GaussTrackInformation* aRichPhotTrackInfo
             = (GaussTrackInformation*)aUserTrackInfo;
   
        if( aRichPhotTrackInfo) 
        {
          if( aRichPhotTrackInfo->detInfo() )
          {
            RichInfo* aRichTypeInfo =
               ( RichInfo*) (aRichPhotTrackInfo->detInfo());
            if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() )
            {
              RichPhotInfo* aRichPhotInfo = 
              aRichTypeInfo-> RichPhotInformation();
              if( aRichPhotInfo ) 
              {
	            	aRichPhotInfo->setQW2PCreflFlag(aFlag);	
                if(aFlag  > 0 ) {
                 aRichPhotInfo->setQW2PCreflFlagSave(aFlag);
                }
                
              }
            }
            
          }
        }

       
}

G4int getRichHpdQwPcReflInfo(const G4Track& aPhotonTk){
  

        G4int aQW2PCreflFlag=0;

        G4VUserTrackInformation* aUserTrackInfo=aPhotonTk.GetUserInformation();
        GaussTrackInformation* aRichPhotTrackInfo
            = (GaussTrackInformation*)aUserTrackInfo;
   
        if( aRichPhotTrackInfo) 
        {
          if( aRichPhotTrackInfo->detInfo() )
          {
            RichInfo* aRichTypeInfo =
               ( RichInfo*) (aRichPhotTrackInfo->detInfo());
            if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() )
            {
              RichPhotInfo* aRichPhotInfo = 
              aRichTypeInfo-> RichPhotInformation();
              if( aRichPhotInfo ) 
              {
            		aQW2PCreflFlag=aRichPhotInfo->QW2PCreflFlag();	
              }
            }
            
            
          }
          
        }



        return aQW2PCreflFlag;   
}

G4int getRichHpdQwPcReflInfoSave(const G4Track& aPhotonTk){


  
        G4int aQW2PCreflFlagSave=0;

        G4VUserTrackInformation* aUserTrackInfo=aPhotonTk.GetUserInformation();
        GaussTrackInformation* aRichPhotTrackInfo
            = (GaussTrackInformation*)aUserTrackInfo;
   
        if( aRichPhotTrackInfo) 
        {
          if( aRichPhotTrackInfo->detInfo() )
          {
            RichInfo* aRichTypeInfo =
               ( RichInfo*) (aRichPhotTrackInfo->detInfo());
            if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() )
            {
              RichPhotInfo* aRichPhotInfo = 
              aRichTypeInfo-> RichPhotInformation();
              if( aRichPhotInfo ) 
              {
            		aQW2PCreflFlagSave=aRichPhotInfo->QW2PCreflFlagSave();	
              }
            }
            
            
          }
          
        }



        return aQW2PCreflFlagSave;   
}

