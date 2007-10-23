// $Id: IPatDebugTTTool.h,v 1.1 2007-10-23 11:42:13 cattanem Exp $
#ifndef TRACKINTERFACES_IPATDEBUGTTTOOL_H 
#define TRACKINTERFACES_IPATDEBUGTTTOOL_H 1

// Include files
#include "PatKernel/PatTTHit.h"

// Forward declarations
class MsgStream;

namespace LHCb{
  class Track;
}

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPatDebugTTTool ( "IPatDebugTTTool", 1, 0 );

/** @class IPatDebugTTTool IPatDebugTTTool.h TrackInterfaces/IPatDebugTTTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class IPatDebugTTTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatDebugTTTool; }

  virtual void debugTTClusterOnTrack( const LHCb::Track* track, 
                                      const PatTTHits::const_iterator beginCoord,
                                      const PatTTHits::const_iterator endCoord   ) = 0;
  
  virtual void debugTTCluster( MsgStream& msg, const PatTTHit* hit ) = 0;


protected:

private:

};
#endif // TRACKINTERFACES_IPATDEBUGTTTOOL_H
