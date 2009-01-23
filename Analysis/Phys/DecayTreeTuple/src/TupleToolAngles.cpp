// $Id: TupleToolAngles.cpp,v 1.1 2009-01-23 10:54:36 pkoppenb Exp $
// Include files 
#include "gsl/gsl_sys.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "TupleToolAngles.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolAngles
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolAngles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolAngles::TupleToolAngles( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolAngles::~TupleToolAngles() {} 

//=============================================================================
// initialize
//=============================================================================

StatusCode TupleToolAngles::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolAngles::fill( const LHCb::Particle* mother
                                     , const LHCb::Particle* mcp
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
