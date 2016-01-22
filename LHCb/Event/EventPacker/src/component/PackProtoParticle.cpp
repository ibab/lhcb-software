// Include files

// local
#include "PackProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackProtoParticle
//
// 2008-11-13 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackProtoParticle::PackProtoParticle( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName"  , m_inputName  = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "OutputName" , m_outputName = LHCb::PackedProtoParticleLocation::Charged );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput  = false     );
  declareProperty( "DeleteInput",                m_deleteInput   = false     );
  declareProperty( "EnableCheck",                m_enableCheck   = false     );
  declareProperty( "ClearRegistry",              m_clearRegistry = true      );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
PackProtoParticle::~PackProtoParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackProtoParticle::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::ProtoParticles>(m_inputName) ) return StatusCode::SUCCESS;

  LHCb::ProtoParticles* parts = getOrCreate<LHCb::ProtoParticles,LHCb::ProtoParticles>( m_inputName );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << parts->size() << " ProtoParticles at '" << m_inputName << "'" << endmsg;

  LHCb::PackedProtoParticles* out = new LHCb::PackedProtoParticles();
  put( out, m_outputName );
  out->setVersion( 2 ); // CRJ : Why set this ?
  out->setPackingVersion( LHCb::PackedProtoParticles::defaultPackingVersion() );

  // pack
  const LHCb::ProtoParticlePacker packer(this);
  packer.pack( *parts, *out );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << out->protos().size() << " PackedProtoParticles at '"
            << m_outputName << "'" << endmsg;

  // Packing checks
  if ( UNLIKELY(m_enableCheck) )
  {
    // make new unpacked output data object
    LHCb::ProtoParticles * unpacked = new LHCb::ProtoParticles();
    put( unpacked, m_inputName+"_PackingCheck" );
    
    // unpack
    packer.unpack( *out, *unpacked );
    
    // run checks
    packer.check( *parts, *unpacked ).ignore();
    
    // clean up after checks
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) 
    {
      delete unpacked;
    }
    else
    {
      return Error( "Failed to delete test data after unpacking check", sc );  
    }
  }

  // If requested, remove the input data from the TES and delete
  if ( UNLIKELY(m_deleteInput) )
  {
    const StatusCode sc = evtSvc()->unregisterObject( parts );
    if ( sc.isSuccess() )
    {
      delete parts;
      parts = nullptr;
    }
    else
    {
      return Error("Failed to delete input data as requested", sc );
    }
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    if ( m_clearRegistry ) { parts->registry()->setAddress(0); }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackProtoParticle )

//=============================================================================
