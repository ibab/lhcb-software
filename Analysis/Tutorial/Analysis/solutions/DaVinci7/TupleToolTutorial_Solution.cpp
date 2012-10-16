// $Id: TupleToolTutorial_Solution.cpp,v 1.1 2009-06-11 16:22:29 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTutorial.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTutorial
//
// 2009-06-09 : Rob Lambert
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTutorial );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTutorial::TupleToolTutorial( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("absCharge", m_abs=false);
  
}

//=============================================================================

StatusCode TupleToolTutorial::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){

  //fill some information about the particle here!
  bool test = true;
  if( P ){
    //fill some information here!
    if(m_abs) test &= tuple->column( head+"_Q", fabs(P->charge()) );
    else test &= tuple->column( head+"_Q", P->charge() );
    test &= tuple->column( head+"_eta", P->momentum().eta() );

  }
  else 
  {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
