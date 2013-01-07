// $Id: IPatDebugUTTool.h,v 1.1 2008-12-04 09:07:35 cattanem Exp $
#ifndef PATKERNEL_IPATDEBUGUTTOOL_H 
#define PATKERNEL_IPATDEBUGUTTOOL_H 1

// Include files
#include "PatKernel/PatUTHit.h"

// Forward declarations
class MsgStream;

namespace LHCb{
  class Track;
}

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPatDebugUTTool ( "IPatDebugUTTool", 1, 0 );

/** @class IPatDebugUTTool IPatDebugUTTool.h PatKernel/IPatDebugUTTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class IPatDebugUTTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatDebugUTTool; }

  virtual void debugUTClusterOnTrack( const LHCb::Track* track, 
                                      const PatUTHits::const_iterator beginCoord,
                                      const PatUTHits::const_iterator endCoord   ) = 0;
  
  virtual void debugUTCluster( MsgStream& msg, const PatUTHit* hit ) = 0;


protected:

private:

};
#endif // PATKERNEL_IPATDEBUGUTTOOL_H
