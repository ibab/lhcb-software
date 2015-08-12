// $Id$
// Include files

// local
#include "ConjugateNeutralPID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ConjugateNeutralPID
//
// 2006-12-15 : Patrick Spradlin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ConjugateNeutralPID )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ConjugateNeutralPID::ConjugateNeutralPID( const std::string& name,
                                            ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
    , m_changePIDTool(0)
    , m_nEvents(0)
    , m_nAccepted(0)
    , m_nCandidates(0)
{ }

//=============================================================================
// Destructor
//=============================================================================
ConjugateNeutralPID::~ConjugateNeutralPID() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConjugateNeutralPID::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  m_changePIDTool = tool<IChangePIDTool>("NeutralCCChangePIDTool", this);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ConjugateNeutralPID::execute()
{

  debug() << "==> Execute" << endmsg;
  ++m_nEvents;  // Increment event counter

  setFilterPassed( false );

  const LHCb::Particle::ConstVector& inparts = this->i_particles();
  verbose() << "Retrieved " << inparts.size() << " particles from local storage" << endmsg;

  // Apply the ChangePIDTool to the particles.
  std::vector<LHCb::Particle> outparts = m_changePIDTool->changePID( inparts );

  // Save each of the modified Particles to the local storage.
  std::vector<LHCb::Particle>::iterator i;
  for( i = outparts.begin(); i != outparts.end(); i++ )
  {
    const LHCb::Particle *deskPart = this->markTree( &(*i) );
    if( !deskPart )
    {
      err() << " Unable to save particle" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if( !outparts.empty() )
  {
    setFilterPassed( true );
    ++m_nAccepted;   // Increment accepted event counter
    m_nCandidates += outparts.size();  // Increment candidate counter

    debug() << "Saved " << outparts.size() << " from " << inparts.size()
            << " candidates" << endmsg;
  }
  else
  {
    debug() << "Writing nothing to output" << endmsg;
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ConjugateNeutralPID::finalize()
{
  info() << "Passed " << m_nCandidates << " candidates in "
         << m_nAccepted << " accepted events among "
         << m_nEvents << " events" << endmsg ;
  return DaVinciAlgorithm::finalize();
}

//=============================================================================
