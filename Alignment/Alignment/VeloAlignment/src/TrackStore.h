#ifndef VELOALIGNMENT_TRACKSTORE_H 
#define VELOALIGNMENT_TRACKSTORE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"

#include "ITrackStore.h"            // Interface

#include "Event/AlignTrack.h"

// TrFitEvent
#include "Event/Track.h"
#include "Event/MCParticle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

//from DetDesc
#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"

#include "DetDesc/ILVolume.h"

/** @class TrackStore TrackStore.h VeloAlignment/TrackStore.h
 * 
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */

//--------------------------------------------------------------
// This tool is used to create a track store for Velo Alignment
//--------------------------------------------------------------


class TrackStore : public GaudiTool, virtual public ITrackStore {
public: 
  /// Standard constructor
  TrackStore( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~TrackStore( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize();
  virtual StatusCode TransformTrack(Track* ftrack, AlignTrack* atrack, double Map_VELO[], DeVelo* my_velo, 
				    double residual_r[], double residual_phi[]);

protected:

private:

  StatusCode GetTrackSlope(AlignTrack* atrack);

  DeVelo* my_velo;
  VeloClusters* m_veloClusters;

  double m_momentum_cut;   // Momentum cut
  bool   m_longCut;        // Do we take long tracks or not ?
  double m_RDiffCut;       // Difference bet. R estimated in phi and R sensor, for the same half station
  double m_RMin;           // Minimal Radius for accepting hit
  int    m_NonzerCut;      // Minimum number of valid coordinates on a track ?
  bool   m_OverlapCut;     // Do we take the overlap or not ?
  bool   m_MissedHits;     // Do we take the tracks with missed hits ?
  int    m_SelectOption;

  ITrackExtrapolator*   m_extrapolator;
  std::string m_extrapolatorName;

};
#endif // VELOALIGNMENT_TRACKSTORE_H
