// $Id: TrTransporter.cpp,v 1.4 2004-03-31 06:54:48 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// Gaudi interfaces
//#include "GaudiKernel/IAlgManager.h"
//#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

// local
#include "TrTransporter.h"

// from Extrapolators
#include "TrKernel/ITrExtrapolator.h"

// from Event
#include "Event/TrStateP.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrTransporter
//
// 2002-06-14 : Edgar C. de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrTransporter>          s_factory ;
const        IToolFactory& TrTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrTransporter::TrTransporter( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_tolerance( 1.e-5 )
  , m_extrapolatorName ( "TrFirstCleverExtrapolator" )
  , m_extrapolator(0) {
  declareProperty("Extrapolator", m_extrapolatorName );

  // declare additional Interface
  declareInterface<IParticleTransporter>(this);

}

//=============================================================================
// Initializes Track extrapolator
// ============================================================================
StatusCode TrTransporter::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TrTransporter initialization" << endreq;

  StatusCode sc = toolSvc()->retrieveTool( m_extrapolatorName, 
                                           m_extrapolator, this );  
  if (sc.isFailure()){
    log<< MSG::FATAL<<"Unable to retrieve Extrapolator Tool "<<endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( "LinearTransporter", 
				m_LinearTransporter, this );  
  if (sc.isFailure()){
    log<< MSG::FATAL<<"Unable to retrieve Linear Extrapolator Tool "<<endreq;
    return sc;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Transport with TrExtrapolator (particle iterator)
//=============================================================================
StatusCode TrTransporter::transport(ParticleVector::const_iterator &icand, 
                                        double znew,
                                        Particle & transParticle){
  MsgStream log(msgSvc(), name());
  
  Particle *workParticle = (*icand);
  
  // check z-range
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }
  
  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::WARNING << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->trTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with TrExtrapolator (const particle)
//=============================================================================
StatusCode TrTransporter::transport(const Particle & icand, 
                                        double znew,
                                        Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = const_cast<Particle *> (&icand);
  
  // check z-range
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }
  
  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::WARNING << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }
  
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->trTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with TrExtrapolator (particle)
//=============================================================================
StatusCode TrTransporter::transport(Particle & icand, 
                                        double znew,
                                        Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = (&icand);
  
  // check z-range
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }
  
  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::WARNING << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }  
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->trTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with TrExtrapolator common calculation
//=============================================================================
StatusCode TrTransporter::trTransport(Particle *workParticle, 
                                      double znew, 
                                      Particle &transParticle){
  
  // initialize msg service
  
  MsgStream log(msgSvc(), name());
  
  // check for particle
  if ( !workParticle ) {
    log << MSG::WARNING
        << "TrTransporter::transport should be called with a"
        << "pointer to a particle as argument!" << endreq;
    return StatusCode::FAILURE;
  }
  if (workParticle->charge()!=0.0) {  
    // initialization of transported Particle  
    transParticle = *workParticle;
    
    // initialize and create PointOnTrack vectors  
    HepPoint3D oldPOT(999., 999., 999.);
    oldPOT = workParticle->pointOnTrack();
    HepPoint3D newPOT = oldPOT;
    
    // initialize and create PointOnTrack matrix error
    HepSymMatrix oldPOTErr(3, 0);
    oldPOTErr = workParticle->pointOnTrackErr();
    HepSymMatrix newPOTErr = oldPOTErr;
    
    // initialize and create Slope and Momentum correlation matrix error 
    HepSymMatrix oldSlopesMomErr(3, 0);
    oldSlopesMomErr = workParticle->slopesMomErr();
    HepSymMatrix newSlopesMomErr = oldSlopesMomErr;
    
    // initialize and create Position, Slope and Momentum correlation 
    // matrix error
    HepMatrix oldPosSlopesCorr(3, 3, 0);
    oldPosSlopesCorr = workParticle->posSlopesCorr();
    HepMatrix newPosSlopesCorr = oldPosSlopesCorr;
    
    // check current z-position
    double zold = oldPOT.z();
    double dz = znew - zold;
    
    if (fabs(dz) < m_tolerance){
      // already at required z position
      log << MSG::DEBUG << " already at required position " << endreq;
      return StatusCode::SUCCESS;
    }
    // x and y coordinates at point on track
    double xold = oldPOT.x();
    double yold = oldPOT.y();
    
    double sx = workParticle->slopeX();
    double sy = workParticle->slopeY();
    
    double charge = workParticle->charge();
    double momentum = workParticle->p();
    
    
    // Fill the TrState with Particle
    HepVector pState(5, 0);
    HepSymMatrix pCov(5, 1);
    TrStateP* workTrStateP = new TrStateP(zold, pState, pCov);
    
    // Initialize new momentum;
    HepLorentzVector newMomentum;
    newMomentum = workParticle->momentum();
    
    workTrStateP->setX( xold );
    workTrStateP->setY( yold );
    workTrStateP->setTx( sx );
    workTrStateP->setTy( sy );
    workTrStateP->setQdivP( charge/momentum );
    
    double qp =  charge/momentum;
    
    pState[0] = xold;
    pState[1] = yold;
    pState[2] = sx;
    pState[3] = sy; 
    pState[4] = qp;
    workTrStateP->setStateVector( pState );
    
    double a = -qp/momentum;
    
    pCov[0][0] = oldPOTErr[0][0];
    pCov[1][0] = oldPOTErr[1][0];
    pCov[1][1] = oldPOTErr[1][1];
    pCov[2][0] = oldPosSlopesCorr[0][0];
    pCov[2][1] = oldPosSlopesCorr[0][1];
    pCov[3][0] = oldPosSlopesCorr[1][0];
    pCov[3][1] = oldPosSlopesCorr[1][1];
    pCov[2][2] = oldSlopesMomErr[0][0];
    pCov[3][2] = oldSlopesMomErr[1][0];
    pCov[3][3] = oldSlopesMomErr[1][1];
    pCov[4][0] = a*oldPosSlopesCorr[2][0];
    pCov[4][1] = a*oldPosSlopesCorr[2][1];
    pCov[4][2] = a*oldSlopesMomErr[2][0];
    pCov[4][3] = a*oldSlopesMomErr[2][1];
    pCov[4][4] = a*a*oldSlopesMomErr[2][2];
    
    
    workTrStateP->setStateCov( pCov );
    
    
    ParticleID pID( workParticle->particleID() );
    StatusCode sc = workTrStateP->extrapolate(m_extrapolator, znew, pID);  
    if ( sc.isFailure() ) {
      log << MSG::FATAL << " Unable to extrapolate TrStateP " << endreq;
    }
    
    newPOT[0] = workTrStateP->x();
    newPOT[1] = workTrStateP->y();
    newPOT[2] = znew;
    
    double tx, ty, pz;
    
    tx = workTrStateP->tx();
    ty = workTrStateP->ty();
    pz = workTrStateP->p()/sqrt(1. + tx*tx + ty*ty);
    
    newMomentum(0) = tx*pz;
    newMomentum(1) = ty*pz;
    newMomentum(2) = pz;
    // NEW
    newMomentum(3) = sqrt(  workTrStateP->p()*workTrStateP->p()
                           +workParticle->mass()*workParticle->mass());
    
    pCov = workTrStateP->pCovMatrix();
        
    newPOTErr[0][0] = pCov[0][0];
    newPOTErr[1][0] = pCov[1][0];
    newPOTErr[1][1] = pCov[1][1];
    newPosSlopesCorr[0][0] = pCov[2][0];
    newPosSlopesCorr[0][1] = pCov[2][1];
    newPosSlopesCorr[1][0] = pCov[3][0];
    newPosSlopesCorr[1][1] = pCov[3][1];
    newSlopesMomErr[0][0] = pCov[2][2];
    newSlopesMomErr[1][0] = pCov[3][2];
    newSlopesMomErr[1][1] = pCov[3][3];
    newPosSlopesCorr[2][0] = pCov[4][0];
    newPosSlopesCorr[2][1] = pCov[4][1];
    newSlopesMomErr[2][0] = pCov[4][2];
    newSlopesMomErr[2][1] = pCov[4][3];
    newSlopesMomErr[2][2] = pCov[4][4];
    
    // a new "particle" is made with transported values
    transParticle.setPointOnTrack(newPOT);
    transParticle.setMomentum(newMomentum);
    transParticle.setPointOnTrackErr(newPOTErr);
    transParticle.setSlopesMomErr(newSlopesMomErr);
    transParticle.setPosSlopesCorr(newPosSlopesCorr);
    
    delete workTrStateP;
  }
  else{
    // Use only linear Transport
    StatusCode sc = m_LinearTransporter->transport(*workParticle,
						  znew,
						  transParticle);
    if ( sc.isFailure() ) {
      log << MSG::FATAL << " Unable to extrapolate Neutral Particle" << endreq;
    }
  }
  return StatusCode::SUCCESS;  
}
