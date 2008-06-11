// $Id: LSAdaptPV3DFitter.h,v 1.1 2008-06-11 19:28:24 witekma Exp $
#ifndef LSADAPTPVFITTER_H
#define LSADAPTPVFITTER_H 1
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// Interfaces
#include "IPVFitter.h"
#include "PVUtils.h"
// Track info
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
// DeVelo
#include "VeloDet/DeVelo.h"

class LSAdaptPV3DFitter : public GaudiTool, virtual public IPVFitter {

public:
  // Standard constructor
  LSAdaptPV3DFitter(const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  // Destructor
  ~LSAdaptPV3DFitter();
  // Initialization
  virtual StatusCode initialize();
  // Fitting 
  StatusCode fitVertex(const Gaudi::XYZPoint seedPoint, 
		       std::vector<const LHCb::Track*>& tracks,
		       LHCb::RecVertex& vtx);
private:
  int    m_minTr;         // Minimum number of tracks to make a vertex
  int    m_Iterations;    // Number of iterations for minimisation
  double m_maxIP2PV;      // Maximum IP of a track to accept track
  double m_maxDeltaZ;     // Fit convergence condition
  double m_minTrackWeight;// Minimum Tukey's weight to accept a track
  double m_detectorResolutionCoeff; ///< detector resolution
  double m_multipleScatteringCoeff; ///< multiple scattering resolution
  DeVelo* m_velo;
  PVTracks m_pvTracks;

  // Add track for PV
  void addTrackForPV(const LHCb::Track* str, PVTracks& pvTracks,
                           Gaudi::XYZPoint seed);
  
  double err2d0(const LHCb::Track* track);
  void computeErrorParameters( const LHCb::Track* track, double z,
                                      double & minZDistToVELOCluster,
                                      int & nbStaBeforeLastVELOCluster,
			       double & distFirstLastVELOCluster );
  Gaudi::XYZVector impactParameterVector(const Gaudi::XYZPoint& vertex,
                                        const Gaudi::XYZPoint& point,
					 const Gaudi::XYZVector& direction);
  // Get Tukey's weight
  double getTukeyWeight(double trchi2, int iter);
};
#endif // LSADAPTPVFITTER_H
