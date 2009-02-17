// $Id: TupleToolAngles.cpp,v 1.3 2009-02-17 18:07:31 pkoppenb Exp $
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
                                     , const LHCb::Particle* part
                                     , const std::string& head
                                     , Tuples::Tuple& tuple ){
  
  bool test = true;

  if ( 0==part || 0==mother || part==mother ) return StatusCode::SUCCESS ;
  double cosT = cosTheta(mother->momentum(), part->momentum() );
  // fill the tuple:
  test &= tuple->column( head+"_CosTheta", cosT );
  if ( msgLevel(MSG::DEBUG)) debug() << mother->particleID().pid() << " " << mother->momentum() << " " 
                                     << part->particleID().pid() << " " << part->momentum() << endmsg ;
  return StatusCode(test) ;
  
}
