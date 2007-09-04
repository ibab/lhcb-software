#ifndef VELOALIGNMENT_TRACKSTORE_H 
#define VELOALIGNMENT_TRACKSTORE_H 1

// Include files
#include "ITrackStore.h"            // Interface
#include "VeloTrack.h"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/LHCbID.h"
#include "GaudiKernel/Point3DTypes.h"

// TrFitEvent
#include "Event/Track.h"
#include "Event/VeloCluster.h"

//from DetDesc
#include "VeloDet/DeVelo.h"


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
  virtual StatusCode TransformTrack(LHCb::Track* ftrack, VeloTrack& atrack, double Map_VELO[]);

protected:

private:
  

  StatusCode AnalyzeOverlap(int hits[]);

  DeVelo* my_velo;
  LHCb::VeloClusters* m_veloClusters;

  int    m_NonzerCut;      // Minimum number of valid coordinates on a track 
  int    m_NonzerOver;     // Minimum number of valid coordinates on the overlap side
  bool   m_OverlapCut;     // Do we take the overlap or not ?
  double m_xOverlapCut;    // X max value for overlap track
  bool   m_MissedHits;     // Do we take the tracks with missed hits ?
  bool   m_ACDC;           // ACDC (aka real) or DC06 geometry ?

  const DeVeloRType* rDet;
  const DeVeloPhiType* phiDet;
  
};
#endif // VELOALIGNMENT_TRACKSTORE_H
