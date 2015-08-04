// $Id: $
// Include files 
// -------------
// From Gaudi
#include "GaudiKernel/IIncidentSvc.h"

// From Boost
#include <boost/foreach.hpp>

// From Event/TrackEvent
#include "Event/Track.h"

// Local
#include "VeloSelectChannelIDOnTrack.h"


//-----------------------------------------------------------------------------
// Implementation file for class : VeloSelectChannelIDOnTrack
//
// 2011-06-28 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloSelectChannelIDOnTrack )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSelectChannelIDOnTrack::VeloSelectChannelIDOnTrack( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_configured( false )
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default );

  declareInterface<IVeloChannelIDSelector>( this );
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode VeloSelectChannelIDOnTrack::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_ids.clear();
  
  incSvc() -> addListener( this, IncidentType::BeginEvent );
  
  if ( msgLevel( MSG::INFO ) )
    info() << "Will look at tracks from location \"" << m_trackLocation << "\" ..." << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
void VeloSelectChannelIDOnTrack::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_configured = false;
}

//=============================================================================
// 
//=============================================================================
bool VeloSelectChannelIDOnTrack::select( const LHCb::VeloChannelID& id ) const
{
  return (*this) ( id );
}

//=============================================================================
// 
//============================================================================= 
bool VeloSelectChannelIDOnTrack::operator()( const LHCb::VeloChannelID& id ) const
{
  if ( ! m_configured ) {
    initEvent();
    m_configured = true;
  }
  
  return std::binary_search( m_ids.begin(), m_ids.end(), id );
}

//=============================================================================
// 
//============================================================================= 
void VeloSelectChannelIDOnTrack::initEvent() const
{  
  m_ids.clear();
  
  const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackLocation );
  
  for ( LHCb::Tracks::const_iterator iterT = tracks -> begin();
        iterT != tracks -> end(); ++iterT ) {
    const std::vector<LHCb::LHCbID>& ids = (*iterT) -> lhcbIDs();
    BOOST_FOREACH( LHCb::LHCbID chan, ids ) {
      if ( chan.isVelo() ) m_ids.push_back( chan.veloID() );
    } // ids
  }  // tracks
  
  std::sort( m_ids.begin(), m_ids.end() );
}

//=============================================================================
// Destructor
//=============================================================================
VeloSelectChannelIDOnTrack::~VeloSelectChannelIDOnTrack() {} 

//=============================================================================
