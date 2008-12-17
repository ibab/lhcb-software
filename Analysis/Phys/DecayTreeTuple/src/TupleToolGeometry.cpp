// $Id: TupleToolGeometry.cpp,v 1.7 2008-12-17 20:14:55 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolGeometry.h"


#include <Kernel/IDistanceCalculator.h>
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
  , m_dist(0)
  ,m_photonID(22)
  ,m_pi0ID(111)
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolGeometry::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  
  m_context = tool<IContextTool>( "ContextTool", this );

  if( !m_context ){
    Error("Unable to retrieve the IContext tool");
    return StatusCode::FAILURE;
  }

  m_dist = m_context->distanceTool ();
  if( !m_dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TupleToolGeometry::fill( const Particle* mother
                                    , const Particle* P
                                    , const std::string& head
                                    , Tuples::Tuple& tuple ){
  Assert( P && mother && m_dist && m_context
          , "This should not happen, you are inside TupleToolGeometry.cpp :(" );
  
  if( P->particleID().pid() == m_photonID ||  P->particleID().pid()  == m_pi0ID  ){
    return Warning("Will not fill geometry tuple for photons and pi0. No worry.", StatusCode::SUCCESS, 10);
  }
  
  StatusCode sc = fillMinIP(P,head,tuple);
  if (!sc) return sc;
 
  // fill with mother PV
  const VertexBase* motherPV = m_context->desktop()->relatedVertex ( mother );
  sc = fillBPV(motherPV,P,head,tuple);
  if (!sc) return sc;
  
  // fill with particle's best PV
  if ( mother != P ){
    const VertexBase* myPV = m_context->desktop()->relatedVertex ( P );
    sc = fillBPV(myPV,P,head+"_OwnPV",tuple);
    if (!sc) return sc;
  }
  // nothing more for basic particles
  if( P->isBasicParticle() ) return sc ;
  
  const VertexBase* vtx = 0;
  if ( mother != P ) vtx = originVertex( mother, P );
  else vtx = motherPV;
  if( !vtx ){
    Error("Can't retrieve the origin vertex for " + head );
    return StatusCode::FAILURE;
  }
  sc = fillEndVertex(vtx,motherPV,P,head,tuple);
  return sc ;
}
//=========================================================================
//  Fill PV for related PV 
//=========================================================================
StatusCode TupleToolGeometry::fillBPV( const VertexBase* primVtx
                                    , const Particle* P
                                    , const std::string head
                                    , Tuples::Tuple& tuple ) const {
  bool test = true ;
  if( 0==primVtx ){ 
    Error("No related primary vertex to compute the IP of "+head); 
    return StatusCode::FAILURE;
  }
  
  double ip=0, chi2=0;
  test &= m_dist->distance ( P, primVtx, ip, chi2 );
  if( !test ){
    ip=-1;
    chi2=-1;
  }
  test &= tuple->column( head + "_IP", ip );
  test &= tuple->column( head + "_IPCHI2", chi2 );

  return StatusCode(test) ;
}
//=========================================================================
//  Fill PV for all PV
//=========================================================================
StatusCode TupleToolGeometry::fillMinIP( const Particle* P
                                         , const std::string head
                                         , Tuples::Tuple& tuple ) const {
  bool test = true ;
  // minimum IP
  double ipmin = -1;
  double minchi2 = -1 ;
  const RecVertex::Container* PV = m_context->primaryVertices();
  for ( RecVertex::Container::const_iterator pv = PV->begin() ; pv!=PV->end() ; ++pv){
    double ip, chi2;
    StatusCode test2 = m_dist->distance ( P, *pv, ip, chi2 );
    if( test2 ) {
      if ((ip<ipmin) || (ipmin<0.)) ipmin = ip ;
      if ((chi2<minchi2) || (minchi2<0.)) minchi2 = chi2 ;
    }
  }
  test &= tuple->column( head + "_MINIP", ipmin );
  test &= tuple->column( head + "_MINIPCHI2", minchi2 );
  
  // --------------------------------------------------
  return StatusCode(test) ;
}
//=========================================================================
// fill origin vertex stuff 
//=========================================================================
StatusCode TupleToolGeometry::fillEndVertex( const VertexBase* vtx
                                          , const VertexBase* primVtx
                                          , const Particle* P
                                          , const std::string head
                                          , Tuples::Tuple& tuple ) const {
  bool test = true ;
  // --------------------------------------------------
  // cosine of (flight distance) dot (momentum):
  // find the origin vertex. Either the primary or the origin in the
  // decay
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
  double dist = 0;
  double chi2 = 0 ;
  StatusCode sc = m_dist->distance( vtx, P->endVertex(), dist, chi2 );
  if( sc ){}
  else {
    dist = 0;
    chi2 = 0;
  }
  
  test &= tuple->column( head + "_FD", dist );
  // test &= tuple->column( head + "_FDS", dist/edist );
  test &= tuple->column( head + "_FDCHI2", chi2 );

  sc = m_dist->distance( primVtx, P->endVertex(), dist, chi2 );
  if( sc ){}
  else {
    dist = 0;
    chi2 = 0;
  }

  test &= tuple->column( head + "_FDPV", dist );
  //test &= tuple->column( head + "_FDPVS", dist/edist );
  test &= tuple->column( head + "_FDPVCHI2", chi2 );
  return StatusCode(test);
}
// =====================================================
// =====================================================
const VertexBase* TupleToolGeometry::originVertex( const Particle* top
						 , const Particle* P ) const {
  if( top == P || P->isBasicParticle() ) return 0;

  const SmartRefVector< LHCb::Particle >& dau = top->daughters ();
  if( dau.empty() ) return 0;

  SmartRefVector< LHCb::Particle >::const_iterator it;
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P == *it ){ // I found the daughter
      return top->endVertex();
    }
  }
  
  // vertex not yet found, get deeper in the decay:
  for( it = dau.begin(); dau.end()!=it; ++it ){
    if( P != *it && !(*it)->isBasicParticle() ){
      const VertexBase* vv = originVertex( *it, P );
      if( vv ) return vv;
    }
  }
  return 0;
}
