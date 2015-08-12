// $Id: TrackInitFit.h,v 1.1 2009-11-14 16:03:49 kholubye Exp $
#ifndef TRACKINITFIT_H 
#define TRACKINITFIT_H 1

// Include files
// from Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiTool.h"
// from LHCbKernel
#include "Kernel/ParticleID.h"


// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateInit.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/State.h"

/** @class TrackInitFit
 *  
 *
 *  @author K Holubyev
 *  @date   2009-11-14
 */
class TrackInitFit : public GaudiTool ,
                     virtual public ITrackFitter {
public: 
  /// Standard constructor
  TrackInitFit( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~TrackInitFit( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();
  //! fit a track
  StatusCode fit( LHCb::Track& track, LHCb::ParticleID pid = LHCb::ParticleID(211)  ) ;

protected:

private:
 
  std::string m_initToolName;
  std::string m_fitToolName;
  
  ITrackStateInit* m_initTrack;
  ITrackFitter* m_fitTrack;

};
#endif // TRACKINITFIT_H
