// $Id: LinearTransporter.cpp,v 1.6 2004-07-16 16:12:12 pkoppenb Exp $
// Include files 

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"

// Detector description classes

// local
#include "LinearTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LinearTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<LinearTransporter>          s_factory ;
const        IToolFactory& LinearTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LinearTransporter::LinearTransporter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : AlgTool( type, name , parent )
  , m_tolerance( 1.e-5 ) {

  // declare additional Interface
  declareInterface<IParticleTransporter>(this);

}
//=============================================================================
// Initialize
//=============================================================================
StatusCode LinearTransporter::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "LinearTransporter Initialization starting..." 
      << endreq;
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Transport with linear extrapolation (particle iterator)
//=============================================================================
StatusCode LinearTransporter::transport(ParticleVector::const_iterator &icand, 
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
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }


  
  StatusCode sc = StatusCode::SUCCESS;
  
    sc = this->linTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with linear extrapolation (const particle)
//=============================================================================
StatusCode LinearTransporter::transport(const Particle & icand, 
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
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }
  
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->linTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with linear extrapolation (particle)
//=============================================================================
StatusCode LinearTransporter::transport(Particle & icand, 
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
    log << MSG::DEBUG << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }
  
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->linTransport(workParticle, znew, transParticle);
  return sc;    
}
//=============================================================================
//  Transport with linear extrapolator common calculation
//=============================================================================
StatusCode LinearTransporter::linTransport(Particle *&workParticle, 
                                           double znew, 
                                           Particle &transParticle){
  
  // initialize msg service
  
  MsgStream log(msgSvc(), name());
  
  // check for particle
  if ( !workParticle ) {
    log << MSG::WARNING
        << "LinearTransporter::transport should be called with a"
        << "pointer to a particle as argument!" << endreq;
    return StatusCode::FAILURE;
  }

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
    log << MSG::DEBUG << " already at required a position " << endreq;
    return StatusCode::SUCCESS;
  }
  // x and y coordinates at point on track
  double xold = oldPOT.x();
  double yold = oldPOT.y();
  
  double sx = workParticle->slopeX();
  double sy = workParticle->slopeY();

  //transform to the same z with err_z=0
  HepMatrix JA(3,3);
  JA(1,1)=1;
  JA(1,2)=0;
  JA(1,3)=-sx,
  JA(2,1)=0;
  JA(2,2)=1;
  JA(2,3)=-sy;
  JA(3,1)=0;
  JA(3,2)=0;
  JA(3,3)=0;
  HepSymMatrix tmpPOTErr=oldPOTErr.similarity(JA);
  HepMatrix tmpPosSlopesCorr=oldPosSlopesCorr*JA.T();

  // linear transport of state
  //  double dz2 = dz*dz;
  double xnew = xold + sx*dz;
  double ynew = yold + sy*dz;
  newPOT.set(xnew, ynew, znew);

  // transfromation of the 6x6 covariance matrix 
  HepMatrix tF(6,6,1);
  tF[0][3] = dz;  
  tF[1][4] = dz;  
  HepSymMatrix C0(6,0);
  C0.sub(1,tmpPOTErr);
  C0.sub(4,oldSlopesMomErr);
  for (unsigned i=0;i<3;++i) {
    for (unsigned j=0;j<3;++j) C0[3+i][j] = tmpPosSlopesCorr[i][j];
  }  
  HepSymMatrix C1=C0.similarity(tF);  

  // extract the position matrix  and pos-slope correlation matrix
  // notice the slope matrix is unchanged
  newPOTErr=C1.sub(1,3);
  HepMatrix C1Copy=C1;
  newPosSlopesCorr=C1Copy.sub(4,6,1,3);  

  // a new "particle" is made with transported values
  transParticle.setPointOnTrack(newPOT);
  transParticle.setPointOnTrackErr(newPOTErr);
  transParticle.setSlopesMomErr(newSlopesMomErr);
  transParticle.setPosSlopesCorr(newPosSlopesCorr);

  return StatusCode::SUCCESS;  
}
