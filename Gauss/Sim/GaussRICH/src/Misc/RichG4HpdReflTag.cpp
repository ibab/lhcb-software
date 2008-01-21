// $Id: RichG4HpdReflTag.cpp,v 1.1 2008-01-21 16:59:00 seaso Exp $
// Include files
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"
#include "RichG4HpdReflectionFlag.h"
#include <bitset> 

// local
#include "RichG4HpdReflTag.h"


extern void RichG4HpdReflectionTag(const G4Track& aPhotonTk,
                                   G4int aFlagPos ){
  


  
  G4VUserTrackInformation* aUserTrackInfo=aPhotonTk.GetUserInformation();
  GaussTrackInformation* aRichPhotTrackInfo
    = (GaussTrackInformation*)aUserTrackInfo;
  if( aRichPhotTrackInfo) {
    if( aRichPhotTrackInfo->detInfo() ){
      RichInfo* aRichTypeInfo =
        ( RichInfo*) (aRichPhotTrackInfo->detInfo());

      if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
        RichPhotInfo* aRichPhotInfo =
          aRichTypeInfo-> RichPhotInformation();
        if( aRichPhotInfo ) {
          // now add the info
          if( aRichPhotInfo -> VerbosePhotTagFlag() ) {
           
            //RichG4HpdReflectionFlag* aRichG4HpdReflectionFlag= RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();
            // G4int aMaxFlag=aRichG4HpdReflectionFlag->MaxNumHpdReflFlag();
            
            std::bitset<16>aCurFl((unsigned long ) (aRichPhotInfo->PhotonHpdReflectionFlag()));
             aCurFl.set(aFlagPos,1);           
             aRichPhotInfo->setPhotonHpdReflectionFlag( (G4int) (aCurFl.to_ulong()));
          }
          
        }
        
      }
      
    }
    
  }  


}


