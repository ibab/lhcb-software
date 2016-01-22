// $Id: CompareProtoParticle.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
// Include files 

// local
#include "CompareProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareProtoParticle
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareProtoParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareProtoParticle::CompareProtoParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName", m_inputName = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "TestName" , m_testName  = LHCb::ProtoParticleLocation::Charged+"Test" );
}

//=============================================================================
// Destructor
//=============================================================================
CompareProtoParticle::~CompareProtoParticle() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareProtoParticle::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
 
  LHCb::ProtoParticles* old  = get<LHCb::ProtoParticles>( m_inputName  );
  LHCb::ProtoParticles* test = get<LHCb::ProtoParticles>( m_testName  );

  // check and return
  const LHCb::ProtoParticlePacker packer(this);
  return packer.check( *old, *test );
}

//=============================================================================
