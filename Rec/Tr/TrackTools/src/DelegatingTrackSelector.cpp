
//-----------------------------------------------------------------------------
/** @file DelegatingTrackSelector.cpp
 *
 *  Implementation file for reconstruction tool : DelegatingTrackSelector
 *
 *  CVS Log :-
 *  $Id: DelegatingTrackSelector.cpp,v 1.2 2009-07-06 15:50:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "DelegatingTrackSelector.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( DelegatingTrackSelector )

//-----------------------------------------------------------------------------

DelegatingTrackSelector::DelegatingTrackSelector( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : TrackSelectorBase ( type, name, parent ) { }

DelegatingTrackSelector::~DelegatingTrackSelector() { }

bool DelegatingTrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << aTrack.key() << " " << aTrack.type()
              << " P=" << aTrack.p() << " Pt=" << aTrack.pt()
              << endmsg;
  }

  // return
  return ( checkTrackType(aTrack) && trackSelector(aTrack)->accept(aTrack) );
}

ITrackSelector * DelegatingTrackSelector::trackSelector( const LHCb::Track& aTrack ) const
{
  ITrackSelector * selector(NULL);
  TrackSelectors::const_iterator iS = m_trSels.find(aTrack.type());
  if ( iS == m_trSels.end() )
  {
    std::ostringstream name;
    name << aTrack.type();
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Getting selector for " << name.str() << endmsg;
    if ( !name.str().empty() )
    {
      selector = m_trSels[aTrack.type()] = tool<ITrackSelector>( "TrackSelector", name.str(), this );
    }
    else
    {
      Exception( "Empty track type" );
    }
  }
  else 
  {
    selector = iS->second; 
  }
  return selector;
}
