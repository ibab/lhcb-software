// $Id: PackProtoParticle.cpp,v 1.7 2009-11-10 10:25:07 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/ProtoParticle.h"
#include "Event/PackedProtoParticle.h"
#include "Kernel/StandardPacker.h"

// local
#include "PackProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackProtoParticle
//
// 2008-11-13 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackProtoParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackProtoParticle::PackProtoParticle( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName"  , m_inputName  = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "OutputName" , m_outputName = LHCb::PackedProtoParticleLocation::Charged ); 
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
PackProtoParticle::~PackProtoParticle() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackProtoParticle::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::ProtoParticles>(m_inputName) ) return StatusCode::SUCCESS;

  LHCb::ProtoParticles* parts = getOrCreate<LHCb::ProtoParticles,LHCb::ProtoParticles>( m_inputName );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << parts->size() << " ProtoParticles at '" << m_inputName << "'" << endmsg;

  LHCb::PackedProtoParticles* out = new LHCb::PackedProtoParticles();
  out->protos().reserve(parts->size());
  put( out, m_outputName );
  out->setVersion( 1 );

  StandardPacker pack;
  
  for ( LHCb::ProtoParticles::const_iterator itP = parts->begin(); parts->end() != itP; ++itP )
  {

    out->protos().push_back( LHCb::PackedProtoParticle() );
    LHCb::PackedProtoParticle & newPart = out->protos().back();
    LHCb::ProtoParticle* part = *itP;
    
    newPart.key      = part->key();
    if ( 0 != part->track() ) {
      newPart.track = pack.reference( out, part->track()->parent(), part->track()->key() );
    } else {
      newPart.track = -1;
    }

    if ( 0 != part->richPID() ) {
      newPart.richPID = pack.reference( out, part->richPID()->parent(), part->richPID()->key() );
    } else {
      newPart.richPID = -1;
    }

    if ( 0 != part->muonPID() ) {
      newPart.muonPID = pack.reference( out, part->muonPID()->parent(), part->muonPID()->key() );
    } else {
      newPart.muonPID = -1;
    }
    
    //== Store the CaloHypos
    newPart.firstHypo = out->refs().size();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = part->calo().begin();
          part->calo().end() != itO; ++itO ) {
      int myRef = pack.reference( out, (*itO)->parent(), (*itO)->key() );
      out->refs().push_back( myRef );
    }
    newPart.lastHypo = out->refs().size();

    //== Handles the ExtraInfo
    newPart.firstExtra = out->extras().size();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = part->extraInfo().begin();
          part->extraInfo().end() != itE; ++itE ) {
      out->extras().push_back( std::pair<int,int>((*itE).first, pack.fltPacked((*itE).second)) );
    }
    newPart.lastExtra = out->extras().size();
    
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << out->protos().size() << " PackedProtoParticles at '" 
            << m_outputName << "'" << endmsg;
  
  // Clear the registry address of the unpacked container, to prevent reloading
  parts->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
}
//=============================================================================
