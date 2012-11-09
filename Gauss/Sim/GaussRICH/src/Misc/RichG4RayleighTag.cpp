// $Id: RichG4RayleighTag.cpp,v 1.2 2006-11-02 10:23:29 seaso Exp $
// Include files 

#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"


// local
#include "GaussRICH/RichG4RayleighTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4RayleighTag
//
// 2003-10-14 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4int RichG4RayleighTag(const G4Track& aPhotTrack  ) {

   G4int NumRayleighTag=1;
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
	   NumRayleighTag =   aRichPhotInfo-> PhotonRayleighScatFlag();      
         }
         
         
       }
       
       
     }
     
     
   }
   
   return  NumRayleighTag;   
}

//=============================================================================
