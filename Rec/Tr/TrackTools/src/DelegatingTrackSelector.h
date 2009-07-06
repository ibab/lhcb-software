
//-----------------------------------------------------------------------------
/** @file DelegatingTrackSelector.h
 *
 *  Header file for reconstruction tool : DelegatingTrackSelector
 *
 *  CVS Log :-
 *  $Id: DelegatingTrackSelector.h,v 1.2 2009-07-06 15:50:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/06/2009
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_DelegatingTrackSelector_H
#define TRACKTOOLS_DelegatingTrackSelector_H

//-----------------------------------------------------------------------------
/** @class DelegatingTrackSelector DelegatingTrackSelector.h
 *
 *  General track Selection tool
 *
 *  Cuts can be applied on various quantities like p, hits, chi^2, pt, and track type.
 *
 *  @author C. Jones  Christopher.Rob.Jones@cern.ch
 *
 *  @date   30/06/2009
 */
//-----------------------------------------------------------------------------

// STL
#include <sstream>

// base class
#include "TrackSelectorBase.h"

class DelegatingTrackSelector : public TrackSelectorBase
{

private: // definitions

  /// Track selector for each track type
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, ITrackSelector* > TrackSelectors;

public:

  /// constructer
  DelegatingTrackSelector( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~DelegatingTrackSelector();

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

  /// Access on demand the track selector for each track type
  ITrackSelector * trackSelector( const LHCb::Track& aTrack ) const;

private: // data

  /// Track selector for each track type
  mutable TrackSelectors m_trSels;

};

#endif // TRACKTOOLS_DelegatingTrackSelector_H
