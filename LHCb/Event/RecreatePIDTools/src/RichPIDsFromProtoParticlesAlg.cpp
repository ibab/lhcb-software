
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
RichPIDsFromProtoParticlesAlg::RichPIDsFromProtoParticlesAlg( const std::string& name,
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

  info() << "Creating RichPIDs '" << m_richPIDloc << "' from ProtoParticles at '"
         << m_protoPloc << "'" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichPIDsFromProtoParticlesAlg::execute()
{

  // check data is not already there
  if ( exist<RichPIDs>( m_richPIDloc ) )
  {
    debug() << "Data already exists at '" << m_richPIDloc
            << "' -> Will NOT replace" << endmsg;
    return StatusCode::SUCCESS;
  }

  // new RichPID container
  RichPIDs * rpids = new RichPIDs();
  put( rpids, m_richPIDloc );

  // Do ProtoParticles exist
  const ProtoParticles * protos = getIfExists<ProtoParticles>( m_protoPloc );
  if ( protos )
  {

    // loop over protos and re-create RichPIDs
    for ( ProtoParticles::const_iterator iP = protos->begin();
          iP != protos->end(); ++iP )
    {

      verbose() << "consider new proto-particle " << endmsg;

      // get Track pointer
      const Track * track = (*iP)->track();
      if ( !track )
      {
        Warning( "Charged ProtoParticle has NULL Track pointer" );
        continue;
      }

      // does this proto have any Rich info in it ?
      if ( (*iP)->hasInfo(ProtoParticle::RichPIDStatus) )
      {

        // new RichPID
        RichPID * pid = new RichPID();

        // Add to container with same key as Track
        rpids->insert( pid, track->key() );

        // make sure proto points to this RichPID
        (*iP)->setRichPID( pid );

        // copy info

        // history word
        pid->setPidResultCode( static_cast<unsigned int>((*iP)->info(ProtoParticle::RichPIDStatus,0)) );

        // Track Pointer
        pid->setTrack( track );

        // DLLs
        pid->setParticleDeltaLL(Rich::Electron,(float)(*iP)->info(ProtoParticle::RichDLLe, -999));
        pid->setParticleDeltaLL(Rich::Muon,    (float)(*iP)->info(ProtoParticle::RichDLLmu,-999));
        pid->setParticleDeltaLL(Rich::Pion,    (float)(*iP)->info(ProtoParticle::RichDLLpi,-999));
        pid->setParticleDeltaLL(Rich::Kaon,    (float)(*iP)->info(ProtoParticle::RichDLLk, -999));
        pid->setParticleDeltaLL(Rich::Proton,  (float)(*iP)->info(ProtoParticle::RichDLLp, -999));

      } // has rich info

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << rpids->size() << " RichPIDs at " << m_richPIDloc << endmsg;
    }

  }
  else
  {
    Warning( "No ProtoParticles at '" + m_protoPloc + 
             "' -> Empty RichPIDs created at '" + m_richPIDloc + "'" ).ignore();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
