#ifndef TRACKINTERFACES_ITRACKFITTER_H 
#define TRACKINTERFACES_ITRACKFITTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCbKernel
#include "Kernel/ParticleID.h"

// Forward declarations
namespace LHCb {
 class Track;
}

/** @class ITrackFitter ITrackFitter.h TrackInterfaces/ITrackFitter.h
 *  
 *  Interface for a track fitting tool.
 *
 *  @author Jose A. Hernando, Eduardo Rodrigues
 *  @date   2005-05-25
 *
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */
class ITrackFitter : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID( ITrackFitter, 3, 0 );

  //! fit a track 
  virtual StatusCode fit(LHCb::Track& track,
                         LHCb::ParticleID pid = LHCb::ParticleID(211) ) const = 0;

};
#endif // TRACKINTERFACES_ITRACKFITTER_H
