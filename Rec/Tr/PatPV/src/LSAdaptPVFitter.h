// $Id: LSAdaptPVFitter.h,v 1.3 2007-12-04 21:48:36 witekma Exp $
#ifndef LSADAPTPVFITTER_H
#define LSADAPTPVFITTER_H 1
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "IPVFitter.h"
#include "PVUtils.h"
// Track info
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

class LSAdaptPVFitter : public GaudiTool, virtual public IPVFitter {

public:
  // Standard constructor
  LSAdaptPVFitter(const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  // Destructor
  ~LSAdaptPVFitter();
  // Initialization
  virtual StatusCode initialize();
  // Fitting 
  StatusCode fitVertex(const Gaudi::XYZPoint seedPoint, 
		       std::vector<const LHCb::Track*>& tracks,
		       LHCb::RecVertex& vtx);
private:
  int    m_minTr;         // Minimum number of tracks to make a vertex
  int    m_Iterations;    // Number of iterations for minimisation
  double m_maxChi2;       // Chi2 of completely wrong tracks
  double m_extrapRCut;    // Radius after which one uses full extrapolator
  double m_maxDeltaZ;     // Fit convergence condition
  double m_acceptTrack;   // Value of the Tukey's weight to accept a track
  PVTracks m_pvTracks;
  // Extrapolators
  ITrackExtrapolator* m_linExtrapolator;   // Linear extrapolator
  ITrackExtrapolator* m_fullExtrapolator;  // Full extrapolator
  // Least square iterative PV fit
  StatusCode fit(LHCb::RecVertex& vtx,std::vector<PVTrack*>& pvTracks);
  // Add track for PV
  StatusCode addTrackForPV(const LHCb::Track* str,std::vector<PVTrack>& pvTracks,
                           double zseed);
  
  void initVertex(PVTracks& pvTracks, 
                  PVVertex& pvVertex, 
                  const Gaudi::XYZPoint seedPoint);
  
  // Prepare hessian matrix and vectorD0
  void prepareVertex(LHCb::RecVertex& vtx, 
                    PVTrackPtrs& pvTracks, 
                    Gaudi::SymMatrix3x3& hess,
                    ROOT::Math::SVector<double,3>& d0vec,
                    int iter);
  // Extrapolation
  StatusCode trackExtrapolate(PVTrack* pvTrack, 
                              const LHCb::RecVertex& vtx);
  // Add track to fit
  void addTrack(PVTrack* pTrack, 
                Gaudi::SymMatrix3x3& hessian,
                ROOT::Math::SVector<double,3>& vectorD0);
  // Remove track from fit
  void removeTrack(PVTrack* pTrack, 
                Gaudi::SymMatrix3x3& hessian,
                ROOT::Math::SVector<double,3>& vectorD0);
  // Add subtrack
  void addsubTrack(PVTrack* pTrack, 
                   Gaudi::SymMatrix3x3& hessian,
                   ROOT::Math::SVector<double,3>& vectorD0,
                   double invs);
  // Update matrices and vectors
  StatusCode outVertex(LHCb::RecVertex& vtx,
                       PVTrackPtrs& pvTracks,
                       Gaudi::SymMatrix3x3& hess,
                       ROOT::Math::SVector<double,3>& d0vec);
  // Set current chi2
  void setChi2(LHCb::RecVertex& vtx, 
               PVTrackPtrs& pvTracks);
  
  // Get Tukey's weight
  double getTukeyWeight(double& trchi2,int& iter);
};
#endif // LSADAPTPVFITTER_H
