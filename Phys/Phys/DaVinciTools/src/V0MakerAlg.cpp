// $Id: V0MakerAlg.cpp,v 1.1 2003-02-07 18:41:25 gcorti Exp $
// Include files 
#include <memory>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from EventSys
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/V0.h"

#include "DaVinciTools/IParticleStuffer.h"

// local
#include "V0MakerAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : V0MakerAlg
//
// 2002-12-02 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<V0MakerAlg>          s_factory ;
const        IAlgFactory& V0MakerAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
V0MakerAlg::V0MakerAlg( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_inputPath( "/Event/Rec/V0/V0s" )
  , m_outputPath( "/Event/Phys/UpstreamKs" )
  , m_v0Name( "KS0" )
  , m_pStuffer( 0 )
{

  declareProperty( "InputLocation",  m_inputPath );
  declareProperty( "OutputLocation", m_outputPath); 
  declareProperty( "V0Name", m_v0Name );  

}

//=============================================================================
// Destructor
//=============================================================================
V0MakerAlg::~V0MakerAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode V0MakerAlg::initialize() {

  MsgStream msg(msgSvc(), name());

  StatusCode sc = service("ParticlePropertySvc", m_ppSvc);
  if( !sc.isSuccess() ) {
    msg << MSG::FATAL << "Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  ParticleProperty* partProp;
  partProp = m_ppSvc->find( m_v0Name );
  m_v0ID.setPid( (*partProp).jetsetID() );

  msg << MSG::DEBUG << "Retreiving ParticleStuffer" << endreq;
  sc = toolSvc()->retrieveTool( "ParticleStuffer", m_pStuffer );
  if( sc.isFailure() ) {
    msg << MSG::ERROR << "ParticleStuffer not found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode V0MakerAlg::execute() {

  MsgStream  msg( msgSvc(), name() );

  // Prepare output containers
  std::string outputPath = m_outputPath+"/Protos";
  ProtoParticles* ksProtoCnt = new ProtoParticles();
  StatusCode sc = eventSvc()->registerObject( outputPath, ksProtoCnt );
  if( !sc.isSuccess() ) {
    delete ksProtoCnt; 
    msg << MSG::ERROR  
        << "Unable to register ProtoParticles in " << outputPath << endreq;
    return sc;
  } else {
    msg << MSG::DEBUG << "Writing ProtoParticles in " << outputPath << endreq;
  }

  outputPath = m_outputPath+"/Particles"; 
  Particles* ksPartCnt = new Particles();
  sc = eventSvc()->registerObject( outputPath, ksPartCnt );
  if( !sc.isSuccess() ) {
    delete ksPartCnt; 
    msg << MSG::ERROR  
        << "Unable to register Particles in " << outputPath << endreq;
    return sc;
  } else {
    msg << MSG::DEBUG << "Writing Particles in " << outputPath << endreq;
  }
  
  outputPath = m_outputPath+"/Vertices"; 
  Vertices* ksVertCnt = new Vertices();
  sc = eventSvc()->registerObject( outputPath, ksVertCnt );
  if( !sc.isSuccess() ) {
    delete ksPartCnt; 
    msg << MSG::ERROR
        << "Unable to register Vertices in " << m_outputPath << endreq;
    return sc;
  } else {
    msg << MSG::DEBUG << "Writing Vertices in " << outputPath << endreq;
  }
  
  // Now load input data
  SmartDataPtr<V0s> kshortCnt ( eventSvc(), m_inputPath );
  if( !kshortCnt ) {
    msg << MSG::ERROR
        << "Unable to find V0s in " << m_inputPath << endreq;
    return StatusCode::FAILURE;
  }
  if( 0 == kshortCnt->size() ) {
    msg << MSG::DEBUG
        << "No V0s found in " << m_inputPath << endreq;
  }
  
  // Transform input data into V0
  for( V0s::const_iterator iks = kshortCnt->begin(); 
       kshortCnt->end() != iks; ++iks ) {

    auto_ptr<ProtoParticle> protoPiP( new ProtoParticle() );
    fillProtoP( (*iks)->trackPos(),  protoPiP.get() );    
    auto_ptr<Particle> piP( new Particle() );
    fillParticle( protoPiP.get(), (*iks)->pIDPos().pid(), piP.get() );
    
    auto_ptr<ProtoParticle> protoPiM( new ProtoParticle() );
    fillProtoP( (*iks)->trackNeg(),  protoPiM.get() );
    auto_ptr<Particle> piM( new Particle() ); 
    fillParticle( protoPiM.get(), (*iks)->pIDNeg().pid(), piM.get() );
    
    auto_ptr<Vertex> vtx( new Vertex() );
    *vtx = *((*iks)->vertex());
    vtx->clearProducts();
    vtx->addToProducts(piP.get());
    vtx->addToProducts(piM.get());

    auto_ptr<Particle> kshort( new Particle() );
    m_pStuffer->fillParticle( *(vtx.get()), *(kshort.get()), m_v0ID );
    
    ksProtoCnt->insert(protoPiP.release());
    ksProtoCnt->insert(protoPiM.release());
    
    ksPartCnt->insert(piP.release());
    ksPartCnt->insert(piM.release());
    ksPartCnt->insert(kshort.release());
    
    ksVertCnt->insert(vtx.release());
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  fillProtoP
//=============================================================================
void V0MakerAlg::fillProtoP( TrStoredTrack* aTrack, ProtoParticle* aProto ) {

  aProto->setTrack( aTrack );
  ProtoParticle::PIDDetVector iddetvec;
  ProtoParticle::PIDInfoVector idinfovec;
  aProto->setPIDDetectors(iddetvec);
  aProto->setPIDInfo(idinfovec);
  
  return;
};


//=============================================================================
//  Finalize
//=============================================================================
StatusCode V0MakerAlg::finalize() {

  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG << "Releasing tools" << endreq;

  if( 0 != m_pStuffer ) {
    toolSvc()->releaseTool( m_pStuffer );
    m_pStuffer = 0;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// fillParticle
//=============================================================================
StatusCode V0MakerAlg::fillParticle( ProtoParticle* protoCand,
                                     int protoID, Particle* particle ) {
  
  // Start filling the particle:     
  particle->setParticleID( ParticleID(protoID) );
  particle->setConfLevel( 1.0 );
  // Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  
  const TrStateP* trackState = protoCand->trStateP();
  if( !trackState ) return StatusCode::FAILURE;
  
  // Set position of first measured point on track:
  HepPoint3D position( trackState->x(), trackState->y(), trackState->z() ) ;
  particle->setPointOnTrack( position );
  
  // Calculate and set four momentum: do this in ProtoParticle... 
  double momentum = trackState->p();
  double slopeX   = trackState->tx();
  double slopeY   = trackState->ty();     
  double pZ = momentum/sqrt( 1.0 + slopeX*slopeX + slopeY*slopeY );
  HepLorentzVector quadriMomentum;
  quadriMomentum.setPx( pZ*slopeX );
  quadriMomentum.setPy( pZ*slopeY );
  quadriMomentum.setPz( pZ );
  quadriMomentum.setE( sqrt( mass*mass + momentum*momentum) );
  particle->setMomentum( quadriMomentum );
  
  // Retrieve track state covariance matrix and set particle error matrices:
  const HepSymMatrix& trkCov = trackState->pCovMatrix();
  
  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr = trkCov.sub(1,3);
  pointOnTrackErr(3,1) = 0.0;
  pointOnTrackErr(3,2) = 0.0;
  pointOnTrackErr(3,3) = 0.0;
  particle->setPointOnTrackErr(pointOnTrackErr);
  
  // Set slope+Momentum error:
  HepSymMatrix slpMomErr(3, 0);
  slpMomErr = trkCov.sub(3,5);
  particle->setSlopesMomErr(slpMomErr);
  
  // Set position-slopes correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posSlopesCorr(3, 3, 0);
  int i, j;
  for( i = 1; i <= 3; i++ ) {
    for ( j = 1; j <= 2; j++ ) {
      int istart = i+2;
      posSlopesCorr(i,j) = trkCov(istart,j);
    }
  }
  particle->setPosSlopesCorr(posSlopesCorr);
  
  particle->setOrigin(protoCand);
  return StatusCode::SUCCESS;
}
