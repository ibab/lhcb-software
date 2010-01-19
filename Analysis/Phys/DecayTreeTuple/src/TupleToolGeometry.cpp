// $Id: TupleToolGeometry.cpp,v 1.15 2010-01-19 14:12:32 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolGeometry.h"

#include <Kernel/DVAlgorithm.h>
#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>

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
  : 
  GaudiTool ( type, name , parent )
  , m_dist(0)
  , m_photonID(22)
  , m_pi0ID(111)
  , m_fillMother(true)
  , m_dva(0)
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("FillMother",m_fillMother=true,
                  "Turn false if the mother is expected to be NULL, will not fill mother PV info");
  
}

//=============================================================================

StatusCode TupleToolGeometry::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);  

  m_dist = m_dva->distanceCalculator ();
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
  Assert( P && (mother || !m_fillMother) && m_dist && m_dva
          , "No mother or particle, or tools misconfigured. Try setting TupleToolGeometry.FillMother=False." );
  
  if( P->particleID().pid() == m_photonID ||  P->particleID().pid()  == m_pi0ID  ){
    return Warning("Will not fill geometry tuple for photons and pi0. No worry.", StatusCode::SUCCESS, 10);
  }

  verbose() << "TupleToolGeometry::fill " << mother << " " << P << " " << head << endmsg ;
  
  //fill min IP
  StatusCode sc = fillMinIP(P,head,tuple);
  if (!sc) return sc;
  
  const VertexBase* aPV = NULL;
  
  //=========================================================================
  // fill IP for Particles's Own BPV.. if it isn't the mother!
  //=========================================================================
  if ( !m_fillMother || !mother || mother != P )
  {
    aPV = m_dva->bestPV ( P );
    sc = fillVertexFull(aPV,P,head,"_OWNPV",tuple);
    if (!sc) return sc;
  }
  //=========================================================================
  // fill IP for head of chain's own BPV
  //=========================================================================
  if ( mother && m_fillMother )
  {
    aPV = m_dva->bestPV ( mother );
    sc = fillVertexFull(aPV,P,head,"_PV",tuple);
    if (!sc) return sc;
  }
  //=========================================================================
  // fill IP wrt Mother's decay vertex, if it isn't the mother!
  //=========================================================================
  if ( mother && m_fillMother && mother != P)
  {
    aPV = originVertex( mother, P );
    sc = fillVertexFull(aPV,P,head,"_ORIVX",tuple);
    if (!sc) return sc;
  }
  //=========================================================================
  // nothing more for basic particles
  if( P->isBasicParticle() ) return sc ;
  //=========================================================================
  
  //=========================================================================
  //fill end vertex info
  //=========================================================================
  const VertexBase* evtx = P->endVertex();
  if( 0==evtx ) return Error("Can't retrieve the end vertex for " + head );
  // end vertex 
  sc = fillVertex(evtx,head+"_ENDVERTEX",tuple);

  return sc ;
  
}
//=========================================================================
//  Fill Everything for this vertex for related PV 
//=========================================================================
StatusCode TupleToolGeometry::fillVertexFull(const LHCb::VertexBase* vtx, 
				      const LHCb::Particle* P, 
				      std::string head, std::string vtx_name, 
				      Tuples::Tuple& tuple) const 
{
    if( 0==vtx ) counter("Can't retrieve the " +vtx_name+ " vertex for " + head )++;
    StatusCode sc = fillVertex(vtx,head+vtx_name,tuple);
    if (!sc) return sc;
    sc = fillBPV(vtx,P,head,tuple,vtx_name);
    if (!sc) return sc;
    if( !P->isBasicParticle() ) sc = fillFlight(vtx,P,head,tuple,vtx_name);
    return sc;

}

//=========================================================================
//  Fill PV for related PV 
//=========================================================================
StatusCode TupleToolGeometry::fillBPV( const VertexBase* primVtx
                                    , const Particle* P
                                    , const std::string head
                                    , Tuples::Tuple& tuple
                                    , std::string trail) const {
  bool test = true ;
  
  double ip=0, chi2=0;
  if ( 0==primVtx ){
    counter("No BPV for "+head)++;
    test &= tuple->column( head + "_IP"+trail, -999. );
    test &= tuple->column( head + "_IPCHI2"+trail, -999. );
  } else {
    test &= m_dist->distance ( P, primVtx, ip, chi2 );
    if( !test ){
      ip=-1;
      chi2=-1;
    }
    test &= tuple->column( head + "_IP"+trail, ip );
    test &= tuple->column( head + "_IPCHI2"+trail, chi2 );
  }
  
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
  const RecVertex::Container* PV = m_dva->primaryVertices();
  if ( 0!=PV ){
    verbose() << "Filling IP " << head + "_MINIP : " << P << " PVs:" << PV->size() << endmsg ;
    for ( RecVertex::Container::const_iterator pv = PV->begin() ; pv!=PV->end() ; ++pv){
      double ip, chi2;
      StatusCode test2 = m_dist->distance ( P, *pv, ip, chi2 );
      if( test2 ) {
        if ((ip<ipmin) || (ipmin<0.)) ipmin = ip ;
        if ((chi2<minchi2) || (minchi2<0.)) minchi2 = chi2 ;
      }
    }
  }
  verbose() << "Filling IP " << head + "_MINIP " << ipmin << " at " << minchi2 << endmsg  ;
  test &= tuple->column( head + "_MINIP", ipmin );
  test &= tuple->column( head + "_MINIPCHI2", minchi2 );
  
  // --------------------------------------------------
  return StatusCode(test) ;
}
//=========================================================================
// fill vertex stuff 
//=========================================================================
StatusCode TupleToolGeometry::fillVertex( const LHCb::VertexBase* vtx
                                          , std::string vtx_name
                                          , Tuples::Tuple& tuple ) const {
  bool test = true ;
  
  // decay vertex information:
  if ( 0==vtx){
    Gaudi::XYZPoint pt(-999.,-999.,-999.) ; // arbitrary point
    Gaudi::SymMatrix3x3 tmp; // arbitrary matrix
    test &= tuple->column(  vtx_name+"_", pt );
    test &= tuple->column(  vtx_name + "_XERR", -999. );
    test &= tuple->column(  vtx_name + "_YERR", -999.  );
    test &= tuple->column(  vtx_name + "_ZERR", -999. );
    test &= tuple->column(  vtx_name + "_CHI2", -999. );
    test &= tuple->column(  vtx_name + "_NDOF", -1 );
    test &= tuple->matrix(  vtx_name + "_COV_", tmp  );
  } else { 
    test &= tuple->column( vtx_name+"_", vtx->position() );
    const Gaudi::SymMatrix3x3 & m = vtx->covMatrix ();
    test &= tuple->column(  vtx_name + "_XERR", std::sqrt( m(0,0) ) );
    test &= tuple->column(  vtx_name + "_YERR", std::sqrt( m(1,1) ) );
    test &= tuple->column(  vtx_name + "_ZERR", std::sqrt( m(2,2) ) );
    test &= tuple->column(  vtx_name + "_CHI2", vtx->chi2() );
    test &= tuple->column(  vtx_name + "_NDOF", vtx->nDoF() );
    test &= tuple->matrix(  vtx_name + "_COV_", m );
  }
  
  // --------------------------------------------------
  return StatusCode(test) ;

}
//=========================================================================
// fill flight distance, angle...
//=========================================================================
StatusCode TupleToolGeometry::fillFlight( const VertexBase* oriVtx
                                          , const Particle* P
                                          , const std::string head
                                          , Tuples::Tuple& tuple
                                          , std::string trail ) const {
  bool test = true ;
  // --------------------------------------------------
  if ( 0==oriVtx ){
    test &= tuple->column( head + "_FD"+trail, -999. );
    test &= tuple->column( head + "_FDCHI2"+trail, -999. );
    test &= tuple->column( head + "_DIRA"+trail, -999.);    
  } else {
   
    // flight distance
    double dist = 0;
    double chi2 = 0 ;
    StatusCode sc = m_dist->distance( oriVtx, P->endVertex(), dist, chi2 );
    if (!sc) return sc ;
    
    test &= tuple->column( head + "_FD"+trail, dist );
    test &= tuple->column( head + "_FDCHI2"+trail, chi2 );
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
    Gaudi::XYZVector B = evtx->position() - oriVtx->position ();  
    
    double cosPFD = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
    test &= tuple->column( head + "_DIRA"+trail, cosPFD );
  }
  
  return StatusCode(test);
}
// =====================================================
// find origin vertex in the decay chain
// =====================================================
const VertexBase* TupleToolGeometry::originVertex( const Particle* top
						 , const Particle* P ) const 
{
  //this used to pass back zero if P was a basic particle.
  //I don't think that's necessary. R Lambert 2009-08-14
  if( top == P || top->isBasicParticle() ) return 0;

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
