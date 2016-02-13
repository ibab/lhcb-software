#ifndef FASTVELOFITLHCBIDS_H 
#define FASTVELOFITLHCBIDS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackFitter.h"            // Interface
#include "VeloDet/DeVelo.h"

#include "FastVeloHitManager.h"

/** @class FastVeloFitLHCbIDs FastVeloFitLHCbIDs.h
 *  Fit a track from LHCbIDs.
 *
 *  @author Olivier Callot
 *  @date   2010-11-18
 */
class FastVeloFitLHCbIDs : public extends<GaudiTool, ITrackFitter> {
public: 
  /// Standard constructor
  FastVeloFitLHCbIDs( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  ~FastVeloFitLHCbIDs( ) override; ///< Destructor

  StatusCode initialize() override; ///< initialize

  /// fit the track using the LHCbIDs on it with the FastVelo internal fit
  StatusCode fit ( LHCb::Track & track, 
                   LHCb::ParticleID pid ) const override;

private:
  DeVelo* m_velo = nullptr;
  FastVeloHitManager* m_hitManager = nullptr;
  bool   m_stateAtBeam;
  bool   m_useKalmanFit ;
  double m_kalmanPtForScattering ;
  std::vector<double>  m_kalmanScatteringNoiseParameters;
  bool   m_kalmanUseTrackMomentum ;
  bool   m_kalmanStateLastMeasurement ;
  bool   m_kalmanStateEndVelo ;
};
#endif // FASTVELOFITLHCBIDS_H
