// $Id: TupleToolGeometry.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolGeometry.h"


#include <Kernel/IGeomDispCalculator.h>
#include <Kernel/IContextTool.h>
#include <Kernel/IPhysDesktop.h>

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"




//-----------------------------------------------------------------------------
// Implementation file for class : GeometryTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolGeometry );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolGeometry::TupleToolGeometry( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_context(0)
  , m_geom(0)
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolGeometry::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_context = tool<IContextTool>( "ContextTool", this );

  if( !m_context ){
    Error("Unable to retrieve the IGeomDispCalculator tool");
    return StatusCode::FAILURE;
  }

  m_geom = m_context->geomTool ();
  if( !m_geom ){
    Error("Unable to retrieve the IGeomDispCalculator tool");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolGeometry::fill( const Particle* mother
				    , const Particle* P
				    , const std::string& head
				    , Tuples::Tuple& tuple ){
  Assert( P && mother && m_geom && m_context
	  , "This should not happen, you are inside TupleToolGeometry.cpp :(" );

  bool test=true;
  
  const VertexBase* primVtx = m_context->desktop()->relatedVertex ( mother );
  if( primVtx ){}
  else { 
    Error("No related primary vertex to compute the IPS"); 
    return StatusCode::FAILURE;
  }
    
  double ip=0, eip=0;
  test &= m_geom->calcImpactPar ( *P, *primVtx, ip, eip );
  if( !test ){
    ip=-1;
    eip=-1;
  }
  test &= tuple->column( head + "_IP", ip );
  // test &= tuple->column( head + "_IPS", ip/eip );
  test &= tuple->column( head + "_IPERR", eip );

  // nothing more for basic particles
  if( P->isBasicParticle() ) return StatusCode(test);
  // --------------------------------------------------

  // --------------------------------------------------
  // cosine of (flight distance) dot (momentum):

  // find the origin vertex. Either the primary or the origin in the
  // decay
  const VertexBase* vtx = NULL;
  if( mother != P ) vtx = originVertex( mother, P );
  else vtx = primVtx;
  if( !vtx ){
    Error("Can't retrieve the origin vertex for " + head );
    return StatusCode::FAILURE;
  }


  const LHCb::Vertex* evtx = P->endVertex();
  if( !evtx ){
    Error("Can't retrieve the end vertex for " + head );
    return StatusCode::FAILURE;
  }
  Gaudi::XYZVector A = P->momentum().Vect();
  Gaudi::XYZVector B = evtx->position() - vtx->position ();  

  double cosPFD = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
  test &= tuple->column( head + "_DIRA", cosPFD );

  // decay vertex information:
  test &= tuple->column( head + "_ENDVERTEX_", evtx->position() );
  const Gaudi::SymMatrix3x3 & m = evtx->covMatrix ();
  test &= tuple->column( head + "_ENDVERTEX_XERR", std::sqrt( m(0,0) ) );
  test &= tuple->column( head + "_ENDVERTEX_YERR", std::sqrt( m(1,1) ) );
  test &= tuple->column( head + "_ENDVERTEX_ZERR", std::sqrt( m(2,2) ) );
  test &= tuple->column( head + "_ENDVERTEX_CHI2", evtx->chi2() );
  test &= tuple->column( head + "_ENDVERTEX_NDOF", evtx->nDoF() );
  
  // --------------------------------------------------
  // flight distance
  double dist=0, edist=0;
  StatusCode sc = m_geom->calcSignedFlightDistance( *vtx, *P, dist, edist );
  if( sc ){}
  else {
    dist = 0;
    edist = 0;
  }
  
  test &= tuple->column( head + "_FD", dist );
  // test &= tuple->column( head + "_FDS", dist/edist );
  test &= tuple->column( head + "_FDERR", edist );

  return StatusCode(test);
}

// =====================================================
// =====================================================

const Vertex* TupleToolGeometry::originVertex( const Particle* top
						 , const Particle* P ) const {
  if( top == P || P->isBasicParticle() ) return NULL;

  const SmartRefVector< LHCb::Particle >& dau = top->daughters ();
  if( dau.empty() ) return NULL;

  SmartRefVector< LHCb::Particle >::const_iterator it;
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P == *it ){ // I found the daughter
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
