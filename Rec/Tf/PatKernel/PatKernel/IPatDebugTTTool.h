// $Id: IPatDebugTTTool.h,v 1.1 2008-12-04 09:07:35 cattanem Exp $
#ifndef PATKERNEL_IPATDEBUGTTTOOL_H 
#define PATKERNEL_IPATDEBUGTTTOOL_H 1

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

/** @class IPatDebugTTTool IPatDebugTTTool.h PatKernel/IPatDebugTTTool.h
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
#endif // PATKERNEL_IPATDEBUGTTTOOL_H
