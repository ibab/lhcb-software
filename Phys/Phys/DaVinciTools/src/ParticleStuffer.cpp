// $Id: ParticleStuffer.cpp,v 1.1 2002-03-27 20:35:01 gcorti Exp $
// Include files 

// 
#include <math.h>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

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
  : AlgTool ( type, name , parent ) {

  // declare additional Interface
  declareInterface<IParticleStuffer>(this);
}

//=============================================================================
// Stuffer
//=============================================================================
StatusCode ParticleStuffer::fillParticle( const Vertex& vtx, Particle& part, 
                                          const ParticleID& pid ){

  // Set the ParticleID.
  part.setParticleID( pid ); 

  // Set the four-momentum.
  HepLorentzVector lorVec( 0., 0., 0., 0. );
  SmartRefVector<Particle>::const_iterator it;  
  for (it = vtx.products().begin(); 
        it != vtx.products().end(); it++ ) {
    lorVec += (*it)->momentum(); 
  }
  part.setMomentum( lorVec );

  // Set the four-momentum error matrix.
  // The slopes+momentum error matrix is set internally.
  HepSymMatrix meMat( 4, 0 ); 
  for (it = vtx.products().begin(); it != vtx.products().end(); it++) {
    meMat += (*it)->momentumErr();    
  }
  part.setMomentumErr( meMat );

  // Set the measured mass.
  part.setMass( part.momentum().mag() ); 

  // Set the error on the measured mass.
  //HepVector derivs(4);
  //derivs(1) = - part.momentum().px() / sqrt( part.mass() );
  //derivs(2) = - part.momentum().py() / sqrt( part.mass() );
  //derivs(3) = - part.momentum().pz() / sqrt( part.mass() );
  //derivs(4) =   part.momentum().e()  / sqrt( part.mass() );
  //double massErr = sqrt( part.momentumErr().similarity( derivs ) );
  //part.setMassErr( massErr );
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
  HepMatrix pmeMat( 3, 4, 0 );  
  //part.setCorrelationErr( pmeMat );
  part.setPosMomCorr( pmeMat );

  // Set the end vertex reference.
  part.setEndVertex( &vtx ); 

  // Set the point - slopes+momentum error matrix.
  HepMatrix pseMat( 3, 3, 0 );  
  //part.setPosSlpMomCorrelationErr( pseMat );
  part.setPosSlopesCorr( pseMat );

  return StatusCode::SUCCESS;
}
//=============================================================================





