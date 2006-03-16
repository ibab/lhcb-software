// $Id: IPatForwardTool.h,v 1.1 2006-03-16 17:56:44 cattanem Exp $
#ifndef TRACKINTERFACES_IPATFORWARDTOOL_H 
#define TRACKINTERFACES_IPATFORWARDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"

static const InterfaceID IID_IPatForwardTool ( "IPatForwardTool", 1, 0 );

/** @class IPatForwardTool IPatForwardTool.h TrackInterfaces/IPatForwardTool.h
 *  Interface to the forward pattern tool
 *
 *  @author Olivier Callot
 *  @date   2005-10-04
 */
class IPatForwardTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatForwardTool; }

  virtual void forwardTrack( const LHCb::Track* track, LHCb::Tracks* output ) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IPATFORWARDTOOL_H
