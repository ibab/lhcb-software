// $Id: TupleToolTutorial.cpp,v 1.1 2009-06-11 16:22:29 rlambert Exp $
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
  //declare properties here!
}

//=============================================================================

StatusCode TupleToolTutorial::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){

  //the fill method is called once per input particle
  //fill some information about the particle here!

  return StatusCode::FAILURE;
}
