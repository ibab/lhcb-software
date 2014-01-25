// $Id: L0ConfirmWithT.cpp,v 1.16 2010-01-06 07:48:43 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/State.h"
#include "Event/Track.h"

#include "HltBase/ITrackConfirmTool.h"

// local
#include "L0ConfirmWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0ConfirmWithT
//
// 2007-07-04 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0ConfirmWithT )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ConfirmWithT::L0ConfirmWithT( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
  m_l0ConfExtrapolator(0)
  , m_TrackConfirmTool(0)
  , m_DataStore(0)
{
  declareInterface<ITracksFromTrack>(this);
  declareInterface<ITrackView>(this);
  
  declareProperty("trackingTool", m_trackingTool = "PatConfirmTool");
  declareProperty("debugMode",m_debugMode = false );
  declareProperty("particleType", m_particleTypeTMP = 0 );
}
//=============================================================================
// Destructor
//=============================================================================
L0ConfirmWithT::~L0ConfirmWithT() {} 

//=============================================================================
StatusCode L0ConfirmWithT::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " Initialize L0ConfirmWithT" << endmsg;

  // check if particle type is valid
  if ( m_particleTypeTMP < Muon || m_particleTypeTMP >= Last) {
	fatal() << "Unknown particle type " << m_particleTypeTMP << endmsg
		<< "Can't continue." << endmsg;
	return StatusCode::FAILURE;
  }
  m_particleType = static_cast<ParticleType>(m_particleTypeTMP);

  // Pattern recognition tool
  m_TrackConfirmTool=tool<ITrackConfirmTool>( m_trackingTool, this  );

  // to store debug information
  m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");

  // tool to extrapolate states
  m_l0ConfExtrapolator = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");

  debug() << "Selected particle type " << m_particleType << endmsg;

  return sc;
}

StatusCode L0ConfirmWithT::tracksFromTrack(
		const LHCb::Track& seed,
		std::vector<LHCb::Track*>& tracks )
{
  if ( msgLevel(MSG::DEBUG) )
	  debug() << "enter tracksFromTrack method" << endmsg;
  if ( seed.nStates() < 1 ) {
    error() << "No states on track - skipping" << endmsg;
    if ( m_debugMode ) {
      m_DataStore->nTHits.push_back( 0 );
      m_DataStore->decodingTime.push_back( 0 );
      m_DataStore->trackingTime.push_back( 0 );
    }
    return StatusCode::SUCCESS;
  }
  
  // get state(s) for the track search which we want to confirm
  // the L0 decision
  LHCb::State seedStates[2];
  
  int nStates = 1;
  StatusCode sc = prepareStates(seed , seedStates, nStates);
  if( sc.isFailure() ) return StatusCode::FAILURE;
  
  while (nStates--) {
    sc = m_TrackConfirmTool->tracks( seedStates[nStates], tracks );
    if (sc.isFailure()) break;
  }
  return sc;
}


std::vector<Tf::IStationSelector*> L0ConfirmWithT::view(const LHCb::Track& seed)
{
// get state(s) for the track search which we want to confirm
  // the L0 decision
  std::vector<Tf::IStationSelector*> windows;
  LHCb::State seedStates[2];
  int nStates = 1;
  StatusCode sc = prepareStates(seed , seedStates, nStates);
  if( sc.isFailure() ) return windows;

  std::vector<LHCb::LHCbID> ids;
  while (nStates--) {
    ParabolaHypothesis  tp = m_TrackConfirmTool->prepareT( seedStates[nStates], ids );
	  if (sc.isFailure()) break;
    windows.push_back( new ParabolaHypothesis(tp) );
  }
  return windows;
}


std::vector<LHCb::LHCbID> L0ConfirmWithT::lhcbIDsInView(const LHCb::Track& seed)
{
  std::vector<LHCb::LHCbID> ids;
  
  // get state(s) for the track search which we want to confirm
  // the L0 decision
  LHCb::State seedStates[2];
  int nStates = 1;
  StatusCode sc = prepareStates(seed , seedStates, nStates);
  if( sc.isFailure() ) return ids;

  while (nStates--) {
    ParabolaHypothesis tp = m_TrackConfirmTool->prepareT( seedStates[nStates], ids );
	  if (sc.isFailure()) break;
  }
  return ids;
}


StatusCode L0ConfirmWithT::prepareStates( const LHCb::Track& seed, LHCb::State* seedStates, int& nStates )
{
  switch (m_particleType) {
  case Muon:
    m_l0ConfExtrapolator->muon2T( seed , seedStates[0] );
    break;
  case Hadron:
    m_l0ConfExtrapolator->hcal2T( seed , seedStates[0] , seedStates[1] );
    if( fabs(m_l0ConfExtrapolator->getBScale() ) > 0.1 ) ++nStates;
    break;
  case Electron:
    m_l0ConfExtrapolator->ecal2T( seed , seedStates[0] , seedStates[1] );
    if( fabs(m_l0ConfExtrapolator->getBScale() ) > 0.1 ) ++nStates;
    break;
  default:
    // Unknown particle type, complain
    error()<<"unknown particle type, return.."<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS; 
}
