// $Id: RichG4MirrorReflPointTag.cpp,v 1.1 2004-06-03 12:42:34 seaso Exp $
// Include files 


#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"

// local
#include "GaussRICH/RichG4MirrorReflPointTag.h"

//------------------------------------------------------------------------
// Implementation file for class : RichG4MirrorReflPointTag
//
// 2004-05-27 : Sajan EASO
//-------------------------------------------------------------------------
extern void RichG4MirrorReflPointTag(const G4Track* aPhotonTk,
                                     const G4ThreeVector & aReflPoint,
                                     int aRichDetNum, int aMirrorTypeNum,
                                     int aMirrorCopyNum) 
{
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
          //G4cout<<"MirrorREflPointTag VerboseFlag "<<aRichPhotInfo -> VerbosePhotTagFlag()
	  //	<<G4endl;
          if( aRichPhotInfo -> VerbosePhotTagFlag() ) {
	    G4int aMirrorDetCopyNum= aMirrorTypeNum *1000+ aRichDetNum*100 + aMirrorCopyNum;
            if(aMirrorTypeNum == 0 ) {
              aRichPhotInfo -> setMirror1ReflPosition(aReflPoint);
              aRichPhotInfo ->  setMirror1DetCopyNum(aMirrorDetCopyNum);
            }else if (aMirrorTypeNum == 1 ) {
              aRichPhotInfo -> setMirror2ReflPosition(aReflPoint);
	      aRichPhotInfo -> setMirror2DetCopyNum(aMirrorDetCopyNum);
            }

          }

        }

      }

    }
  }


}


