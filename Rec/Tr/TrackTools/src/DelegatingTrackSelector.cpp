
//-----------------------------------------------------------------------------
/** @file DelegatingTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : DelegatingTrackSelector
 *
 *  CVS Log :-
 *  $Id: DelegatingTrackSelector.cpp,v 1.1 2009-07-01 18:28:07 jonrob Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "DelegatingTrackSelector.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( DelegatingTrackSelector );

//-----------------------------------------------------------------------------

DelegatingTrackSelector::DelegatingTrackSelector( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name, parent )
{
  // interface
  declareInterface<ITrackSelector>(this);
  // JOs
  m_trTypes =
    boost::assign::list_of("Velo")("VeloR")("Long")("Upstream")("Downstream")("Ttrack");
  declareProperty( "TrackTypes", m_trTypes );
}

DelegatingTrackSelector::~DelegatingTrackSelector() { }

StatusCode DelegatingTrackSelector::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // initialise track type and alg selections
  m_selTypes.clear();
  if ( !m_trTypes.empty() )
  {
    for ( TrackTypes::const_iterator iT = m_trTypes.begin();
          iT != m_trTypes.end(); ++iT )
    {
      if      ( *iT == "Velo"       ) { m_selTypes[Track::Velo]       = true; }
      else if ( *iT == "VeloR"      ) { m_selTypes[Track::VeloR]      = true; }
      else if ( *iT == "Long"       ) { m_selTypes[Track::Long]       = true; }
      else if ( *iT == "Upstream"   ) { m_selTypes[Track::Upstream]   = true; }
      else if ( *iT == "Downstream" ) { m_selTypes[Track::Downstream] = true; }
      else if ( *iT == "Ttrack"     ) { m_selTypes[Track::Ttrack]     = true; }
      else
      {
        return Error( "Unknown track type '"+*iT+"'" );
      }
    }
    // Note, track types not selected above, will automatically NOT be selected
  }

  return sc;
}

bool DelegatingTrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << aTrack.key() << " " << aTrack.type()
              << endreq;
  }

  // track types
  if ( !m_selTypes[aTrack.type()] )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track type " << aTrack.type() << " is rejected" << endreq;
    return false;
  }

  // delegate
  return trackSelector(aTrack)->accept(aTrack);
}

ITrackSelector * DelegatingTrackSelector::trackSelector( const LHCb::Track& aTrack ) const
{
  ITrackSelector * selector(NULL);
  TrackSelectors::const_iterator iS = m_trSels.find(aTrack.type());
  if ( iS == m_trSels.end() )
  {
    std::ostringstream name;
    name << aTrack.type();
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
