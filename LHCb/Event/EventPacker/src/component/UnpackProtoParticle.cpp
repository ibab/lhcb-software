// $Id: UnpackProtoParticle.cpp,v 1.6 2009-11-10 10:25:07 jonrob Exp $
// Include files

// local
#include "UnpackProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackProtoParticle
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackProtoParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  UnpackProtoParticle::UnpackProtoParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedProtoParticleLocation::Charged );
  declareProperty( "OutputName", m_outputName = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
UnpackProtoParticle::~UnpackProtoParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackProtoParticle::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedProtoParticles>(m_inputName) )
    return StatusCode::SUCCESS;

  const auto * dst =
    getOrCreate<LHCb::PackedProtoParticles,LHCb::PackedProtoParticles>( m_inputName );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << dst->protos().size() << " PackedProtoParticles at '"
            << m_inputName << "'" << endmsg;

  auto * newProtoParticles = new LHCb::ProtoParticles();
  put( newProtoParticles, m_outputName );

  // unpack
  const LHCb::ProtoParticlePacker packer(*this);
  packer.unpack( *dst, *newProtoParticles );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << newProtoParticles->size() << " ProtoParticles at '"
            << m_outputName << "'" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
