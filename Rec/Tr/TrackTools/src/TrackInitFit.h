#ifndef TRACKINITFIT_H 
#define TRACKINITFIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateInit.h"

/** @class TrackInitFit
 *  
 *
 *  @author K Holubyev
 *  @date   2009-11-14
 */
class TrackInitFit : public extends<GaudiTool, ITrackFitter> {
public: 
  /// Standard constructor
  TrackInitFit( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  ~TrackInitFit( ) override; ///< Destructor

  StatusCode initialize() override;
  //! fit a track
  StatusCode fit( LHCb::Track& track, LHCb::ParticleID pid) const override;


private:
 
  std::string m_initToolName;
  std::string m_fitToolName;
  
  ITrackStateInit* m_initTrack = nullptr;
  ITrackFitter* m_fitTrack = nullptr;

};
#endif // TRACKINITFIT_H
