// $Id: TupleToolDalitz.cpp,v 1.2 2009-02-18 18:34:55 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Particle.h" 

// local
#include "TupleToolDalitz.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDalitz
//
// 2009-02-17 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDalitz );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolDalitz::TupleToolDalitz( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  m_ppSvc(0)
{
  declareInterface<IParticleTupleTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolDalitz::~TupleToolDalitz() {} 

//=============================================================================
//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolDalitz::fill( const LHCb::Particle* mother
                                  , const LHCb::Particle* part
                                  , const std::string& head
                                  , Tuples::Tuple& tuple ){
  
  if (0==part) return StatusCode::FAILURE ;
  const LHCb::Particle::ConstVector	dauts = part->daughtersVector() ;
  if ( 2>=dauts.size() ){
    return Warning("Will not fill Dalitz of two body decay "+head,StatusCode::SUCCESS,0);
  }

  return fill(dauts,"",tuple) ;
  
}
