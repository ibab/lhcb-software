// $Id: TupleToolP2VV.cpp,v 1.1 2007-12-18 18:12:26 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Particle.h"     
// kernel
#include "Kernel/IP2VVPartAngleCalculator.h"            // Interface

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// local
#include "TupleToolP2VV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolP2VV
//
// 2007-12-18 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolP2VV );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolP2VV::TupleToolP2VV( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
, m_pB2LLKstar()
{
  declareInterface<IParticleTupleTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolP2VV::~TupleToolP2VV() {} 

//=============================================================================

//=========================================================================
//  initialize
//=========================================================================
StatusCode TupleToolP2VV::initialize( ) {
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_pB2LLKstar  = tool<IP2VVPartAngleCalculator>("Bd2KstarMuMuAngleCalculator",this);
  return sc;
}

//=========================================================================
//  Fill
//=========================================================================
StatusCode TupleToolP2VV::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  bool test = true;
  if( P ){
    double thetaL, thetaK, phi ;
    StatusCode sc = m_pB2LLKstar->calculateAngles( P, thetaL, thetaK, phi );
    if (msgLevel(MSGG:DEBUG)) debug() << "Three angles are theta_L : " 
                                      << thetaL << " K: " << thetaK
                                      << " phi: " << phi << endmsg ;
    if ( !sc ) return sc;
    test &= tuple->column( head+"_ThetaL", thetaL );
    test &= tuple->column( head+"_ThetaK", thetaK );
    test &= tuple->column( head+"_Phi",    phi  );
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
