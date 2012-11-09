// $Id: $
// Include files 
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include <bitset> 



// local
#include "GaussCherenkov/CherenkovG4PmtReflTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4PmtReflTag
//
// 2011-04-19 : Sajan Easo
//-----------------------------------------------------------------------------

extern void   RichG4PmtQWIncidentTag(const G4Track& aPhotonTk, const G4ThreeVector & aPmtQWPoint ){

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
           
	    aRichPhotInfo ->setHpdQWPhotIncidentPosition(aPmtQWPoint);

	  }
	}
      }
    }
  }
 
   
}


