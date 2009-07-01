
//-----------------------------------------------------------------------------
/** @file TrackSelector.h
 *
 *  Header file for RICH reconstruction tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: TrackSelector.h,v 1.10 2009-07-01 18:28:07 jonrob Exp $
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

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"

#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class TrackSelector : public GaudiTool,
                      virtual public ITrackSelector
{

public:

  /// constructer
  TrackSelector( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~TrackSelector();

  /// Tool initialization
  virtual StatusCode initialize();

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

  double m_minLikCut;  ///< Minimum track likelihood
  double m_maxLikCut;  ///< Maximum track likelihood

  bool m_acceptClones;  ///< Flag to turn on/off reject of clones
  double m_minCloneCut; ///< Minimum Clone distance cut
  double m_maxCloneCut; ///< Minimum Clone distance cut

  double m_minGhostProb; ///< minimum ghost probability cut
  double m_maxGhostProb; ///< maximum ghost probability cut

  int m_minNVeloRHits ;    ///< Minimum number of velo R hits
  int m_minNVeloPhiHits ;  ///< Minimum number of velo phi hits
  int m_minNOTHits ;       ///< Minimum number of OT hits

  /// Track types to accept
  typedef std::vector<std::string> TrackTypes;
  TrackTypes m_trTypes; ///< List of track types to select

  /// Mapping type linking track types to selection boolean
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
  SelTypes m_selTypes;  ///< Mapping linking track types to selection boolean

};

#endif // TRACKTOOLS_TrackSelector_H
