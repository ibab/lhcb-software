
//-----------------------------------------------------------------------------
/** @file DelegatingTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : DelegatingTrackSelector
 *
 *  CVS Log :-
 *  $Id: DelegatingTrackSelector.h,v 1.1 2009-07-01 18:28:07 jonrob Exp $
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

// Interface
#include "TrackInterfaces/ITrackSelector.h"

// Gaudi
#include "GaudiKernel/HashMap.h"
#include "GaudiAlg/GaudiTool.h"

// Event model
#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"

class DelegatingTrackSelector : public GaudiTool,
                                virtual public ITrackSelector
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

  /// Access on demand the track selector for each track type
  ITrackSelector * trackSelector( const LHCb::Track& aTrack ) const;

private: // data

  /// Track types to accept
  typedef std::vector<std::string> TrackTypes;
  TrackTypes m_trTypes; ///< List of track types to select

  /// Mapping type linking track types to selection boolean
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
  SelTypes m_selTypes;  ///< Mapping linking track types to selection boolean

  /// Track selector for each track type
  mutable TrackSelectors m_trSels;

};

#endif // TRACKTOOLS_DelegatingTrackSelector_H
