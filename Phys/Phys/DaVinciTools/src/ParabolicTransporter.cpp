// $Id: ParabolicTransporter.cpp,v 1.8 2004-07-19 12:44:08 pkoppenb Exp $
// Include files 

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"

// local
#include "ParabolicTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParabolicTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParabolicTransporter>          s_factory ;
const        IToolFactory& ParabolicTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParabolicTransporter::ParabolicTransporter( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_pIMF(0)
  , m_tolerance(1.e-5) {
  // declare additional Interface
  declareInterface<IParticleTransporter>(this);

}
//=============================================================================
// Initialize
//=============================================================================
StatusCode ParabolicTransporter::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "ParabolicTransporter Initialization starting..." 
      << endreq;
  
  StatusCode sc = service("MagneticFieldSvc", m_pIMF, true);
  
  if( sc.isSuccess() ) {
    log << MSG::DEBUG <<"Magnetic field service found "
        << "m_pIMF = " << m_pIMF << endreq;
    return StatusCode::SUCCESS;
  }
  else {
    log << MSG::ERROR << "failed to get magnetic field service !!"
        << endreq;
    return StatusCode::FAILURE;
  }
}
//=============================================================================
//  Transport in a magnetic field (parabolic approximation)
//=============================================================================
StatusCode 
ParabolicTransporter::transport(ParticleVector::const_iterator & icand, 
                                double znew,
                                Particle & transParticle){
  MsgStream log(msgSvc(), name());
  
  Particle *workParticle = (*icand);
  
  // check z-range
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }

  
  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  
  StatusCode sc = StatusCode::SUCCESS;
  sc = this->magfTransport(workParticle, znew, transParticle);  
  return sc;    
}
//=============================================================================
//  Transport in a magnetic field (parabolic approximation)
//=============================================================================
StatusCode ParabolicTransporter::transport(const Particle & icand, 
                                           double znew,
                                           Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = const_cast<Particle *> (&icand);

  // check z-range and decide which transport will be done
  // with or without magnetic field or none.
  // any other option ?
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }

  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  StatusCode sc = StatusCode::SUCCESS;
  sc = this->magfTransport(workParticle, znew, transParticle);  
  return sc;    
}
//=============================================================================
//  Transport in a magnetic field (parabolic approximation)
//=============================================================================
StatusCode ParabolicTransporter::transport(Particle & icand, 
                                           double znew,
                                           Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = (&icand);

  // check z-range and decide which transport will be done
  // with or without magnetic field or none.
  // any other option ?
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }

  if ( zl < -500.0 || zr > 21000.0 ){
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }

  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks  
  StatusCode sc = StatusCode::SUCCESS;
  sc = this->magfTransport(workParticle, znew, transParticle);  
  return sc;    
}
//=============================================================================
// Magnetic field Transporter
//=============================================================================
StatusCode ParabolicTransporter::magfTransport(Particle *& workParticle, 
                                               double znew, 
                                               Particle &transParticle){

  // initialize msg service
  MsgStream log(msgSvc(), name());

  // check for particle
  if ( !workParticle ) {
    log << MSG::WARNING
        << "ParabolicTransporter::transport should be called with a"
        << "pointer to a particle as argument!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // initialization of transported Particle
  transParticle = *workParticle;
  
  // initialize and create PointOnTrack vectors  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  HepPoint3D newPOT = oldPOT;
  
  //initialize and crate Momentum vector
  HepLorentzVector newMomentum;
  HepLorentzVector oldMomentum = workParticle->momentum();
  newMomentum = oldMomentum;
  
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

  // transform to the same z with err_z=0
  HepMatrix JA(3,3);
  JA(1,1)=1;
  JA(1,2)=0;
  JA(1,3)=-workParticle->slopeX();
  JA(2,1)=0;
  JA(2,2)=1;
  JA(2,3)=-workParticle->slopeY();
  JA(3,1)=0;
  JA(3,2)=0;
  JA(3,3)=0;
  oldPOTErr=oldPOTErr.similarity(JA);
  oldPosSlopesCorr=oldPosSlopesCorr*JA.T();

  // check current z-position
  double zold = oldPOT.z();
  double dz = znew - zold;

  if (fabs(dz) < m_tolerance){
    // already at required z position
    log << MSG::DEBUG << " already at required a position " << endreq;
    return StatusCode::SUCCESS;
  }

  // x and y coordinates at point on track
  double xold = oldPOT.x();
  double yold = oldPOT.y();

  double sx = workParticle->slopeX();
  double sy = workParticle->slopeY();
  double R2 = 1.0 + sx*sx + sy*sy;
  double R = sqrt(R2);


  // get the B field
  HepVector3D fB(0.0,0.0,0.0);
  m_pIMF->fieldVector( oldPOT, fB );


  // Parabolic extrapolation
  // U = u + u'*dz + (1/2)*u''*dz*dz, u={x,y}, ' = d/dz(u)
  // d/dz(d/dz)(x) = (q/p)*R*{sx*sy*Bx - (1. + sx*sx)*By + sy*Bz}
  //               = (q/p)*fAx
  // d/dz(d/dz)(x) = (q/p)*R*{(1. + sx*sy)*Bx - sx*sy*By - sx*Bz}
  //               = (q/p)*fAy

  // calculate the A factors
  double Bx = fB.x();
  double By = fB.y();
  double Bz = fB.z();

  double fAx = R*( sx*sy*Bx - (1. + sx*sx)*By + sy*Bz );
  double fAy = R*( (1. + sy*sy)*Bx - sx*sy*By - sx*Bz );

  double q = workParticle->charge();
  double p = workParticle->p();
  double qDivP = q/p;
  double dz2 = dz*dz;
  double xnew = xold + sx*dz + 0.5*qDivP*eplus*c_light*fAx*dz2;
  double ynew = yold + sy*dz + 0.5*qDivP*eplus*c_light*fAy*dz2;
  double newSx = sx + qDivP*eplus*c_light*fAx*dz;
  double newSy = sy + qDivP*eplus*c_light*fAy*dz;
  double newR = sqrt( 1. + newSx*newSx + newSy*newSy);
  

  // M is the transport matrix (d/dX0)(X), X={x,y,x',y',p}
  // TM is the transposed of M
  // The new correlation matrix V(X) = V(X0).similarity( TM(X0) )
  // V(X) = TM.V.M(X0)
  HepMatrix TM(5, 5, 1);
  HepSymMatrix VX0(5, 0);
  // Calculating the elements not equal to zero or 1
  // Some auxiliary derivatives
  double dfAxdSx = sx*fAx/R2 + R*( sy*Bx - 2.*sx*By);
  double dfAxdSy = sy*fAx/R2 + R*( sx*Bx + Bz);
  double dfAydSx = sx*fAy/R2 + R*(-sy*By - Bz);
  double dfAydSy = sy*fAy/R2 + R*(-sx*By + 2.*sy*Bx);

  TM(1,3) = dz + 0.5*dfAxdSx*qDivP*eplus*c_light*dz2;
  TM(1,4) = 0.5*dfAxdSy*qDivP*eplus*c_light*dz2;
  TM(1,5) = -0.5*fAx*q*eplus*c_light*dz2/(p*p);


  TM(2,3) = 0.5*dfAydSx*qDivP*eplus*c_light*dz2;
  TM(2,4) = dz + 0.5*dfAydSy*qDivP*eplus*c_light*dz2;
  TM(2,5) = -0.5*fAy*q*eplus*c_light*dz2/(p*p);

  TM(3,3) = 1.0 + dfAxdSx*qDivP*eplus*c_light*dz;
  TM(3,4) = dfAxdSy*qDivP*eplus*c_light*dz;
  TM(3,5) = -fAx*q*eplus*c_light*dz/(p*p);

  TM(4,3) = dfAydSx*qDivP*eplus*c_light*dz;
  TM(4,4) = 1.0 + dfAydSy*qDivP*eplus*c_light*dz;
  TM(4,5) = -fAy*q*eplus*c_light*dz/(p*p);

  VX0(1,1) = oldPOTErr(1,1);
  VX0(2,1) = oldPOTErr(2,1);
  VX0(2,2) = oldPOTErr(2,2);

  VX0(3,1) = oldPosSlopesCorr(1,1);
  VX0(3,2) = oldPosSlopesCorr(1,2);
  VX0(4,1) = oldPosSlopesCorr(2,1);
  VX0(4,2) = oldPosSlopesCorr(2,2);
  VX0(5,1) = oldPosSlopesCorr(3,1);
  VX0(5,2) = oldPosSlopesCorr(3,2);

  VX0(3,3) = oldSlopesMomErr(1,1);
  VX0(4,3) = oldSlopesMomErr(2,1);
  VX0(4,4) = oldSlopesMomErr(2,2);
  VX0(5,3) = oldSlopesMomErr(3,1);
  VX0(5,4) = oldSlopesMomErr(3,2);
  VX0(5,5) = oldSlopesMomErr(3,3);

  VX0 = VX0.similarity(TM);

  newPOTErr(1,1)= VX0(1,1);
  newPOTErr(2,1)= VX0(2,1);
  newPOTErr(2,2)= VX0(2,2);

  newPosSlopesCorr(1,1) = VX0(3,1);
  newPosSlopesCorr(1,2) = VX0(3,2);
  newPosSlopesCorr(2,1) = VX0(4,1);
  newPosSlopesCorr(2,2) = VX0(4,2);
  newPosSlopesCorr(3,1) = VX0(5,1);
  newPosSlopesCorr(3,2) = VX0(5,2);

  newSlopesMomErr(1,1) = VX0(3,3);
  newSlopesMomErr(2,1) = VX0(4,3);
  newSlopesMomErr(2,2) = VX0(4,4);
  newSlopesMomErr(3,1) = VX0(5,3);
  newSlopesMomErr(3,2) = VX0(5,4);
  newSlopesMomErr(3,3) = VX0(5,5);

  // a new "particle" is made with transported values

  newPOT.set(xnew, ynew, znew);
  newMomentum(0) = newSx*p/newR;
  newMomentum(1) = newSy*p/newR;
  newMomentum(2) = p/newR;
  transParticle.setPointOnTrack(newPOT);
  transParticle.setMomentum(newMomentum);
  transParticle.setPointOnTrackErr(newPOTErr);
  transParticle.setSlopesMomErr(newSlopesMomErr);
  transParticle.setPosSlopesCorr(newPosSlopesCorr);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode ParabolicTransporter::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> ParabolicTransporter::finalize" << endreq;
  return StatusCode::SUCCESS;
}
