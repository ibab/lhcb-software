// $Id: TrackSelector.h,v 1.2 2006-06-13 13:17:56 jonrob Exp $
#ifndef _TrackSelector_H
#define _TrackSelector_H

/** @class TrackSelector TrackSelector TrackTools/TrackSelector.h
 *
 *  General track Selection tool
 *  you can cut on p, hits, chi^2, pt (what more do you want)
 *  default no cut
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"

#include "Event/Track.h"

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

  double weightedMeasurementSum(const LHCb::Track& aTrack) const;

  double m_vWeight;
  double m_oWeight;
  double m_iWeight;

  double m_chi2Cut;
  double m_pCut;
  double m_ptCut;
  double m_hitCut;

  /// Track types to accept
  typedef std::vector<std::string> TrackTypes;
  TrackTypes m_trTypes; ///< List of track types to select

  typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
  SelTypes m_selTypes;

};

#endif
