// $Id: NsctVeloTransporter.cpp,v 1.7 2004-05-14 09:43:31 pkoppenb Exp $
// Include files 

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SmartDataPtr.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"

// Detector description classes
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"

// local
#include "NsctVeloTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NsctVeloTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<NsctVeloTransporter>          s_factory ;
const        IToolFactory& NsctVeloTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NsctVeloTransporter::NsctVeloTransporter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : AlgTool( type, name , parent ) 
  , m_tolerance(1.e-5)
  , m_k(0.01)
  , m_DDS(0) {

  // declare additional Interface
  declareInterface<IParticleTransporter>(this);
  
  // Retrieve the detector data service
  StatusCode sc = StatusCode::FAILURE;
  sc = serviceLocator()->service("DetectorDataSvc", m_DDS, true);
  if( sc.isFailure () ) {
    throw GaudiException("DetectorDataSvc Not Found", 
                         "ParticleTransporterException", 
                         StatusCode::FAILURE );
  }
}

//=============================================================================
// Accessor to Detector Data Service
//=============================================================================
IDataProviderSvc*  NsctVeloTransporter::detSvc() const {
  return m_DDS;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode NsctVeloTransporter::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "NsctVeloTransporter Initialization starting..."
      << endreq;
  log << MSG::DEBUG << "Retrieving now detector elements" << endreq;
  
  SmartDataPtr<IDetectorElement> vertex( detSvc(), "/dd/Structure/LHCb/Velo" );
  if( !vertex ) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Velo" << endreq;
    return StatusCode::FAILURE;
  }
  
  const ILVolume* ivol = vertex->geometry()->lvolume();
  
  unsigned long noppv = ivol->noPVolumes();
  
  // starting noppv 2 = vStation00
  // m_vStations = noppv - 2;
  
  // m_zVelo = z position of velo stations
  unsigned int ppvc;  
  // filling the Velo z position
  for (ppvc = 2; ppvc < noppv; ppvc++) {
    HepTransform3D vtran =  ivol->pvolume(ppvc)->matrixInv();
    
    Hep3Vector vvec = vtran.getTranslation();
    m_zVelo.push_back( vvec.z() );
    
  }
  return StatusCode::SUCCESS;  
}
//=============================================================================
//  Transport linear + multiple scattering (particle iterator)
//=============================================================================
StatusCode NsctVeloTransporter::transport(ParticleVector::const_iterator &icand, 
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
  
    sc = this->veloTransport(workParticle, znew, transParticle, ipz);
  return sc;    
}
//=============================================================================
//  Transport linear + multiple scattering (const particle)
//=============================================================================
StatusCode NsctVeloTransporter::transport(const Particle & icand, 
                                          double znew,
                                          Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = const_cast<Particle *> (&icand);
  
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
  
  sc = this->veloTransport(workParticle, znew, transParticle, ipz);
  return sc;    
}
//=============================================================================
//  Transport linear + multiple scattering (particle)
//=============================================================================
StatusCode NsctVeloTransporter::transport(Particle & icand, 
                                          double znew,
                                          Particle & transParticle){
  MsgStream log(msgSvc(), name());

  Particle *workParticle = (&icand);
  
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
  
  sc = this->veloTransport(workParticle, znew, transParticle, ipz);
  return sc;    
}
//=============================================================================
//  Transport linear + multiple scattering ( common calculation )
//=============================================================================
StatusCode NsctVeloTransporter::veloTransport(Particle *&workParticle, 
                                             double znew, 
                                             Particle &transParticle,
                                             int ipz){
  
  // initialize msg service
  
  MsgStream log(msgSvc(), name());
  
  if ( 0 == m_DDS ) {
    log << MSG::INFO << ">>> Transporter Failed to find Detector Data Svc " 
        << endreq;
  }
  
  // check for particle
  if ( !workParticle ) {
    log << MSG::WARNING
        << "NsctVeloTransporter::transport should be called with a"
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
  
  // counting how many stations are on the way 
  // if there is none, the transport will be linear
  
  int nscat = 0;
  double innerR = 8.0;
  double outerR = 42.0;
  double zr;
  double zl;
  zr = zold;
  zl = znew;
  if ( ipz == -1 ){ zr = znew; zl = zold;  }
  // upstream/downstream search
  std::vector<double>::const_iterator iStep;
  for ( iStep = m_zVelo.begin(); iStep != m_zVelo.end();
        iStep++ ){
    if ( *(iStep) < zr && *(iStep) > zl ) {
      // track distance from the beam at the station
      double xts = xold + ( *(iStep) - zold )*sx;
      double yts = yold + ( *(iStep) - zold )*sy;
      double rts =sqrt ( xts*xts + yts*yts );
      if ( innerR < rts && outerR > rts ) nscat = nscat + 1;
    }
  }
  
  // linear transport
  double dz2 = dz*dz;
  double xnew = xold + sx*dz;
  double ynew = yold + sy*dz;
  newPOT.set(xnew, ynew, znew);
  
  newPOTErr(1,1) = oldPOTErr(1,1) + 2.*dz*oldPosSlopesCorr(1,1) + 
    dz2*oldSlopesMomErr(1,1);    
  newPOTErr(2,1) = oldPOTErr(2,1) + 
    dz*( oldPosSlopesCorr(1,2) + oldPosSlopesCorr(2,1) ) + 
    dz2*oldSlopesMomErr(2,1);
  newPOTErr(2,2) = oldPOTErr(2,2) + 2.*dz*oldPosSlopesCorr(2,2) + 
    dz2*oldSlopesMomErr(2,2);
  newPosSlopesCorr(1,1) = oldPosSlopesCorr(1,1) + dz*oldSlopesMomErr(1,1);
  newPosSlopesCorr(2,2) = oldPosSlopesCorr(2,2) + dz*oldSlopesMomErr(2,2);
  
  // transport considers multiple scattering 
  double a = 1. / ( sqrt (1.0 + sx*sx + sy*sy ) ); 
  double p = workParticle->p();
  if ( nscat > 0 ) {    
    double b =  sx*a;
    double c =  sy*a;
    HepVector3D UnitVector(b, c, a);
    double CosTheta = UnitVector.z();
    double X0 = nscat /(fabs(CosTheta));
    
    double Teth = (m_k/p)*sqrt(X0);
    newSlopesMomErr(1,1) = oldSlopesMomErr(1,1) + 
      pow( Teth*(1. + sx*sx), 2);
    newSlopesMomErr(2,2) = oldSlopesMomErr(2,2) + 
      pow( Teth*(1. + sy*sy), 2);
    newPOTErr(1,1) = oldPOTErr(1,1) + 2.*dz*oldPosSlopesCorr(1,1) + 
      dz2*oldSlopesMomErr(1,1);    
    newPOTErr(2,2) = oldPOTErr(2,2) + 2.*dz*oldPosSlopesCorr(2,2) + 
      dz2*oldSlopesMomErr(2,2);
    newPosSlopesCorr(1,1) = oldPosSlopesCorr(1,1) + dz*oldSlopesMomErr(1,1);
    newPosSlopesCorr(2,2) = oldPosSlopesCorr(2,2) + dz*oldSlopesMomErr(2,2);
  }
  
  // a new "particle" is made with transported values
  transParticle.setPointOnTrack(newPOT);
  transParticle.setPointOnTrackErr(newPOTErr);
  transParticle.setSlopesMomErr(newSlopesMomErr);
  transParticle.setPosSlopesCorr(newPosSlopesCorr);
  return StatusCode::SUCCESS;  
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode NsctVeloTransporter::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> NsctVeloTransporter::finalize" << endreq;
  m_zVelo.clear();
  
  return StatusCode::SUCCESS;
}

