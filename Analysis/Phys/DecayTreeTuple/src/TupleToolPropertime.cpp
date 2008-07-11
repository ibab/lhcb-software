// $Id: TupleToolPropertime.cpp,v 1.3 2008-07-11 09:21:04 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartIF.h"

#include <Kernel/IContextTool.h>
#include <Kernel/IPhysDesktop.h>
#include <Kernel/ILifetimeFitter.h>

// local
#include "TupleToolPropertime.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolPropertime );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolPropertime::TupleToolPropertime( const std::string& type,
					  const std::string& name,
					  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_context(0)
  , m_fit(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("ToolName", m_toolName = "PropertimeFitter" );

}//=============================================================================

StatusCode TupleToolPropertime::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_context = tool<IContextTool>( "ContextTool", this );
  m_fit = tool<ILifetimeFitter>( m_toolName, this );
  if( !m_fit ){
    Error("Unable to retrieve the ILifetimeFitter tool");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolPropertime::fill( const Particle* mother
				   , const Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){

  Assert( m_fit && P,
	  "Should not happen, you are inside TupleToolPropertime.cpp" );

  // no proper-time for basic parts.
  if( P->isBasicParticle() ) return StatusCode::SUCCESS; 

  const VertexBase* originVtx = NULL;
  if( mother != P ){
    originVtx = originVertex( mother, P ); // the origin vertex is 
                                           // somewhere in the decay
  } else { // the origin vertex is the primary.
    originVtx = m_context->desktop()->relatedVertex ( mother );
  }

  if( originVtx ){} // I'm happy
  else {
    Error("Can't get the origin vertex");
    return StatusCode::FAILURE;
  }

  double pt   = -100;
  double ept  = -100;
  double chi2 = -100;
  StatusCode sc = m_fit->fit ( *originVtx, *P , pt, ept, chi2 );

  if( !sc ){
    Warning("The propertime fit failed").ignore();
    pt   = -100;
    ept  = -100;
    chi2 = -100;
  }

  bool test = true;
  test &= tuple->column( head+"_TAU" , pt/Gaudi::Units::picosecond );
  test &= tuple->column( head+"_TAUERR" , ept/Gaudi::Units::picosecond );
  test &= tuple->column( head+"_TAUCHI2" , chi2 );
  
  return StatusCode(test);

}

const Vertex* TupleToolPropertime::originVertex( const Particle* top
						 , const Particle* P ) const {
  if( top == P || P->isBasicParticle() ) return NULL;

  const SmartRefVector< LHCb::Particle >& dau = top->daughters ();
  if( dau.empty() ) return NULL;

  SmartRefVector< LHCb::Particle >::const_iterator it;
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P == *it ){
      return top->endVertex();
    }
  }
  
  // vertex not yet found, get deeper in the decay:
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P != *it && !(*it)->isBasicParticle() ){
      const Vertex* vv = originVertex( *it, P );
      if( vv ) return vv;
    }
  }
  return NULL;
}
