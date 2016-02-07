// Include files

// local
#include "RichRecPIDTupleAlg.h"

//-----------------------------------------------------------------------------

using namespace Rich::Rec::MC;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PIDTupleAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDTupleAlg::PIDTupleAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Rich::Rec::TupleAlgBase ( name , pSvcLocator )
{
  m_pidLocations.push_back( LHCb::RichPIDLocation::Offline );
  m_pidLocations.push_back( LHCb::RichPIDLocation::HLT     );
  declareProperty( "RichPIDLocations", m_pidLocations );
}

//=============================================================================
// Destructor
//=============================================================================
PIDTupleAlg::~PIDTupleAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PIDTupleAlg::initialize()
{
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "TrackSelector",      m_trSelector,  this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PIDTupleAlg::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    _ri_debug << "No tracks found : Created " << richTracks()->size()
              << " RichRecTracks " << richSegments()->size()
              << " RichRecSegments" << endmsg;
  }

  // Load the associations
  StatusCode sc = fillTrackTable();
  if ( sc.isFailure() ) return sc;

  // dummy data values
  const std::vector<float> dummyDLLs(Rich::NParticleTypes,0);

  // loop over track map
  for ( const auto & ipid : m_pidMap )
  {

    // get the tuple
    Tuple tuple = nTuple("RichPID", "Rich PID Information");

    // MCParticle information
    const LHCb::MCParticle * mcPart = ipid.first;
    tuple->column( "MCParticleType", mcPart ? mcPart->particleID().pid() : 0 );
    tuple->column( "MCParticleP",    mcPart ? mcPart->p()  : -99999 );
    tuple->column( "MCParticlePt",   mcPart ? mcPart->pt() : -99999 );
    tuple->column( "MCVirtualMass",  mcPart ? mcPart->virtualMass() : -99999 );

    // Add reco information for each RichPID associated to the MCParticle
    for ( unsigned int n = 0; n < m_pidLocations.size(); ++n )
    {
      const std::string sn = boost::lexical_cast<std::string>(n);
      // find a pid
      const LHCb::RichPID * pid = (ipid.second)[n];
      // Add best ID
      tuple->column( "BestID"+sn, pid ? (int)pid->bestParticleID() : -1 );
      // Add the DLL values
      tuple->array( "Dlls"+sn,
                    pid ? pid->particleLLValues().begin() : dummyDLLs.begin(),
                    pid ? pid->particleLLValues().end()   : dummyDLLs.end() );
      // Add reco momentum
      tuple->column( "RecoP"+sn,  pid ? pid->track()->p()  : -1 );
      tuple->column( "RecoPt"+sn, pid ? pid->track()->pt() : -1 );
    }

    // write the tuple for this track
    tuple->write();

  }

  return sc;
}

StatusCode PIDTupleAlg::fillTrackTable()
{
  // clear the map
  m_pidMap.clear();

  // Loop over all PID locations
  unsigned int n(0);
  for ( auto iLoc = m_pidLocations.begin(); iLoc != m_pidLocations.end(); ++iLoc, ++n )
  {
    // try and load this location
    const auto * pids = get<LHCb::RichPIDs>( *iLoc );
    _ri_verbo << "Loaded " << pids->size() << " RichPIDs from " << *iLoc << endmsg;
    // loop over RichPIDs
    for ( const auto * PID : *pids )
    {
      // the reco track
      const LHCb::Track * track = PID->track();
      // apply track selection
      if ( !m_trSelector->trackSelected(track) ) continue;
      // associated MC
      const LHCb::MCParticle * mcPart = m_richRecMCTruth->mcParticle( track );
      _ri_verbo << " -> Track " << track << " MCParticle " << mcPart << endmsg;
      // fill map
      if ( mcPart )
      {
        PIDVector & vect = m_pidMap[mcPart];
        if ( vect.empty() ) { vect = PIDVector(m_pidLocations.size(),nullptr); }
        vect[n] = PID;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
