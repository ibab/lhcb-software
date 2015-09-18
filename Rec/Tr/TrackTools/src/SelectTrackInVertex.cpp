// include files
#include "SelectTrackInVertex.h"
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SelectTrackInVertex
//
// 2015-09-18 : Michel De Cian
//
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY( SelectTrackInVertex )

//=============================================================================
// -- Constructor
//=============================================================================
SelectTrackInVertex::SelectTrackInVertex( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
: TrackSelectorBase ( type, name, parent ){
  
  declareProperty( "VertexContainer", m_vertexContainerName = LHCb::RecVertexLocation::Primary );

}
//=============================================================================
// -- Destructor
//=============================================================================
SelectTrackInVertex::~SelectTrackInVertex() { }
//=============================================================================
// -- Initialize
//=============================================================================
StatusCode SelectTrackInVertex::initialize(){

  StatusCode sc = TrackSelectorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  incSvc()->addListener(this, IncidentType::BeginEvent);
  m_newEvent = true;

  return StatusCode::SUCCESS;
  
}
//=============================================================================
// -- Check if track is in track collection belonging to a vertex
//=============================================================================
bool SelectTrackInVertex::accept ( const LHCb::Track& aTrack ) const{
 
  if( m_newEvent ) getTracksFromVertices();
  
  if(  msgLevel( MSG::DEBUG ) ){
    bool found = std::binary_search( std::begin(m_tracks), std::end(m_tracks), &aTrack);
    if( !found ) debug() << "Track " << &aTrack << " does not belong to a vertex" << endmsg;
    return found;
  }else{
    return std::binary_search( std::begin(m_tracks), std::end(m_tracks), &aTrack);
  }
  
}
//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void SelectTrackInVertex::handle ( const Incident& incident ) {
  
  if ( IncidentType::BeginEvent == incident.type() ) m_newEvent = true;
  
}
//=============================================================================
// -- Get vertices, once per event
//=============================================================================
void SelectTrackInVertex::getTracksFromVertices() const {
  
  m_tracks.clear();
  
  const LHCb::RecVertices* vertexContainer = getIfExists<LHCb::RecVertices>( m_vertexContainerName );
  if( !vertexContainer) return;
  
  for( const LHCb::RecVertex* vert : *vertexContainer){
  
    if( !vert) continue;
    
    const SmartRefVector<LHCb::Track> vTracks = vert->tracks();
    
    for( const LHCb::Track* track : vTracks){
      m_tracks.push_back( track );
    }
    
  }

  std::sort( std::begin(m_tracks), std::end(m_tracks), 
             [](const LHCb::Track* lhs, const LHCb::Track* rhs){ return lhs < rhs; });
  
  m_newEvent = false;
  
}

