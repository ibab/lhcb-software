// $Id: TrackTrajFitter.h,v 1.2 2008-03-31 07:07:59 mneedham Exp $
#ifndef TRACKFITTER_TRACKTRAJFITTER_H 
#define TRACKFITTER_TRACKTRAJFITTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackFitter.h"

// from TrackEvent
namespace LHCb {
    class Track;
}

class ITrajFitter;
/** @class TrackTrajFitter TrackTrajFitter.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   31-10-2006
 */


class TrackTrajFitter : public GaudiTool,
                        virtual public ITrackFitter {
public: 
  /// Standard constructor
  TrackTrajFitter( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~TrackTrajFitter( );

  StatusCode initialize();

  //! fit a track 
  StatusCode fit( LHCb::Track& track,  LHCb::ParticleID pid = LHCb::ParticleID(211)  ) ;

private:
  const ITrajFitter*      m_trajfitter;
};
#endif // TRACKFITTER_TRACKTRAJFITTER_H
