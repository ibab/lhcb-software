// $Id: LSAdaptPV3DFitter.h,v 1.4 2009-12-22 12:38:26 graven Exp $
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
  double m_TrackErrorScaleFactor;
  double  m_x0MS;         // X0 (tunable) of MS to add for extrapolation of
                          // track parameters to PV
  double m_scatCons;      // calculated from m_x0MS
  PVTracks m_pvTracks;

  // Add track for PV
  void addTrackForPV(const LHCb::Track* str, PVTracks& pvTracks,
                           Gaudi::XYZPoint seed);

  double err2d0(const LHCb::Track* track);
  Gaudi::XYZVector impactParameterVector(const Gaudi::XYZPoint& vertex,
                                        const Gaudi::XYZPoint& point,
					 const Gaudi::XYZVector& direction);
  // Get Tukey's weight
  double getTukeyWeight(double trchi2, int iter);
};
#endif // LSADAPTPVFITTER_H
