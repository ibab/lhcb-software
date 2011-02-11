// $Id: UnpackProtoParticle.cpp,v 1.6 2009-11-10 10:25:07 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/StandardPacker.h"
#include "Event/PackedProtoParticle.h"
#include "Event/ProtoParticle.h"

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
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackProtoParticle::~UnpackProtoParticle() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackProtoParticle::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedProtoParticles>(m_inputName) ) 
    return StatusCode::SUCCESS;

  const LHCb::PackedProtoParticles* dst = 
    getOrCreate<LHCb::PackedProtoParticles,LHCb::PackedProtoParticles>( m_inputName );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << dst->protos().size() << " PackedProtoParticles at '" << m_inputName << "'" << endmsg;

  LHCb::ProtoParticles* newProtoParticles = new LHCb::ProtoParticles();
  newProtoParticles->reserve(dst->protos().size());
  put( newProtoParticles, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedProtoParticle>::const_iterator itS = dst->protos().begin();
        dst->protos().end() != itS; ++itS ) {
    const LHCb::PackedProtoParticle& src = (*itS);

    LHCb::ProtoParticle* part = new LHCb::ProtoParticle( );
    newProtoParticles->insert( part, src.key );

    int hintID;
    int key;
    if ( -1 != src.track ) {
      pack.hintAndKey( src.track, dst, newProtoParticles, hintID, key );
      SmartRef<LHCb::Track> ref( newProtoParticles, hintID, key );
      part->setTrack( ref );
    }
    
    if ( -1 != src.richPID ) {
      pack.hintAndKey( src.richPID, dst, newProtoParticles, hintID, key );
      SmartRef<LHCb::RichPID> ref( newProtoParticles, hintID, key );
      part->setRichPID( ref );
    }
    
    if ( -1 != src.muonPID ) {
      pack.hintAndKey( src.muonPID, dst, newProtoParticles, hintID, key );
      SmartRef<LHCb::MuonPID> ref( newProtoParticles, hintID, key );
      part->setMuonPID( ref );
    }

    int kk;
    for ( kk = src.firstHypo; src.lastHypo > kk; ++kk ) {
      const int reference = *(dst->refs().begin()+kk);
      pack.hintAndKey( reference, dst, newProtoParticles, hintID, key );
      SmartRef<LHCb::CaloHypo> ref( newProtoParticles, hintID, key );
      part->addToCalo( ref );
    }

    for ( kk = src.firstExtra; src.lastExtra > kk; ++kk ) {
      const std::pair<int,int>& info = *(dst->extras().begin()+kk);
      part->addInfo( info.first, pack.fltPacked( info.second ) );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << newProtoParticles->size() << " ProtoParticles at '" << m_outputName << "'" << endmsg;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
