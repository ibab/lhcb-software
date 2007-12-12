// $Id: TupleToolKinetic.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolKinetic.h"

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
DECLARE_TOOL_FACTORY( TupleToolKinetic );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolKinetic::TupleToolKinetic( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolKinetic::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  if( P ){
    if( ! tuple->column( head+"_P", P->p() ) ) return StatusCode::FAILURE;
    if( ! tuple->column( head+"_PT", P->pt() ) ) return StatusCode::FAILURE;

    // momentum components
    if( ! tuple->column( head+"_P", P->momentum() ) ) return StatusCode::FAILURE;
    // reference point:
    if( ! tuple->column( head+"_REFP", P->referencePoint() ) ) return StatusCode::FAILURE;

    // mass
    //     if( P->isBasicParticle() ){
    //       if( ! tuple->column( head+"_M", P->measuredMass() ) ) return StatusCode::FAILURE;
    //     } else {
    if( ! tuple->column( head+"_MM", P->measuredMass() ) ) return StatusCode::FAILURE;
    if( ! tuple->column( head+"_MMERR", P->measuredMassErr() ) ) return StatusCode::FAILURE;
    //    }
    return StatusCode::SUCCESS;

  }
  return StatusCode::FAILURE;
}
