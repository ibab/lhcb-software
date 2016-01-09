// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

//from Event
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "HltTrackFilterGhostProb.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackFilterGhostProb
//
// 2016-01-08 : Sascha Stahl
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackFilterGhostProb )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFilterGhostProb::HltTrackFilterGhostProb( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
: base_class ( type, name , parent ),
  m_ghostTool(nullptr)
{
  declareInterface<ITracksFromTrack>(this);
  declareProperty("GhostIdTool", m_ghostToolName = "Run2GhostId");
  // The streamers don't like  std::numeric_limits<double>::infinity())
  declareProperty("MaxChi2PerDoFCut", m_maxChi2PerDoFCut =  999999.);
  declareProperty("MaxGhostProbCut",   m_maxGhostProbCut   =  999999.);
}

//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void HltTrackFilterGhostProb::handle ( const Incident& incident ) {
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_initializeEvent = true;
  }
}
//=============================================================================
StatusCode HltTrackFilterGhostProb::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_ghostTool =  tool<IGhostProbability>( m_ghostToolName, this);
  m_initializeEvent = true;
  incSvc()->addListener(this, IncidentType::BeginEvent);
   return sc;
}

//=============================================================================
StatusCode HltTrackFilterGhostProb::tracksFromTrack( const LHCb::Track& track,
		                                 std::vector<LHCb::Track*>& tracks )
{
  if ( m_initializeEvent ){
    //always()<<"Initialize event"<<endmsg;
    if ( m_ghostTool->beginEvent().isFailure() ){
      return Error("GhostIdTool could not begin event.", StatusCode::SUCCESS,10);
    }
    //always()<<"Initialized event"<<endmsg;
    m_initializeEvent = false;
  }
  auto tr = const_cast<LHCb::Track*>( &track );
  if ( track.chi2PerDoF() < m_maxChi2PerDoFCut ){
    // Workaround to accomodate that the variable is not always filled in Hlt1.
    if (track.info(LHCb::Track::NCandCommonHits, -999) == -999){
      tr->addInfo(LHCb::Track::NCandCommonHits,0);
    }
    m_ghostTool->execute( const_cast<LHCb::Track&>(track) ).ignore();
  }else{
    return StatusCode::SUCCESS;
  }
  if ( track.ghostProbability() < m_maxGhostProbCut ){
    // There is a good enough candidate, put the track into the output.
    tracks.push_back( tr );
  }
  return StatusCode::SUCCESS;
}
