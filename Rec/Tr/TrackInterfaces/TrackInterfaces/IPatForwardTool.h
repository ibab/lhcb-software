#ifndef TRACKINTERFACES_IPATFORWARDTOOL_H 
#define TRACKINTERFACES_IPATFORWARDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

/** @class IPatForwardTool IPatForwardTool.h TrackInterfaces/IPatForwardTool.h
 *  Interface to the forward pattern tool
 *
 *  @author Olivier Callot
 *  @date   2005-10-04
 */
class IPatForwardTool : public extend_interfaces<IAlgTool> {
public: 

  DeclareInterfaceID( IPatForwardTool, 2, 0 );

  virtual void forwardTrack( const LHCb::Track& track, LHCb::Tracks& output ) const = 0;
  virtual void setNNSwitch( bool nnSwitch) = 0;

};
#endif // TRACKINTERFACES_IPATFORWARDTOOL_H
