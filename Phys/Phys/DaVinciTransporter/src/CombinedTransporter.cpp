// $Id: CombinedTransporter.cpp,v 1.2 2004-12-16 14:35:42 pkoppenb Exp $
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
  : GaudiTool( type, name , parent ), 
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
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  info() << "CombinedTransporter Initialization starting..." << endreq;
  
  // This tool needs to use internally the ToolSvc to retrieve the
  // transporter tool   
  m_pVeloTransporter = tool<IParticleTransporter>(pVeloTransporterName,this);

  if (!m_pVeloTransporter) {
    fatal()  << "Unable to retrieve VeloTransporter  tool" << endreq;
    return  StatusCode::FAILURE;
  }

  m_pMFTransporter = tool<IParticleTransporter>(pMFTransporterName,this);
  if(!m_pMFTransporter) {
    fatal()  << "Unable to retrieve MFTransporter  tool" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Transport linear in Velo region, parabolic in field region
//=============================================================================
StatusCode CombinedTransporter::transport(ParticleVector::const_iterator& iterP, 
                                          const double znew,
                                          Particle& transParticle){
  
  const Particle workParticle = *(*iterP);
  return transport(workParticle,znew,transParticle) ;
  
}
//=============================================================================
//  Transport linear in Velo region, parabolic in field region
//=============================================================================
StatusCode CombinedTransporter::transport(const Particle& iterP, 
                                          const double znew,
                                          Particle& transParticle){
  
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
    debug() << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
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
