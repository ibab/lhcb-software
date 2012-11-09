// $Id: RichG4HpdReflTag.cpp,v 1.3 2008-01-31 13:53:47 seaso Exp $
// Include files
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichG4HpdReflectionFlag.h"
#include <bitset> 

// local
#include "GaussRICH/RichG4HpdReflTag.h"


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
          //if( aRichPhotInfo -> VerbosePhotTagFlag() ) {
           
            //RichG4HpdReflectionFlag* aRichG4HpdReflectionFlag= RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();
            // G4int aMaxFlag=aRichG4HpdReflectionFlag->MaxNumHpdReflFlag();
            
            std::bitset<16>aCurFl((unsigned long ) (aRichPhotInfo->PhotonHpdReflectionFlag()));
             aCurFl.set(aFlagPos,1);           
             aRichPhotInfo->setPhotonHpdReflectionFlag( (G4int) (aCurFl.to_ulong()));
             //}
          
          
          
        }
        
      }
      
    }
    
  }  


}




extern void   RichG4HpdQWIncidentTag(const G4Track& aPhotonTk, const G4ThreeVector & aHpdQWPoint ){

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
           
	    aRichPhotInfo ->setHpdQWPhotIncidentPosition(aHpdQWPoint);

	  }
	}
      }
    }
  }
 

}



