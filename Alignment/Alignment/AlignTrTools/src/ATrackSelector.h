#ifndef Alignment_ATrackSelector_H
#define Alignment_ATrackSelector_H
//-----------------------------------------------------------------------------
/** @file ATrackSelector.h
 *
 *  Header file for Tstation alignment : ATrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.h,v 1.1 2006-09-29 10:48:31 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   31/09/2006
 */
//-----------------------------------------------------------------------------


#include "GaudiAlg/GaudiTool.h"
#include "AlignmentInterfaces/IATrackSelectorTool.h"
#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"

#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class ATrackSelector : public GaudiTool, virtual public IATrackSelectorTool {

public:

  /// constructer
  ATrackSelector( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~ATrackSelector(); ///< Destructor

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */

  bool accept ( const LHCb::Track& aTrack ) const;
  int traversesIT( const LHCb::Track & aTrack, int& nOThits, int& nIThits ) const;
//  int traversesIT( int& nOThits, int& nIThits ) const;

private:


 private:

  

  double m_minChi2Cut; ///< Min chi^2 cut
  double m_minPCut;    ///< Min p cut
  double m_minPtCut;   ///< Min pt cut
  double m_minHitCut;  ///< Min hit cut

  double m_maxChi2Cut; ///< Max chi^2 cut
  double m_maxPCut;    ///< Max p cut
  double m_maxPtCut;   ///< Max pt cut
  double m_maxHitCut;  ///< Max hit cut

  /// Track types to accept
  typedef std::vector<std::string> TrackTypes;
  TrackTypes m_trTypes; ///< List of track types to select

  /// Mapping type linking track types to selection boolean
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
  SelTypes m_selTypes;  ///< Mapping linking track types to selection boolean

};

#endif // TRACKTOOLS_JBSelector_H
