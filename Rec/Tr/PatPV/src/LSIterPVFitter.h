// $Id: LSIterPVFitter.h,v 1.1 2007-12-04 08:46:52 witekma Exp $
#ifndef LSITERPVFITTER_H
#define LSITERPVFITTER_H 1
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

class LSIterPVFitter : public GaudiTool, virtual public IPVFitter {

public:
  // Standard constructor
  LSIterPVFitter(const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  // Destructor
  ~LSIterPVFitter();
  // Initialization
  virtual StatusCode initialize();
  // Fitting 
  StatusCode fitVertex(const Gaudi::XYZPoint seedPoint, 
		       std::vector<const LHCb::Track*>& tracks,
		       LHCb::RecVertex& vtx);
private:
  double m_Chi2Cut;       // How big chi2 is allowed
  int    m_Iterations;    // Number of iterations for minimisation
  double m_extrapRCut;    // Radius after which one uses full extrapolator
  double m_maxChi2;       // Chi2 of completely wrong tracks
  int    m_minTr;         // Minimum number of tracks to make a vertex
  PVTracks m_pvTracks;
  // Extrapolators
  ITrackExtrapolator* m_linExtrapolator;   // Linear extrapolator
  ITrackExtrapolator* m_fullExtrapolator;  // Full extrapolator
  // Least square iterative PV fit
  StatusCode fit(LHCb::RecVertex& vtx,std::vector<PVTrack*>& pvTracks);
  // Add track for PV
  StatusCode addTrackForPV(const LHCb::Track* str,std::vector<PVTrack>& pvTracks);
  StatusCode collectTracks(PVTracks& pvTracks, 
                      PVVertex& pvVertex, 
                      double zseed);
  void copyTrackVertex(LHCb::RecVertex* inTrackVertex,
                       LHCb::RecVertex* outTrackVertex);
  // Prepare hessian matrix and vectorD0
  void prepareVrtcs(LHCb::RecVertex& vtx, 
                    PVTrackPtrs& pvTracks, 
                    Gaudi::SymMatrix3x3& hess,
                    ROOT::Math::SVector<double,3>& d0vec);
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
  double setChi2(LHCb::RecVertex& vtx, 
                 PVTrackPtrs& pvTracks);
  // Remove max Chi2 track
  int removeMaxChi2Track(PVTrackPtrs& pvTracks, 
                         Gaudi::SymMatrix3x3& hessian,
                         ROOT::Math::SVector<double,3>& vectorD0,
                         double chi2Cut);
};
#endif // LSITERPVFITTER_H
