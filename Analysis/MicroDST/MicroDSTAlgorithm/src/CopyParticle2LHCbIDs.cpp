// $Id: CopyParticle2LHCbIDs.cpp,v 1.2 2010-08-20 09:47:07 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <GaudiKernel/ObjectVector.h>
// LHCb
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/LHCbID.h"
#include "Event/Particle.h"
// DaVinci
#include "Kernel/DaVinciStringUtils.h"
#include "Kernel/Particle2LHCbIDs.h"
// local
#include "CopyParticle2LHCbIDs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2LHCbIDs
//
// 2010-08-18 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2LHCbIDs )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2LHCbIDs::CopyParticle2LHCbIDs( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator ),
    m_outputLocation  ( ""                 ),
    m_fullTree        ( false              ),
    m_iTisTos         ( NULL               ),
    m_p2LHCbID        ( NULL               )
{
  declareProperty("FullDecayTree", m_fullTree);
}

//=============================================================================
// Destructor
//=============================================================================
CopyParticle2LHCbIDs::~CopyParticle2LHCbIDs() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2LHCbIDs::initialize() 
{
  const StatusCode sc = MicroDSTAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc; 

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_iTisTos = tool<ITriggerTisTos>("TriggerTisTos", this);

  m_outputLocation = this->outputTESLocation( "Particle2LHCbIDMap" );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2LHCbIDs::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_p2LHCbID = new DaVinci::Map::Particle2LHCbIDs;

  put(m_p2LHCbID, m_outputLocation);

  typedef std::vector<std::string>::const_iterator stringIter;

  for ( stringIter iLoc = this->inputTESLocations().begin();
        iLoc != this->inputTESLocations().end(); ++iLoc ) 
  {
    const std::string inputLocation = niceLocationName(*iLoc);
    executeLocation(inputLocation);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

void CopyParticle2LHCbIDs::executeLocation(const std::string& particleLocation)
{

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Going to clone LHCbIDs from Particles in " << particleLocation
              << " into " << m_outputLocation << endmsg;

  if ( !exist<LHCb::Particle::Range>(particleLocation) ) 
  {
//     Warning( "No LHCb::Particle::Range found at " + particleLocation,
//              StatusCode::SUCCESS,
//              0 ).ignore();
    return;
  }

  const LHCb::Particle::Range particles =
    get<LHCb::Particle::Range>(particleLocation);

  if ( particles.empty() ) 
  {
//     Warning( "No LHCb::Particles in LHCb::Particle::Range " + particleLocation,
//              StatusCode::SUCCESS,
//              0 ).ignore();
    return;
  }
  
  for ( LHCb::Particle::Range::const_iterator iPart = particles.begin();
        iPart != particles.end(); ++iPart) 
  {
    storeLHCbIDs(*iPart);
  }
  
}

//=============================================================================

void CopyParticle2LHCbIDs::storeLHCbIDs(const LHCb::Particle* part)
{

  const LHCb::Particle* clone = getStoredClone<LHCb::Particle>(part);

  if ( !clone ) return;

  if ( m_p2LHCbID->find(clone) != m_p2LHCbID->end() ) return;

  m_iTisTos->setOfflineInput();
  m_iTisTos->addToOfflineInput(*part);
  const std::vector<LHCb::LHCbID> signalHits = m_iTisTos->offlineLHCbIDs();
  if ( signalHits.empty() ) return;

  m_p2LHCbID->insert(clone, signalHits);

  if ( m_fullTree )
  {
    const SmartRefVector<LHCb::Particle> & daughters = part->daughters();
    for ( SmartRefVector<LHCb::Particle>::const_iterator iDau = daughters.begin();
          iDau != daughters.end(); ++iDau )
    {
      storeLHCbIDs(*iDau);
    }
  }

}

//=============================================================================
