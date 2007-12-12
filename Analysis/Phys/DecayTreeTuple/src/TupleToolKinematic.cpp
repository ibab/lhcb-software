// $Id: TupleToolKinematic.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolKinematic.h"

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
DECLARE_TOOL_FACTORY( TupleToolKinematic );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolKinematic::TupleToolKinematic( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolKinematic::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  bool test = true;
  if( P ){
    test &= tuple->column( head+"_P", P->p() );
    test &= tuple->column( head+"_PT", P->pt() );

    // momentum components
    test &= tuple->column( head+"_P", P->momentum() );
    // reference point:
    test &= tuple->column( head+"_REFP", P->referencePoint() );

    test &= tuple->column( head+"_MM", P->measuredMass() );
    if( !P->isBasicParticle() )
      test &= tuple->column( head+"_MMERR", P->measuredMassErr() );
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
