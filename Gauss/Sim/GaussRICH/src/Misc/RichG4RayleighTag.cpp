// $Id: RichG4RayleighTag.cpp,v 1.1 2004-02-04 13:54:04 seaso Exp $
// Include files 

#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"


// local
#include "RichG4RayleighTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4RayleighTag
//
// 2003-10-14 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
void RichG4RayleighTag(const G4Track& aPhotTrack  ) {

   G4VUserTrackInformation* aUserTrackInfo=aPhotTrack.GetUserInformation();
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
           aRichPhotInfo->bumpPhotonRayleighScatFlag();
           
         }
         
         
       }
       
       
     }
     
     
   }
   
   
}

//=============================================================================
