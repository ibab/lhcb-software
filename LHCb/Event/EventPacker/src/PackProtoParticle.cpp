// $Id: PackProtoParticle.cpp,v 1.3 2008-12-09 08:13:23 ocallot Exp $
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
DECLARE_ALGORITHM_FACTORY( PackProtoParticle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackProtoParticle::PackProtoParticle( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName"  , m_inputName  = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "OutputName" , m_outputName = LHCb::PackedProtoParticleLocation::Charged ); 
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

  LHCb::ProtoParticles* parts = get<LHCb::ProtoParticles>( m_inputName );
  LHCb::PackedProtoParticles* out = new LHCb::PackedProtoParticles();
  put( out, m_outputName );
  out->setVersion( 1 );

  StandardPacker pack;
  
  for ( LHCb::ProtoParticles::const_iterator itP = parts->begin(); parts->end() != itP; ++itP ) {
    LHCb::PackedProtoParticle newPart;
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
    newPart.firstHypo = out->sizeRef();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = part->calo().begin();
          part->calo().end() != itO; ++itO ) {
      int myRef = pack.reference( out, (*itO)->parent(), (*itO)->key() );
      out->addRef( myRef );
    }
    newPart.lastHypo = out->sizeRef();

    //== Handles the ExtraInfo
    newPart.firstExtra = out->sizeExtra();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = part->extraInfo().begin();
          part->extraInfo().end() != itE; ++itE ) {
      out->addExtra( (*itE).first, pack.fltPacked( (*itE).second ) );
    }
    newPart.lastExtra = out->sizeExtra();
    
    out->addEntry( newPart );
  }
  
  // Clear the registry address of the unpacked container, to prevent reloading
  parts->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
}
//=============================================================================
