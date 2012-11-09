// $Id: RichG4AgelExitTag.cpp,v 1.4 2006-02-27 14:09:12 seaso Exp $
// Include files

#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"


// local
#include "RichG4AgelExitTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4AgelExitTag
//
// 2003-10-29 : Sajan EASO
//-----------------------------------------------------------------------------


void RichG4AgelExitTag(const G4Track* aPhotonTk,
                       const G4ThreeVector & aAgelExitPos ) {

  RichG4RadiatorMaterialIdValues* aRMIdValues= RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
  
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
	  // G4cout<<"AgelExit Tag "<<aRichPhotInfo -> VerbosePhotTagFlag()
          //      <<G4endl;
          G4int CurPhotProdRadiator =aRichPhotInfo-> PhotProdRadiatorNum();
          
          if( aRMIdValues ->IsRich1AerogelAnyTileRad( CurPhotProdRadiator ) ) {            
            
           if( aRichPhotInfo -> VerbosePhotTagFlag() ) {
            aRichPhotInfo -> setAerogelExitPosition(aAgelExitPos);
           }
          }
          

        }

      }

    }
  }

}
//=============================================================================
