// $Id: ParticleTransporter.cpp,v 1.1 2002-03-27 20:35:01 gcorti Exp $
// Include files 

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
//#include "GaudiKernel/IConversionSvc.h"
//#include "GaudiKernel/IConverter.h"
//#include "GaudiKernel/RegistryEntry.h"

// Detector description classes
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
//#include "DetDesc/Material.h"

// local
#include "ParticleTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParticleTransporter>          s_factory ;
const        IToolFactory& ParticleTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleTransporter::ParticleTransporter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : AlgTool ( type, name , parent ), m_vStations(0), m_k(0.01){
  // declare additional Interface
  declareInterface<IParticleTransporter>(this);
  m_tolerance = 1.e-5;
  // Retrieve the data service and incident service
  m_DDS = 0;
  StatusCode sc = StatusCode::FAILURE;
  sc = serviceLocator()->service("DetectorDataSvc", m_DDS, true);
}

//=============================================================================

IDataProviderSvc*  ParticleTransporter::detSvc() const {
  return m_DDS;
}

//=============================================================================
StatusCode ParticleTransporter::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "Initialization starting..." << endreq;
  log << MSG::INFO << "Retrieving now detector elements" << endreq;
  
  SmartDataPtr<IDetectorElement> vertex( detSvc(), "/dd/Structure/LHCb/Velo" );
  if( !vertex ) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Velo" << endreq;
    return StatusCode::FAILURE;
  }
  
  const ILVolume* ivol = vertex->geometry()->lvolume();
  
  unsigned long noppv = ivol->noPVolumes();
  
  // starting noppv 2 = vStation00
  m_vStations = noppv - 2;
  
  // m_zVelo = z position of velo stations
  unsigned int ppvc;  
  // filling the Velo z position
  for (ppvc = 0; ppvc < noppv; ppvc++) {
    HepTransform3D vtran =  ivol->pvolume(ppvc)->matrixInv();
    Hep3Vector vvec = vtran.getTranslation();
    m_zVelo.push_back( vvec.z() );
  }
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Linear Transporter and multiscatteing
//=============================================================================
StatusCode ParticleTransporter::transport(Particles::const_iterator & icand, 
                                          float znew,
                                          HepPoint3D & newPOT,
                                          HepSymMatrix & newPOTErr,
                                          HepSymMatrix & newSlopesMomErr,
                                          HepMatrix & newPosSlopesCorr){
  // initialize msg service
  MsgStream log(msgSvc(), name());
  
  if ( 0 == m_DDS ) {
    log << MSG::INFO << ">>> Transporter Failed to find Detector Data Svc " 
        << endreq;
  }
  
  // check for particle
  if ( !icand ) {
    log << MSG::WARNING
        << "ParticleTransporter::transport should be called with a"
        << "pointer to a particle as argument!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // initialize and create PointOnTrack vectors  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = (*icand)->pointOnTrack();
  newPOT = oldPOT;
  
  // initialize and create PointOnTrack matrix error
  HepSymMatrix oldPOTErr(3, 0.);
  oldPOTErr = (*icand)->pointOnTrackErr();
  newPOTErr = oldPOTErr;
  
  // initialize and create Slope and Momentum correlation matrix error 
  HepSymMatrix oldSlopesMomErr(3, 0.);
  oldSlopesMomErr = (*icand)->slopesMomErr();
  newSlopesMomErr = oldSlopesMomErr;
  
  // initialize and create Position, Slope and Momentum correlation 
  // matrix error
  HepMatrix oldPosSlopesCorr(3, 3, 0.);
  oldPosSlopesCorr = (*icand)->posSlopesCorr();
  newPosSlopesCorr = oldPosSlopesCorr;
  
  // check current z-position
  float zold = oldPOT.z();
  float dz = znew - zold;
  if (fabs(dz) < m_tolerance){
    // already at required z position
    log << MSG::DEBUG << " already at required a position " << endreq;
    return StatusCode::SUCCESS;
  }
  // x and y coordinates at point on track
  float xold = oldPOT.x();
  float yold = oldPOT.y();
  
  // check z range
  if ( zold < -500. || zold > 20000. || znew < -500. || znew > 20000. ){
    // z is out of range
    log << MSG::INFO << " z is out of range, z < -500. or z > 20000." 
        << endreq;
    
    return StatusCode::FAILURE;
  }
  
  float slpx = (*icand)->slopeX();
  float slpy = (*icand)->slopeY();
  
  // counting how many stations are on the way 
  // if there is none, the transport will be linear
  
  int nscat = 0;
  float innerR = 8.0;
  float outerR = 42.0;
  
  // forward search
  std::vector<double>::const_iterator iStep;
  for ( iStep = m_zVelo.begin(); iStep != m_zVelo.end();
        iStep++ ){
    if ( *(iStep) < zold && *(iStep) > znew ) {
      // track distance from the beam at the station
      float xts = xold + ( *(iStep) - zold )*slpx;
      float yts = yold + ( *(iStep) - zold )*slpy;
      float rts =sqrt ( xts*xts + yts*yts );
      if ( innerR < rts && outerR > rts ) nscat = nscat + 1;
    }
  }
  
  // linear transport
  float dz2 = dz*dz;
  float xnew = oldPOT.x() + slpx*dz;
  float ynew = oldPOT.y() + slpy*dz;
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
  
  // transport considers multscattering according to LHCb note 
  // LHCB-98-47 (F. Teubert, et al. equation (2) and L1Track.cpp
  if ( nscat > 0 ) {    
    float u3 = 1. / ( sqrt (1.0 + slpx*slpx + slpy*slpy ) ); 
    float u1 =  slpx*u3;
    float u2 =  slpy*u3;
    HepVector3D UnitVector(u1, u2, u3);
    float CosTheta = UnitVector.z();
    float X0 = nscat /(fabs(CosTheta));
    float pTrack = (*icand)->p();
    
    float Teth = (m_k/pTrack)*sqrt(X0);
    newSlopesMomErr(1,1) = oldSlopesMomErr(1,1) + 
      pow( Teth*(1. + slpx*slpx), 2);
    newSlopesMomErr(2,2) = oldSlopesMomErr(2,2) + 
      pow( Teth*(1. + slpy*slpy), 2);
    newPOTErr(1,1) = oldPOTErr(1,1) + 2.*dz*oldPosSlopesCorr(1,1) + 
      dz2*oldSlopesMomErr(1,1);    
    newPOTErr(2,2) = oldPOTErr(2,2) + 2.*dz*oldPosSlopesCorr(2,2) + 
      dz2*oldSlopesMomErr(2,2);
    newPosSlopesCorr(1,1) = oldPosSlopesCorr(1,1) + dz*oldSlopesMomErr(1,1);
    newPosSlopesCorr(2,2) = oldPosSlopesCorr(2,2) + dz*oldSlopesMomErr(2,2);
  }
  return StatusCode::SUCCESS;  
}

//=============================================================================

StatusCode ParticleTransporter::finalize() {
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "finalize" << endreq;
  
  return StatusCode::SUCCESS;
}
