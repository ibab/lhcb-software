
//-----------------------------------------------------------------------------
/** @file RichPIDsFromProtoParticlesAlg.cpp
 *
 * Implementation file for algorithm RichPIDsFromProtoParticlesAlg
 *
 * CVS Log :-
 * $Id: RichPIDsFromProtoParticlesAlg.cpp,v 1.4 2007-02-09 14:07:25 ukerzel Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RichPIDsFromProtoParticlesAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichPIDsFromProtoParticlesAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPIDsFromProtoParticlesAlg::RichPIDsFromProtoParticlesAlg( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputProtoParticles", m_protoPloc  = rootOnTES() + ProtoParticleLocation::Charged );
  declareProperty( "OutputRichPIDs",      m_richPIDloc = RichPIDLocation::Default                     );
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

  // load ProtoParticles
  const ProtoParticles * protos = get<ProtoParticles>( m_protoPloc );
  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "got " << protos->size() << " proto-particles from TES" << endmsg;
  } //if


  // new RichPID container
  RichPIDs * rpids = new RichPIDs();
  put( rpids, m_richPIDloc );

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

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "track with |p| " << track->p() << " has key " <<  track->key() << endmsg;
    }//if

    // does this proto have any Rich info in it ?
    if ( (*iP)->hasInfo(ProtoParticle::RichPIDStatus) )
    {

      // new RichPID
      RichPID * pid = new RichPID();
      // Add to container with same key as Track 
      rpids->insert( pid, track->key() );

      // set this RichPID to the proto-particle
      //  needed in case a clone of the proto-particle
      //  is written to a new file and the corresponding
      //  information is re-created when the proto-particle is
      //  read back .
      (*iP)->setRichPID(pid);

      // copy info

      // history word
      pid->setPidResultCode( static_cast<unsigned int>((*iP)->info(ProtoParticle::RichPIDStatus,0)) );

      // Track Pointer
      pid->setTrack( track );

      // DLLs
      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "set DLLs" << endmsg;
        verbose() << "DLL e  " << (*iP)->info(ProtoParticle::RichDLLe,  -999) << endmsg;
        verbose() << "DLL mu " << (*iP)->info(ProtoParticle::RichDLLmu, -999) << endmsg;
        verbose() << "DLL pi " << (*iP)->info(ProtoParticle::RichDLLpi, -999) << endmsg;
        verbose() << "DLL K  " << (*iP)->info(ProtoParticle::RichDLLk,  -999) << endmsg;
        verbose() << "DLL p  " << (*iP)->info(ProtoParticle::RichDLLp,  -999) << endmsg;
      }//if

      pid->setParticleDeltaLL(Rich::Electron,(*iP)->info(ProtoParticle::RichDLLe, -999));
      pid->setParticleDeltaLL(Rich::Muon,    (*iP)->info(ProtoParticle::RichDLLmu,-999));
      pid->setParticleDeltaLL(Rich::Pion,    (*iP)->info(ProtoParticle::RichDLLpi,-999));
      pid->setParticleDeltaLL(Rich::Kaon,    (*iP)->info(ProtoParticle::RichDLLk, -999));
      pid->setParticleDeltaLL(Rich::Proton,  (*iP)->info(ProtoParticle::RichDLLp, -999));


    } // has rich info

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichPIDsFromProtoParticlesAlg::finalize() 
{
  return GaudiAlgorithm::finalize();
}

//=============================================================================
