// $Id: RichRecBase_load.cpp,v 1.1 2003-04-16 12:05:02 cattanem Exp $
// Include files

#include "Event/EventFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in RichRecEvent
//
// 17/05/2002 : Chris Jones
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/RichRecTrack.h"
_ImplementContainedObjectFactory ( RichRecTrack )
_ImplementDataObjectFactory      ( RichRecTracks )

  /// ====================================================================
#include "Event/RichRecPixel.h"
_ImplementContainedObjectFactory ( RichRecPixel )
_ImplementDataObjectFactory      ( RichRecPixels )

  /// ====================================================================
#include "Event/RichRecPhoton.h"
_ImplementContainedObjectFactory ( RichRecPhoton )
_ImplementDataObjectFactory      ( RichRecPhotons )

  /// ====================================================================
#include "Event/RichRecSegment.h"
_ImplementContainedObjectFactory ( RichRecSegment )
_ImplementDataObjectFactory      ( RichRecSegments )

  /// ====================================================================
#include "Event/RichGlobalPIDTrack.h"
_ImplementContainedObjectFactory ( RichGlobalPIDTrack )
_ImplementDataObjectFactory      ( RichGlobalPIDTracks )

/// ====================================================================
#include "Event/RichGlobalPID.h"
_ImplementContainedObjectFactory ( RichGlobalPID )
_ImplementDataObjectFactory      ( RichGlobalPIDs )

/// ====================================================================
#include "Event/RichLocalPID.h"
_ImplementContainedObjectFactory ( RichLocalPID )
_ImplementDataObjectFactory      ( RichLocalPIDs )

void RichRecBase_load() {

  // Declaration of contained object factories

  DLL_DECL_OBJECTFACTORY( RichRecTrack );
  DLL_DECL_OBJECTFACTORY( RichRecTracks );

  DLL_DECL_OBJECTFACTORY( RichRecPixel );
  DLL_DECL_OBJECTFACTORY( RichRecPixels );

  DLL_DECL_OBJECTFACTORY( RichRecPhoton );
  DLL_DECL_OBJECTFACTORY( RichRecPhotons );

  DLL_DECL_OBJECTFACTORY( RichRecSegment );
  DLL_DECL_OBJECTFACTORY( RichRecSegments );

  DLL_DECL_OBJECTFACTORY( RichGlobalPIDTrack );
  DLL_DECL_OBJECTFACTORY( RichGlobalPIDTracks );

  DLL_DECL_OBJECTFACTORY( RichGlobalPID );
  DLL_DECL_OBJECTFACTORY( RichGlobalPIDs );
  
  DLL_DECL_OBJECTFACTORY( RichLocalPID );
  DLL_DECL_OBJECTFACTORY( RichLocalPIDs );

}

extern "C" void RichRecBase_loadRef()  {
  RichRecBase_load();
}
