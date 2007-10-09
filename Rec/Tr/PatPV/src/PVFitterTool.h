// $Id: PVFitterTool.h,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
#ifndef PVFITTERTOOL_H 
#define PVFITTERTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interfaces
#include "IPVFitterTool.h"
#include "PVUtils.h"

// Track info
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

class PVFitterTool : public GaudiTool, virtual public IPVFitterTool {

public:
  // Standard constructor
  PVFitterTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  // Destructor
  ~PVFitterTool();

  virtual StatusCode initialize();

  // Fitting 
  StatusCode fit(LHCb::RecVertex& vtx, PVTrackPtrs& tracks) ;
  StatusCode fit(LHCb::RecVertex& vtx, PVTracks& tracks) ;
  StatusCode fit(LHCb::RecVertex& vtx, LHCb::Tracks& tracks) ;
  StatusCode fit(LHCb::RecVertex& vtx, std::vector<LHCb::Track*>& tracks) ;

  // Refit
  StatusCode addTrackForPV(LHCb::Track* str, PVTracks& pvTracks);

private:
  double m_Chi2Cut;       // How big chi2 is allowed
  int    m_Iterations;    // Number of iterations for minimisation
  double m_extrapRCut;    // Radius after which one uses full extrapolator
  int    m_minTr;         // Minimum number of tracks to make a vertex
  bool   m_deWeight;      // If true, deweight the chi2

  ITrackExtrapolator* m_pLinearExtrap;   // Linear extrapolator
  ITrackExtrapolator* m_pFullExtrap;     // Full extrapolator
  ITrackExtrapolator* m_pSpecialExtrap;     // Full extrapolator
  
  // Add (substrack) track to fit
  void addsubTrack(PVTrack* pTrack, 
                   Gaudi::SymMatrix3x3& hessian,
                   ROOT::Math::SVector<double,3>& vectorD0,
                   double weight); ///< add/substrack track to fit

  // Add track to fit
  void addTrack(PVTrack* pTrack, 
                Gaudi::SymMatrix3x3& hessian,
                ROOT::Math::SVector<double,3>& vectorD0);

  // Substract track from fit
  void removeTrack(PVTrack* pTrack, 
                   Gaudi::SymMatrix3x3& hessian,
                   ROOT::Math::SVector<double,3>& vectorD0);

  // Update matrices and vectors
  StatusCode newVertex(LHCb::RecVertex& vtx,
                       PVTrackPtrs& pvTracks,
                       Gaudi::SymMatrix3x3& hessian,
                       ROOT::Math::SVector<double,3>& vectorD0);

  // Prepare hessian matrix and vectorD0
  void prepareVertices(LHCb::RecVertex& vtx, 
                       PVTrackPtrs& pvTracks, 
                       Gaudi::SymMatrix3x3& hessian,
                       ROOT::Math::SVector<double,3>& vectorD0);

  // Initial track cleaning. Cut in chi2 
  int chi2Exclusion(PVTrackPtrs& pvTracks, 
                    Gaudi::SymMatrix3x3& hessian,
                    ROOT::Math::SVector<double,3>& vectorD0,
                    double chi2Cut);

  // Remove max Chi2 track
  int removeMaxChi2Track(PVTrackPtrs& pvTracks, 
                         Gaudi::SymMatrix3x3& hessian,
                         ROOT::Math::SVector<double,3>& vectorD0,
                         double chi2Cut);

  // Extrapolation
  StatusCode transport(PVTrack* track, 
                       const LHCb::RecVertex& vrtx);


  // Set current chi2
  double setChi2(LHCb::RecVertex& vtx, 
                 PVTrackPtrs& pvtracks);

};
#endif // PVFITTERTOOL_H
