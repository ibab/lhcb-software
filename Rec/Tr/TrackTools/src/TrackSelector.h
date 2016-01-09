
//-----------------------------------------------------------------------------
/** @file TrackSelector.h
 *
 *  Header file for reconstruction tool : TrackSelector
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_TrackSelector_H
#define TRACKTOOLS_TrackSelector_H

//-----------------------------------------------------------------------------
/** @class TrackSelector TrackSelector.h
 *
 *  General track Selection tool
 *
 *  Cuts can be applied on various quantities like p, hits, chi^2, pt, and track type.
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author C. Jones  Christopher.Rob.Jones@cern.ch
 *
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

// STL
#include <string>
#include <sstream>

// base class
#include "TrackSelectorBase.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class TrackSelector : public TrackSelectorBase
{

public:

  /// constructer
  TrackSelector( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~TrackSelector();

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track & aTrack ) const;

private:

  /// Returns weighted hit sum for given track
  double weightedMeasurementSum( const LHCb::Track & aTrack ) const;

  /// Implements the number of hits cuts
  bool checkNHits( const LHCb::Track & aTrack ) const;

private:

  double m_vWeight;
  double m_oWeight;
  double m_iWeight;

  double m_minChi2Cut; ///< Min chi^2 cut
  double m_maxChi2Cut; ///< Max chi^2 cut

  double m_minPCut;    ///< Min p cut
  double m_maxPCut;    ///< Max p cut

  double m_minPtCut;   ///< Min pt cut
  double m_maxPtCut;   ///< Max pt cut

  double m_minHitCut;  ///< Min hit cut
  double m_maxHitCut;  ///< Max hit cut

  int m_minNDoF ;      ///< Minimum number of dofs
  int m_maxNDoF ;      ///< Minimum number of dofs

  double m_minEtaCut;  ///< Minimum track eta cut
  double m_maxEtaCut;  ///< maximum track eta cut

  double m_minPhiCut;  ///< Minimum track phi cut
  double m_maxPhiCut;  ///< maximum track phi cut

  double m_minLikCut;  ///< Minimum track likelihood
  double m_maxLikCut;  ///< Maximum track likelihood

  bool m_acceptClones;  ///< Flag to turn on/off reject of clones
  double m_minCloneCut; ///< Minimum Clone distance cut
  double m_maxCloneCut; ///< Minimum Clone distance cut

  double m_minGhostProb; ///< minimum ghost probability cut
  double m_maxGhostProb; ///< maximum ghost probability cut

  int m_minNVeloRHits ;    ///< Minimum number of velo R hits
  int m_minNVeloPhiHits ;  ///< Minimum number of velo phi hits
  int m_maxNVeloHoles ;    ///< Maximum number of missing velo hits 
  int m_minNOTHits ;       ///< Minimum number of OT hits
  int m_minNTTHits ;       ///< Minimum number of TT hits
  int m_maxNTHoles ;    ///< Maximum number of missing T layers

  double m_maxChi2Velo ;
  double m_maxChi2Upstream ;
  double m_maxChi2Downstream ;
  double m_maxChi2Match ;
};

#endif // TRACKTOOLS_TrackSelector_H
