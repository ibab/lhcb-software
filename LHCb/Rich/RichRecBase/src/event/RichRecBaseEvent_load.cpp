// 
//-----------------------------------------------------------------------------
/** @file RichRecBaseEvent_load.cpp
 *
 *  Implementation file for classes in RichRecEvent
 *
 *  CVS Log :-
 *  $Id: RichRecBaseEvent_load.cpp,v 1.3 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ContainerFactoryDefs.h"

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
