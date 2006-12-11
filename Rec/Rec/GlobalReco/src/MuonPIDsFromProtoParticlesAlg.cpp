
//-----------------------------------------------------------------------------
/** @file MuonPIDsFromProtoParticlesAlg.cpp
 *
 * Implementation file for algorithm MuonPIDsFromProtoParticlesAlg
 *
 * CVS Log :-
 * $Id: MuonPIDsFromProtoParticlesAlg.cpp,v 1.3 2006-12-11 20:48:20 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "MuonPIDsFromProtoParticlesAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonPIDsFromProtoParticlesAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPIDsFromProtoParticlesAlg::MuonPIDsFromProtoParticlesAlg( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputProtoParticles", m_protoPloc  = ProtoParticleLocation::Charged );
  declareProperty( "OutputMuonPIDs",      m_muonPIDloc = MuonPIDLocation::Default       );
}
//=============================================================================
// Destructor
//=============================================================================
MuonPIDsFromProtoParticlesAlg::~MuonPIDsFromProtoParticlesAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonPIDsFromProtoParticlesAlg::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  

  info() << "Creating MuonPIDs '" << m_muonPIDloc << "' from ProtoParticles at '"
         << m_protoPloc << "'" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonPIDsFromProtoParticlesAlg::execute() 
{

  // load ProtoParticles
  const ProtoParticles * protos = get<ProtoParticles>( m_protoPloc );

  // new MuonPID container
  MuonPIDs * mpids = new MuonPIDs();
  put( mpids, m_muonPIDloc );

  // loop over protos and re-create RichPIDs
  for ( ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    // get Track pointer
    const Track * track = (*iP)->track();
    if ( !track )
    {
      Warning( "Charged ProtoParticle has NULL Track pointer" );
      continue;
    }

    // does this proto have any Muon info in it ?
    if ( (*iP)->hasInfo(ProtoParticle::MuonPIDStatus) )
    {

      // new MuonPID
      MuonPID * pid = new MuonPID();
      // Add to container with same key as Track 
      mpids->insert( pid, track->key() );

      // copy info

      // history word
      pid->setStatus( static_cast<unsigned int>((*iP)->info(ProtoParticle::MuonPIDStatus,0)) );

      // Track Pointer
      pid->setIDTrack( track );

      // PID info
      pid->setMuonLLMu( (*iP)->info(ProtoParticle::MuonMuLL,    0) );
      pid->setMuonLLBg( (*iP)->info(ProtoParticle::MuonBkgLL,   0) );
      pid->setNShared(  static_cast<int>((*iP)->info(ProtoParticle::MuonNShared, 0)) );

    } // has muon info

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonPIDsFromProtoParticlesAlg::finalize() 
{
  return GaudiAlgorithm::finalize();
}

//=============================================================================
