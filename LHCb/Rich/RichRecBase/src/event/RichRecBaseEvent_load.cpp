// $Id: RichRecBaseEvent_load.cpp,v 1.1 2003-06-30 15:11:58 jonrob Exp $
// Include files

#include "Event/EventFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in RichRecEvent
//
// 17/05/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Common objects
#include "Event/RichRecTrack.h"
_ImplementContainedObjectFactory ( RichRecTrack )
_ImplementDataObjectFactory      ( RichRecTracks )
#include "Event/RichRecPixel.h"
_ImplementContainedObjectFactory ( RichRecPixel )
_ImplementDataObjectFactory      ( RichRecPixels )
#include "Event/RichRecPhoton.h"
_ImplementContainedObjectFactory ( RichRecPhoton )
_ImplementDataObjectFactory      ( RichRecPhotons )
#include "Event/RichRecSegment.h"
_ImplementContainedObjectFactory ( RichRecSegment )
_ImplementDataObjectFactory      ( RichRecSegments )

void RichRecBaseEvent_load() {

  // Declaration of contained object factories

  // Common objects
  DLL_DECL_OBJECTFACTORY( RichRecTrack );
  DLL_DECL_OBJECTFACTORY( RichRecTracks );
  DLL_DECL_OBJECTFACTORY( RichRecPixel );
  DLL_DECL_OBJECTFACTORY( RichRecPixels );
  DLL_DECL_OBJECTFACTORY( RichRecPhoton );
  DLL_DECL_OBJECTFACTORY( RichRecPhotons );
  DLL_DECL_OBJECTFACTORY( RichRecSegment );
  DLL_DECL_OBJECTFACTORY( RichRecSegments );

}

extern "C" void RichRecBaseEvent_loadRef()  {
  RichRecBaseEvent_load();
}
