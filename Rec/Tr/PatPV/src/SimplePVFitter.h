// $Id: SimplePVFitter.h,v 1.1 2009-12-16 11:51:51 witekma Exp $
#ifndef SIMPLEPVFITTER_H
#define SIMPLEPVFITTER_H 1
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

class SimplePVFitter : public extends<GaudiTool, IPVFitter> {

public:
  // Standard constructor
  SimplePVFitter(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  // Destructor
  ~SimplePVFitter() override;
  // Initialization
  StatusCode initialize() override;
  // Fitting
  StatusCode fitVertex(const Gaudi::XYZPoint& seedPoint,
                       const std::vector<const LHCb::Track*>& tracks,
                       LHCb::RecVertex& vtx,
                       std::vector<const LHCb::Track*>& tracks2remove) const override;
private:
  int    m_minTr = 0;         // Minimum number of tracks to make a vertex
  int    m_Iterations = 0;    // Number of iterations for minimisation
  double m_maxChi2 = 0.;       // Chi2 of completely wrong tracks
  double m_extrapRCut = 0.;    // Radius after which one uses full extrapolator
  double m_maxDeltaZ = 0.;     // Fit convergence condition
  double m_acceptTrack = 0.;   // Value of the Tukey's weight to accept a track
  double m_trackMaxChi2Remove; // Max chi2 tracks to be removed from next PV search
  // Extrapolators
  ITrackExtrapolator* m_linExtrapolator = nullptr;   // Linear extrapolator
  ITrackExtrapolator* m_fullExtrapolator = nullptr;  // Full extrapolator
  // Least square iterative PV fit
  StatusCode fit(LHCb::RecVertex& vtx,std::vector<PVTrack*>& pvTracks, std::vector<const LHCb::Track*>& tracks2remove) const;
  // Add track for PV
  StatusCode addTrackForPV(const LHCb::Track* str,std::vector<PVTrack>& pvTracks,
                           double zseed) const;

  void initVertex(PVTracks& pvTracks,
                  PVVertex& pvVertex,
                  const Gaudi::XYZPoint seedPoint) const;

  // Prepare hessian matrix and vectorD0
  void prepareVertex(LHCb::RecVertex& vtx,
                    PVTrackPtrs& pvTracks,
                    Gaudi::SymMatrix3x3& hess,
                    ROOT::Math::SVector<double,3>& d0vec,
                    int iter) const;
  // Extrapolation
  StatusCode trackExtrapolate(PVTrack* pvTrack,
                              const LHCb::RecVertex& vtx) const;
  // Add track to fit
  void addTrack(PVTrack* pTrack,
                Gaudi::SymMatrix3x3& hessian,
                ROOT::Math::SVector<double,3>& vectorD0) const;
  // Remove track from fit
  void removeTrack(PVTrack* pTrack,
                Gaudi::SymMatrix3x3& hessian,
                ROOT::Math::SVector<double,3>& vectorD0) const;
  // Add subtrack
  void addsubTrack(PVTrack* pTrack,
                   Gaudi::SymMatrix3x3& hessian,
                   ROOT::Math::SVector<double,3>& vectorD0,
                   double invs) const;
  // Update matrices and vectors
  StatusCode outVertex(LHCb::RecVertex& vtx,
                       PVTrackPtrs& pvTracks,
                       Gaudi::SymMatrix3x3& hess,
                       ROOT::Math::SVector<double,3>& d0vec) const;
  // Set current chi2
  void setChi2(LHCb::RecVertex& vtx,
               PVTrackPtrs& pvTracks) const;

  // Get Tukey's weight
  double getTukeyWeight(double trchi2, int iter) const;
};
#endif // SIMPLEPVFITTER_H
