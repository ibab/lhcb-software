// $Id: TupleToolTrackInfo.cpp,v 1.1 2008-03-19 20:26:28 sposs Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrackInfo.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2008-03-17 : Stephane Poss
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTrackInfo );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTrackInfo::TupleToolTrackInfo( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolTrackInfo::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  bool test = true;
  if( P ){
    if( P->isBasicParticle() ){
      const LHCb::ProtoParticle* protop = P->proto();
      if(protop){
        const LHCb::Track* track(0);
        track = protop->track();
        if(track){
          test &= tuple->column( head+"_TRACK_CHI2", track->chi2() );
          test &= tuple->column( head+"_TRACK_NDOF", track->nDoF() );
        }
      }
    }
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
