// $Id: TupleToolPid.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolPid.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h" 

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolPid );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolPid::TupleToolPid( const std::string& type,
				      const std::string& name,
				      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolPid::fill( const Particle* 
				    , const Particle* P
				    , const std::string& head
				    , Tuples::Tuple& tuple ){

  if( P ){
    bool test = true;
    test &= tuple->column( head+"_ID", P->particleID().pid() );

    if( !P->isBasicParticle() ) return StatusCode(test); // no rich info for composite!

    const ProtoParticle* proto = P->proto();
    if( proto ){
      test &= tuple->column( head+"_PIDe"
			     ,proto->info(LHCb::ProtoParticle::CombDLLe,-1000));
      
      test &= tuple->column( head+"_PIDmu"
			     ,proto->info(LHCb::ProtoParticle::CombDLLmu,-1000));
      
      test &= tuple->column( head+"_PIDK"
			     ,proto->info(LHCb::ProtoParticle::CombDLLk,-1000));
      test &= tuple->column( head+"_PIDp"
			     ,proto->info(LHCb::ProtoParticle::CombDLLp,-1000));
      // uncomment it if one days set to something non zero..
      //       test &= tuple->column( head+"_PIDpi"
      // 			     ,proto->info(LHCb::ProtoParticle::CombDLLpi,-1000));
      return StatusCode(test);
    }
  }
  return StatusCode::FAILURE;
}
