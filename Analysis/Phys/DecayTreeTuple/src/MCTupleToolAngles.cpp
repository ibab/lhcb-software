// $Id: MCTupleToolAngles.cpp,v 1.2 2009-01-23 10:28:10 pkoppenb Exp $
// Include files 
#include "gsl/gsl_sys.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "MCTupleToolAngles.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/MCParticle.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolAngles
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolAngles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolAngles::MCTupleToolAngles( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMCParticleTupleTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolAngles::~MCTupleToolAngles() {} 

//=============================================================================
// initialize
//=============================================================================

StatusCode MCTupleToolAngles::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolAngles::fill( const LHCb::MCParticle* mother
                                     , const LHCb::MCParticle* mcp
                                     , const std::string& head
                                     , Tuples::Tuple& tuple ){
  
  bool test = true;

  if ( 0==mcp || 0==mother || mcp==mother ) return StatusCode::SUCCESS ;
  double cosT = cosTheta(mother->momentum(), mcp->momentum() );
  // fill the tuple:
  test &= tuple->column( head+"_TRUECosTheta", cosT );
  if ( msgLevel(MSG::DEBUG)) debug() << mother->particleID().pid() << " " << mother->momentum() << " " 
                                     << mcp->particleID().pid() << " " << mcp->momentum() << endmsg ;
  return StatusCode(test) ;
  
}
