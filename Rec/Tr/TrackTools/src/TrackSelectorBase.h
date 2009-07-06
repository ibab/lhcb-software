
//-----------------------------------------------------------------------------
/** @file TrackSelectorBase.h
 *
 *  Header file for reconstruction tool : TrackSelectorBase
 *
 *  CVS Log :-
 *  $Id: TrackSelectorBase.h,v 1.1 2009-07-06 15:50:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/06/2009
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_TrackSelectorBase_H
#define TRACKTOOLS_TrackSelectorBase_H

//-----------------------------------------------------------------------------
/** @class TrackSelectorBase TrackSelectorBase.h
 *
 *  Common base class for track selectors
 *
 *  @author C. Jones  Christopher.Rob.Jones@cern.ch
 *
 *  @date   30/06/2009
 */
//-----------------------------------------------------------------------------

// Interface
#include "TrackInterfaces/ITrackSelector.h"

// Gaudi
#include "GaudiKernel/HashMap.h"
#include "GaudiAlg/GaudiTool.h"

// Event model
#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"

class TrackSelectorBase : public GaudiTool,
                          virtual public ITrackSelector
{

public:

  /// constructer
  TrackSelectorBase( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  virtual ~TrackSelectorBase();

  /// Tool initialization
  virtual StatusCode initialize();

public:

  // Returns if the given track is selected or not
  virtual bool accept ( const LHCb::Track & aTrack ) const = 0;

protected:

  // Check track type
  inline bool checkTrackType( const LHCb::Track & aTrack ) const
  {
    bool OK = true;
    if ( !m_selTypes[aTrack.type()] )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Track type " << aTrack.type() << " is rejected" << endreq;
      OK = false;
    }
    return OK;
  }

private: // data

  /// Track types to accept
  typedef std::vector<std::string> TrackTypes;
  TrackTypes m_trTypes; ///< List of track types to select

  /// Mapping type linking track types to selection boolean
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
  SelTypes m_selTypes;  ///< Mapping linking track types to selection boolean

};

#endif // TRACKTOOLS_TrackSelectorBase_H
