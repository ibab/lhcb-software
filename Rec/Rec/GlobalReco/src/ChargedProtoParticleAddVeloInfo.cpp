
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddVeloInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddVeloInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleAddVeloInfo.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddVeloInfo::
ChargedProtoParticleAddVeloInfo( const std::string& name,
                                 ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ), 
    m_velodEdx     ( NULL )
{
  // default locations from context()
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPath  = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  }
  declareProperty( "ProtoParticleLocation", m_protoPath );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddVeloInfo::~ChargedProtoParticleAddVeloInfo() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoParticleAddVeloInfo::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_velodEdx = tool<ITrackVelodEdxCharge>( "TrackVelodEdxCharge", "VeloCharge", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddVeloInfo::execute()
{
  // ProtoParticle container
  LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>(m_protoPath);
  if ( !protos )
  {
    return Warning( "No existing ProtoParticle container at " +  
                    m_protoPath + " thus do nothing.",
                    StatusCode::SUCCESS );
  }

  // Loop over proto particles and fill dE/dx
  for ( auto * P : *protos ) { addVelodEdx(P); }

  counter( "VeloDEDX ==> " + m_protoPath ) += protos->size();

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Add VELO dE/dx info to the protoparticle
//=============================================================================
bool 
ChargedProtoParticleAddVeloInfo::addVelodEdx( LHCb::ProtoParticle * proto ) const
{
  // clear current Velo info
  proto->removeVeloInfo();

  // get velo charge
  double veloNtks(0);
  const StatusCode sc = m_velodEdx->nTracks( proto->track(), veloNtks );
  if ( sc.isSuccess() )
  { proto->addInfo( LHCb::ProtoParticle::VeloCharge, veloNtks ); }

  // return status
  return sc.isSuccess();
}

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddVeloInfo )
