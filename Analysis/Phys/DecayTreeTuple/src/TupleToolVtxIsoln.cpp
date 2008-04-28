// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolVtxIsoln.h"


#include <Kernel/IGeomDispCalculator.h>
#include <Kernel/IContextTool.h>
#include <Kernel/IPhysDesktop.h>

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"




//-----------------------------------------------------------------------------
// Implementation file for class : GeometryTupleTool
//
// @author Mitesh Patel
// @date   2008-04-15
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolVtxIsoln );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolVtxIsoln::TupleToolVtxIsoln( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_context(0) 
  , m_geom(0)
  , m_pVertexFit(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty( "IP", m_IP = 0.0);
  declareProperty( "IPS", m_IPS = 0.0);
  declareProperty( "MaxDeltaChi2", m_deltaChi2 = 0.0);
  declareProperty( "MaxChi2", m_Chi2 = 0.0);
  declareProperty( "VertexFit", m_typeVertexFit = "default");
  declareProperty("InputParticles", m_inputParticles);

}

//=============================================================================

StatusCode TupleToolVtxIsoln::initialize() {
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

  m_pVertexFit= m_context->vertexFitter();
  if( !m_pVertexFit ){
    Error("Unable to retrieve the IVertexFit tool");
    return StatusCode::FAILURE;
  }

  

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolVtxIsoln::fill( const Particle* mother
				    , const Particle* P
				    , const std::string& head
				    , Tuples::Tuple& tuple ){
  Assert( P && mother && m_geom && m_context
	  , "This should not happen, you are inside TupleToolVtxIsoln.cpp :(" );

  bool test=true;

  // --------------------------------------------------
  
  // find the origin vertex. Either the primary or the origin in the
  // decay
  /*
  const VertexBase* vtx = NULL;
  if( mother != P ) vtx = originVertex( mother, P );
  if( !vtx ){
    Error("Can't retrieve the origin vertex for " + head );
    return StatusCode::FAILURE;
  }
  */
  const LHCb::Vertex* vtx;
  if (P->isBasicParticle()){
    vtx = mother->endVertex();
  }
  else{
    vtx = P->endVertex();
  }
  if( !vtx ){
    Error("Can't retrieve the  vertex for " + head );
    return StatusCode::FAILURE;
  }

  // The vertex chi2 of the composite particle being tested
  double vtxChi2 = vtx->chi2();

  //--------------------------------------------------
  // Get all the particle's final states
  LHCb::Particle::ConstVector source;
  LHCb::Particle::ConstVector target;
  LHCb::Particle::ConstVector finalStates;
  
  //   const LHCb::Particle* head = P;
  if (P->isBasicParticle()){
    source.push_back(mother);
  }
  else{
    source.push_back(P);
  }
  // The first iteration is for the particle to filter, which has an endVertex
  do {
    target.clear();
    for(LHCb::Particle::ConstVector::const_iterator isource = source.begin(); 
	isource != source.end(); isource++){
      
      if(!((*isource)->daughters().empty())){
 
	LHCb::Particle::ConstVector tmp = (*isource)->daughtersVector();
 
	for( LHCb::Particle::ConstVector::const_iterator itmp = tmp.begin(); 
	     itmp!=tmp.end(); itmp++){
	  target.push_back(*itmp);
	  
           // Add the final states, i.e. particles with proto and ignoring gammas
	  if((*itmp)->proto() && 22 != (*itmp)->particleID().pid()) finalStates.push_back(*itmp);
	}
      } // if endVertex
     } // isource
    source = target;
  } while(target.size() > 0);
  
  debug() << "Final states size= " <<  finalStates.size()  << endreq;
    
  
  //--------------------------------------------------
  // Build vector of particles, excluding signal
  LHCb::Particle::ConstVector theParts;


  for(std::vector<std::string>::iterator i = m_inputParticles.begin(); 
       i !=m_inputParticles.end(); ++i){
    
    if (!exist<LHCb::Particles>(*i+"/Particles")){
      debug() << "No particles at " << *i << " !!!!!" << endreq;
      continue; 
    }

    LHCb::Particles* parts = get<LHCb::Particles>(*i+"/Particles");
 
    debug() << "Getting particles from " << *i
            << " with " << (*parts).size() << " particles" << endreq;
    
    for(LHCb::Particles::const_iterator iparts = (*parts).begin(); 
        iparts != (*parts).end(); ++iparts){
      
      // Ignore if no proto
      if(!(*iparts)->proto()) continue;
      // Ignore if proto and gammas
      if(22 == (*iparts)->particleID().pid()) continue;
       
      // Compare protos, not pointers (e.g. because of clones)
      bool isSignal = false;
      for(LHCb::Particle::ConstVector::const_iterator signal = finalStates.begin(); 
	  signal != finalStates.end(); ++signal){
	const LHCb::ProtoParticle* orig = (*signal)->proto();
	if(!orig) continue;
	if(orig != (*iparts)->proto()) continue;
	isSignal = true;
	break;
      }
      
      // Ignore if it is a signal particle
      if(isSignal) continue;
      
      // Check that the same particle does not appear twice
      bool isIn = false;
      for(LHCb::Particle::ConstVector::const_iterator result = theParts.begin(); 
	  result != theParts.end(); ++result){
	const LHCb::ProtoParticle* orig = (*result)->proto();
	if(!orig) continue;
	if(orig != (*iparts)->proto()) continue;
	isIn = true;
	break;
      }
      
      // Already in ?
      if(isIn) continue;
      if(!isIn) theParts.push_back(*iparts);
      
    } // iparts
   } // m_inputParticles
  
     debug() << "Number of particles to check excluding signal, particles with same proto and gammas = "
             << theParts.size() << endreq;
     //--------------------------------------------------
     
     /*
      *  if basic particle then final states now contains list of particles associated with mother particle - 
      *  have excluded these when building list of particles to try putting into vertex, now need to build 
      * vertex- remove everything but basic particle from final states
      */
     
     if (P->isBasicParticle()){
       finalStates.clear();
       finalStates.push_back(P);
       debug() << "Final states mod size= " <<  finalStates.size()  << endreq;       
     }

     int nCompatibleIP = 0;
     int nCompatibleIPS = 0;
     int nCompatibleDeltaChi2 = 0;
     int  nCompatibleChi2= 0;

     // Now count how many particles are compatible with the vertex of the particle under study
     for(LHCb::Particle::ConstVector::const_iterator iparts = theParts.begin(); 
	 iparts != theParts.end(); ++iparts){
       
       // Compute IP only if cuts are applied
       if(m_IP>0.0 || m_IPS>0.0){
	 double ip = -1.0 ,ipe = -1.0;
	 StatusCode sc = m_geom->calcImpactPar(**iparts, *vtx, ip, ipe);
	 if (!sc.isSuccess()) continue;
	 double ips = fabs(ip/ipe);
	  verbose() << "IP is " << ip << "+/-" << ipe << " mm (" << ips
		    << "sigma)" << endreq;
	  // Count
	  if(ip < m_IP && m_IP > 0.0) nCompatibleIP++;
	  if(ips < m_IPS && m_IPS > 0.0) nCompatibleIPS++;
       } // if IP cuts
       
       // Compute chi2 if cut is applied
       if(m_deltaChi2>0.0||m_Chi2>0.0){
        
	 StatusCode sc = StatusCode::SUCCESS;
	 LHCb::Vertex vtxWithExtraTrack;
	 
	 // Temporarily add the extra track to the finalStates
	 finalStates.push_back(*iparts);
	 // debug() << "Adding trk pid"  << (*iparts)->particleID().pid() << " to vtx" << endmsg;
	 
	 // Fit
	 sc = m_pVertexFit->fit(finalStates,vtxWithExtraTrack);
	 // Remove the added track from finalStates
	 finalStates.pop_back();
	 if (!sc){
	   Warning("Failed to fit vertex");
	   // return sc;
	 }
	 else{
	   if(fabs(vtxChi2 - vtxWithExtraTrack.chi2())  < m_deltaChi2 && m_deltaChi2 > 0.0) nCompatibleDeltaChi2++;
	   if(vtxWithExtraTrack.chi2()  < m_Chi2 && m_Chi2 > 0.0) nCompatibleChi2++;
	   debug() << "Fitted vertex adding track has Delta chi2 = " 
             << fabs(vtxChi2 - vtxWithExtraTrack.chi2())  << "chi2 = " << vtxWithExtraTrack.chi2() << endmsg;
	 }
	 	 
       } // if delta chi2 or chi2 cut applied


      
     } // iparts
     
     if(m_IP > 0.0) debug() << "Number of particles with IP < cut IP = " << nCompatibleIP << endreq;
     if(m_IPS > 0.0)  debug() << "Number of particles with IPS < cut IPS = " 
			      << nCompatibleIPS << endreq;
     if(m_deltaChi2 > 0.0) debug() << "Number of particles with delta chi2 < cut Delta chi2 = " 
				   << nCompatibleDeltaChi2 << endreq;
     if(m_Chi2 > 0.0) debug() << "Number of particles with  chi2 < cut  chi2 = " 
				   << nCompatibleChi2 << endreq;
     
     if(m_IP > 0.0) test &= tuple->column( head + "_NOPARTWITHINIPWDW", nCompatibleIP );
     if(m_IPS > 0.0) test &= tuple->column( head + "_NOPARTWITHINIPSWDW", nCompatibleIPS );
     if(m_deltaChi2 > 0.0) test &= tuple->column( head + "_NOPARTWITHINDCHI2WDW",  nCompatibleDeltaChi2);
     if(m_Chi2 > 0.0) test &= tuple->column( head + "_NOPARTWITHINCHI2WDW",  nCompatibleChi2);
     
  return StatusCode(test);
}

// =====================================================
// =====================================================

const Vertex* TupleToolVtxIsoln::originVertex( const Particle* top
						 , const Particle* P ) const {
  if( top == P || P->isBasicParticle() ) return NULL;

  const SmartRefVector< LHCb::Particle >& dau = top->daughters ();
  if( dau.empty() ){
    // debug() << " Particle has no daughters! "  << endreq;
    return NULL;
  }

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
