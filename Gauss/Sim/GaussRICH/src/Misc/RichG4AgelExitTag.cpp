// $Id: RichG4AgelExitTag.cpp,v 1.2 2004-02-10 10:14:09 jonesc Exp $
// Include files

#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"


// local
#include "RichG4AgelExitTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4AgelExitTag
//
// 2003-10-29 : Sajan EASO
//-----------------------------------------------------------------------------


void RichG4AgelExitTag(const G4Track* aPhotonTk,
                       const G4ThreeVector & aAgelExitPos ) {

  G4VUserTrackInformation* aUserTrackInfo=aPhotonTk->GetUserInformation();
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

          // now add the info.
          // only if the verbose tag is activated.
          if( aRichPhotInfo -> VerbosePhotTagFlag() ) {
            aRichPhotInfo -> setAerogelExitPosition(aAgelExitPos);
          }

        }

      }

    }
  }

}
//=============================================================================
