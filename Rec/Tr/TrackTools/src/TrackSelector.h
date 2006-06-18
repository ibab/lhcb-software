// $Id: TrackSelector.h,v 1.3 2006-06-18 14:15:38 jonrob Exp $
#ifndef _TrackSelector_H
#define _TrackSelector_H

/** @class TrackSelector TrackSelector TrackTools/TrackSelector.h
 *
 *  General track Selection tool
 *
 *  Cuts can be applied on p, hits, chi^2, pt, and track type.
 * 
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author C. Jones  Christopher.Rob.Jones@cern.ch
 *
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"

#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"

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

  /// the method
  virtual bool accept(const LHCb::Track& aTrack) const;

private:

  /// Returns weighted hit sum for given track
  double weightedMeasurementSum( const LHCb::Track & aTrack ) const;

private:

  double m_vWeight;
  double m_oWeight;
  double m_iWeight;

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

#endif
