// $Id: ParticleStuffer.cpp,v 1.5 2006-04-04 06:37:04 jpalac Exp $
// Include files 

// 
#include <math.h>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "ParticleStuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleStuffer
//
// 15/03/2002 : Paul Colrain
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParticleStuffer>          s_factory ;
const        IToolFactory& ParticleStufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleStuffer::ParticleStuffer( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ppSvc(0)
  , m_pTransporter(0) 
  , m_transporterType("ParticleTransporter"){

  // declare additional Interface
  declareInterface<IParticleStuffer>(this);
  declareProperty("Transporter", m_transporterType);
}

//=============================================================================

StatusCode ParticleStuffer::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  
  // This tool needs to use internally the ParticlePropertySvc to retrieve the
  // mass to be used
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc");
  m_pTransporter = tool<IParticleTransporter>(m_transporterType, this);

  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Stuffer
//=============================================================================
StatusCode ParticleStuffer::fillParticle( const LHCb::Particle::ConstVector& daughters,
                                          const LHCb::Vertex& vtx, 
                                          const LHCb::ParticleID& pid,
                                          LHCb::Particle& part){
  part.setParticleID(pid);
  return fillParticle(daughters,vtx,part);
}
//=============================================================================
// Stuffer
//=============================================================================
StatusCode ParticleStuffer::fillParticle( const LHCb::Particle::ConstVector& daughters,
                                          const LHCb::Vertex& vtx, 
                                          LHCb::Particle& part){

  double zVtxPos = vtx.position().z();
  // Set the four-momenta
  Gaudi::XYZTVector MotherlorVec ;
  Gaudi::SymMatrix4x4 MothermeMat ; 

  LHCb::Particle transParticle;
  for (LHCb::Particle::ConstVector::const_iterator it = daughters.begin(); 
       it != daughters.end(); ++it ) {

    // Transport Particle parameters to the vertex position	 
    StatusCode sctrans = m_pTransporter->transport((*it),zVtxPos,transParticle);
    if ( !sctrans.isSuccess() ) {
      debug() << "Particle extrapolation failed" << endmsg;
      return sctrans;
    }
    MotherlorVec += transParticle.momentum(); 
    // Set the four-momentum error matrix.
    MothermeMat += transParticle.momCovMatrix();    
  }
  part.setMomentum( MotherlorVec );
  part.setMomCovMatrix( MothermeMat );

  // Set the measured mass.
  part.setMeasuredMass( part.momentum().mag() ); 

  // Set the error on the measured mass.
  typedef ROOT::Math::SMatrix<double, 1, 4> Matrix1x4;
  Matrix1x4 derivs;
  derivs(0,0) = - part.momentum().X() / part.measuredMass();
  derivs(0,1) = - part.momentum().Y() / part.measuredMass();
  derivs(0,2) = - part.momentum().Z() / part.measuredMass();
  derivs(0,3) =   part.momentum().E() / part.measuredMass();
  //  massErrSqd = part.momentumErr().similarity( derivs );  

  // Stolen and adapted from SHacks
  /// @todo Check that this is correct
  Gaudi::SymMatrix1x1 massErrSqd = 
    ROOT::Math::Similarity<double,1,4>(derivs,part.momCovMatrix());
  double massErr = sqrt( massErrSqd(1,1) );
  part.setMeasuredMassErr( massErr ); 

  // Set the point on track.
  part.setReferencePoint( vtx.position() ); 

  // Set the point on track error matrix.
  part.setPosCovMatrix( vtx.covMatrix() ); 

  // Set the point - four-momentum error matrix.
  // part.setPosMomCorr( pmeMat );
  // Set the end vertex reference.
  part.setEndVertex( &vtx ); 

  // Set the point - slopes+momentum error matrix. Already done internally
  // by Particle via setPosMomCorr
//    HepMatrix pseMat( 3, 3, 0 );  
//    part.setPosSlopesCorr( pseMat );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
/// Fill Composite Particle from a state
//=============================================================================
StatusCode ParticleStuffer::fillParticle( const LHCb::State& state,
                                          LHCb::Particle& particle ){
  
  // point on the track and error 
  particle.setReferencePoint( state.position() ) ;
  particle.setPosCovMatrix( state.errPosition()  ) ;
  
  // momentum
  Gaudi::XYZVector mom = state.momentum();
  double mass = particle.measuredMass();
  double e = sqrt( state.p()*state.p()+mass*mass );
  particle.setMomentum(  Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;

  // momentum error
  Gaudi::SymMatrix4x4 err ;
  err.Place_at(state.errMomentum(),0,0); // no error on mass
  particle.setMomCovMatrix(err);

  Gaudi::Matrix4x3 posMomMatrix;
  Gaudi::SymMatrix6x6 spm = state.posMomCovariance();
  for ( unsigned int i = 0 ; i<3; ++i){
    for ( unsigned int j = 0 ; j<3; ++j){
      posMomMatrix(i,j) = spm(3+i,j);  /// @todo : Check this
    }
  }
  particle.setPosMomCovMatrix(posMomMatrix);
  
  return StatusCode::SUCCESS ;
  
}
//=============================================================================
/// Sum 4-Momenta
//=============================================================================
Gaudi::XYZTVector ParticleStuffer::sumMomenta( const LHCb::Particle::ConstVector& vP ){
  Gaudi::XYZTVector sum;
  for ( LHCb::Particle::ConstVector::const_iterator i = vP.begin(); i != vP.end() ; ++i){
    sum += (*i)->momentum();
  }
  return sum;
}
//=============================================================================
/// Sum 4-Momenta
//=============================================================================
Gaudi::XYZTVector ParticleStuffer::sumMomenta( const SmartRefVector<LHCb::Particle>& vP ){
  Gaudi::XYZTVector sum;
  for ( SmartRefVector<LHCb::Particle>::const_iterator i = vP.begin(); i != vP.end() ; ++i){
    sum += (*i)->momentum();
  }
  return sum;
}







