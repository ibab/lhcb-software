// $Id: ParticleStuffer.cpp,v 1.3 2006-01-24 07:38:16 pkoppenb Exp $
// Include files 

// 
#include <math.h>
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/ThreeVector.h" 

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
  , m_transporterType("CombinedTransporter"){

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
StatusCode ParticleStuffer::fillParticle( const Vertex& vtx, Particle& part, 
                                          const ParticleID& pid ){

  int stdHepID = pid.pid();
  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(stdHepID  );

  if (partProp){
    if( (*partProp).lifetime()*pow(10,-9) < pow(10,-15)) part.setIsResonance(true);
  } else warning() << "No particle property" << endmsg;
 
  
  // Set the ParticleID.
  part.setParticleID( pid ); 

  double zVtxPos = vtx.position().z();
  // Set the four-momentum.
  //  HepLorentzVector lorVec( 0., 0., 0., 0. );
  HepLorentzVector MotherlorVec( 0., 0., 0., 0. );
  SmartRefVector<Particle>::const_iterator it;  
  //  HepSymMatrix meMat( 4, 0 ); 
  HepSymMatrix MothermeMat( 4, 0 ); 

  Particle transParticle;
  for (it = vtx.products().begin(); it != vtx.products().end(); it++ ) {
    if ( (*it)->pointOnTrack().z() > 950.) {
      debug() << "Position > 950 " << (*it)->pointOnTrack().z() << endmsg;
    }

    // Transport Particle parameters to the vertex position	 
    StatusCode sctrans = m_pTransporter->transport(*(*it),
                                                   zVtxPos,
                                                   transParticle);
    if ( !sctrans.isSuccess() ) {
      debug() << "Track extrapolation failed" << endmsg;
      return sctrans;
    }

    //    lorVec += (*it)->momentum(); 
    MotherlorVec += transParticle.momentum(); 
    // Set the four-momentum error matrix.
    // The slopes+momentum error matrix is set internally.
    //    meMat += (*it)->momentumErr();    
    MothermeMat += transParticle.momentumErr();    
  }
  //  part.setMomentum( lorVec );
  part.setMomentum( MotherlorVec );

  //  part.setMomentumErr( meMat );
  part.setMomentumErr( MothermeMat );

  // Set the measured mass.
  part.setMass( part.momentum().mag() ); 

  // Set the error on the measured mass.
  HepMatrix derivs(1,4);
  HepSymMatrix massErrSqd(1,1);
  derivs(1,1) = - part.momentum().px() / part.mass();
  derivs(1,2) = - part.momentum().py() / part.mass();
  derivs(1,3) = - part.momentum().pz() / part.mass();
  derivs(1,4) =   part.momentum().e()  / part.mass();
  massErrSqd = part.momentumErr().similarity( derivs );  
  double massErr = sqrt( massErrSqd(1,1) );
  part.setMassErr( massErr ); 

  // Set the point on track.
  part.setPointOnTrack( vtx.position() ); 

  // Set the point on track error matrix.
  part.setPointOnTrackErr( vtx.positionErr() ); 

  // Set the point - four-momentum error matrix.
  HepMatrix pmeMat( 4, 3, 0 );  
  part.setPosMomCorr( pmeMat );
  // Set the end vertex reference.
  part.setEndVertex( &vtx ); 

  // Set the point - slopes+momentum error matrix. Already done internally
  // by Particle via setPosMomCorr
//    HepMatrix pseMat( 3, 3, 0 );  
//    part.setPosSlopesCorr( pseMat );
  
  return StatusCode::SUCCESS;
}
//=============================================================================





