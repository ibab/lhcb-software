// $Id: CombinedTransporter.cpp,v 1.2 2002-05-17 11:42:05 gcorti Exp $
// Include files 

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

// local
#include "CombinedTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombinedTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CombinedTransporter>          s_factory ;
const        IToolFactory& CombinedTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombinedTransporter::CombinedTransporter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : AlgTool( type, name , parent ), 
  m_pVeloTransporter(0),
  m_pMFTransporter(0) {

  // declare additional Interface
  declareInterface<IParticleTransporter>(this);

  // declare Properties
  declareProperty("VeloTransporterName", 
                        pVeloTransporterName ="NsctVeloTransporter");
  declareProperty("MFTransporterName", 
                        pMFTransporterName ="ParabolicTransporter");

}
//=============================================================================
// Initialize
//=============================================================================
StatusCode CombinedTransporter::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "CombinedTransporter Initialization starting..."
      << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;

  // This tool needs to use internally the ToolSvc to retrieve the
  // transporter tool   
  sc = toolSvc()->retrieveTool(pVeloTransporterName,
                               m_pVeloTransporter, this);
  if(sc.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve VeloTransporter  tool"
        << endreq;
    return sc;
  }
  
  sc = toolSvc()->retrieveTool(pMFTransporterName,
                                m_pMFTransporter, this);
  if(sc.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve MFTransporter  tool"
        << endreq;
    return sc;
  }
  return sc;
}
//=============================================================================
//  Transport linear in Velo region, parabolic in field region
//=============================================================================
StatusCode CombinedTransporter::transport(Particles::const_iterator &iterP, 
                                          double znew,
                                          Particle & transParticle){
  MsgStream log(msgSvc(), name());
  
  //Particle *workParticle = (*icand);
  
  // check z-range and decide which transport will be done
  // Linear + multiple scattering in Velo region
  // Parabolic approximation out of Velo region
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = (*iterP)->pointOnTrack();
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
  }
  
  // ipz = 1 upstream tracks; ipz = -1 downstream tracks
  
  StatusCode sctrans = StatusCode::SUCCESS;
  
  // check if the track is already in the region of Velo
  if ( zr <=  950.0 ){
    sctrans = m_pVeloTransporter->transport(iterP, znew, 
                                            transParticle);
  }
  else if ( zl >= 950.0 ){
    // the track is in the magnetic field region
    sctrans = m_pMFTransporter->transport(iterP, znew, 
                                          transParticle);  
  }
  else {
    // the track cross both regions
    // the transport in magnetic field will stop at the 950*mm limit
    if ( ipz == 1) {
      sctrans = m_pMFTransporter->transport(iterP, 950.0, 
                                            transParticle);
      sctrans = m_pVeloTransporter->transport(transParticle, znew, 
                                              transParticle);
    }
    else{
      sctrans = m_pVeloTransporter->transport(iterP, 950.0, 
                                              transParticle);
      sctrans = m_pMFTransporter->transport(transParticle, znew, 
                                            transParticle);
    }
  }
  return sctrans;
}
//=============================================================================
//  Transport linear in Velo region, parabolic in field region
//=============================================================================
StatusCode CombinedTransporter::transport(const Particle & iterP, 
                                          double znew,
                                          Particle & transParticle){
  MsgStream log(msgSvc(), name());
  
  //Particle *workParticle = const_cast<Particle *> (&icand);
  
  // check z-range and decide which transport will be done
  // Linear + multiple scattering in Velo region
  // Parabolic approximation out of Velo region
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = iterP.pointOnTrack();
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
  }
  
  // ipz = 1 upstream tracks; ipz = -1 downstream tracks
  
  StatusCode sctrans = StatusCode::SUCCESS;
  
  // check if the track is already in the region of Velo
  if ( zr <=  950.0 ){
    sctrans = m_pVeloTransporter->transport(iterP, znew, 
                                            transParticle);
  }
  else if ( zl >= 950.0 ){
    // the track is in the magnetic field region
    sctrans = m_pMFTransporter->transport(iterP, znew, 
                                          transParticle);  
  }
  else {
    // the track cross both regions
    // the transport in magnetic field will stop at the 950*mm limit
    if ( ipz == 1) {
      sctrans = m_pMFTransporter->transport(iterP, 950.0, 
                                            transParticle);
      sctrans = m_pVeloTransporter->transport(transParticle, znew, 
                                              transParticle);
    }
    else{
      sctrans = m_pVeloTransporter->transport(iterP, 950.0, 
                                              transParticle);
      sctrans = m_pMFTransporter->transport(transParticle, znew, 
                                            transParticle);
    }
  }
  return sctrans; 
}
//=============================================================================
//  Transport linear in Velo region, parabolic in field region
//=============================================================================
StatusCode CombinedTransporter::transport(Particle & iterP, 
                                          double znew,
                                          Particle & transParticle){
  MsgStream log(msgSvc(), name());
  
  // Particle *workParticle = (&icand);
  
  
  // check z-range and decide which transport will be done
  // Linear + multiple scattering in Velo region
  // Parabolic approximation out of Velo region
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = iterP.pointOnTrack();
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
  }
  
  // ipz = 1 upstream tracks; ipz = -1 downstream tracks
  
  StatusCode sctrans = StatusCode::SUCCESS;
  
  // check if the track is already in the region of Velo
  if ( zr <=  950.0 ){
    sctrans = m_pVeloTransporter->transport(iterP, znew, 
                                            transParticle);
  }
  else if ( zl >= 950.0 ){
    // the track is in the magnetic field region
    sctrans = m_pMFTransporter->transport(iterP, znew, 
                                          transParticle);  
  }
  else {
    // the track cross both regions
    // the transport in magnetic field will stop at the 950*mm limit
    if ( ipz == 1) {
      sctrans = m_pMFTransporter->transport(iterP, 950.0, 
                                            transParticle);
      sctrans = m_pVeloTransporter->transport(transParticle, znew, 
                                              transParticle);
    }
    else{
      sctrans = m_pVeloTransporter->transport(iterP, 950.0, 
                                              transParticle);
      sctrans = m_pMFTransporter->transport(transParticle, znew, 
                                            transParticle);
    }
  }
  return sctrans;    
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode CombinedTransporter::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> CombinedTransporter::finalize" << endreq;
  
  return StatusCode::SUCCESS;
}
