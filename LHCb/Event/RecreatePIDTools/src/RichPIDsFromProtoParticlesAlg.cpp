
//-----------------------------------------------------------------------------
/** @file RichPIDsFromProtoParticlesAlg.cpp
 *
 * Implementation file for algorithm RichPIDsFromProtoParticlesAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichPIDsFromProtoParticlesAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichPIDsFromProtoParticlesAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPIDsFromProtoParticlesAlg::
RichPIDsFromProtoParticlesAlg( const std::string& name,
                               ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
{
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPloc  = ProtoParticleLocation::HltCharged;
    m_richPIDloc = RichPIDLocation::HLT;
  }
  else
  {
    m_protoPloc  = ProtoParticleLocation::Charged;
    m_richPIDloc = RichPIDLocation::Default;
  }
  declareProperty( "InputProtoParticles", m_protoPloc  );
  declareProperty( "OutputRichPIDs",      m_richPIDloc );
}

//=============================================================================
// Destructor
//=============================================================================
RichPIDsFromProtoParticlesAlg::~RichPIDsFromProtoParticlesAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichPIDsFromProtoParticlesAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Creating RichPIDs '" << m_richPIDloc 
         << "' from ProtoParticles at '"
         << m_protoPloc << "'" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichPIDsFromProtoParticlesAlg::execute()
{

  // check data is not already there
  auto * rpids = getIfExists<RichPIDs>(m_richPIDloc);
  if ( rpids )
  {
    debug() << "Data already exists at '" << m_richPIDloc
            << "' -> Will NOT replace" << endmsg;
  }
  else
  {

    // new RichPID container
    rpids = new RichPIDs();
    put( rpids, m_richPIDloc );

    // Do ProtoParticles exist
    const auto * protos = getIfExists<ProtoParticles>( m_protoPloc );
    if ( protos )
    {

      // loop over protos and re-create RichPIDs
      for ( auto * proto : *protos )
      {

        // get Track pointer
        const auto * track = proto->track();
        if ( !track )
        {
          Warning( "Charged ProtoParticle has NULL Track pointer" );
          continue;
        }

        // does this proto have any Rich info in it ?
        if ( proto->hasInfo(ProtoParticle::RichPIDStatus) )
        {

          // new RichPID
          auto * pid = new RichPID();

          // Add to container with same key as Track
          rpids->insert( pid, track->key() );

          // make sure proto points to this RichPID
          proto->setRichPID( pid );

          // copy info

          // history word
          pid->setPidResultCode( static_cast<unsigned int>(proto->info(ProtoParticle::RichPIDStatus,0)) );

          // Track Pointer
          pid->setTrack( track );

          // DLLs
          pid->setParticleDeltaLL(Rich::Electron,       (float)proto->info(ProtoParticle::RichDLLe, 0));
          pid->setParticleDeltaLL(Rich::Muon,           (float)proto->info(ProtoParticle::RichDLLmu,0));
          pid->setParticleDeltaLL(Rich::Pion,           (float)proto->info(ProtoParticle::RichDLLpi,0));
          pid->setParticleDeltaLL(Rich::Kaon,           (float)proto->info(ProtoParticle::RichDLLk, 0));
          pid->setParticleDeltaLL(Rich::Proton,         (float)proto->info(ProtoParticle::RichDLLp, 0));
          pid->setParticleDeltaLL(Rich::Deuteron,       (float)proto->info(ProtoParticle::RichDLLd, 0));
          pid->setParticleDeltaLL(Rich::BelowThreshold, (float)proto->info(ProtoParticle::RichDLLbt,0));

        } // has rich info

      }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Created XXX" << rpids->size() << " RichPIDs at " << m_richPIDloc << endmsg;
      }

    }
    else
    {
      Warning( "No ProtoParticles at '" + m_protoPloc +
               "' -> Empty RichPIDs created at '" + m_richPIDloc + "'" ).ignore();
    }

  }

  // Final check for inconsistent BelowThreshold/Deuteron information...
  // Deuteron information was added in RichPID version 2
  if ( rpids && rpids->version() < 2 )
  {
    // Loop over PIDs and look for inconsistent ones
    for ( auto * rpid : *rpids )
    {
      // Correct the best ID, if its set to Deuteron
      if ( Rich::Deuteron == rpid->bestParticleID() )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Correcting Best PID " << endmsg;
        rpid->setBestParticleID( Rich::BelowThreshold );
      }
      // Move the 'Deuteron' DLL to Below threshold if needed
      const auto dDLL = rpid->particleDeltaLL(Rich::Deuteron);
      if ( fabs(dDLL) > 0.0f )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Correcting DLLs " << endmsg;
        rpid->setParticleDeltaLL( Rich::BelowThreshold, dDLL );
        rpid->setParticleDeltaLL( Rich::Deuteron, 0.0f );
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
