// $Id: MCParticleMaker.cpp,v 1.4 2004-03-25 16:44:43 pkoppenb Exp $
// Include files 

#include <memory>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/Particle.h"

#include "MCTools/IMCDecayFinder.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

// local
#include "MCParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleMaker
//
// Description: This tool fills the particle class with information from
// MCParticles and places it in the Transient Event Store
// 03/10/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MCParticleMaker>          s_factory ;
const        IToolFactory& MCParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleMaker::MCParticleMaker( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name , parent ), 
    m_ppSvc(0), 
    m_EDS(0),
    m_pMCDecFinder(0)
  {
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Declare properties
  declareProperty( "InputLocation", m_input = MCParticleLocation::Default);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  declareProperty( "OnlyReconstructable", m_onlyReconstructable = false );
  declareProperty( "OnlyReconstructed",   m_onlyReconstructed   = false );
  declareProperty( "UseReconstructedCovariance", 
                   m_useReconstructedCovariance = false );
  declareProperty( "SmearParticle",   m_smearParticle = true );
  declareProperty( "IpErrorC0",   m_ipErrorC0 = 0.0173*mm );
  declareProperty( "IpErrorC1",   m_ipErrorC1 = 0.0265*mm );
  declareProperty( "SlopeError",  m_slopeError = 0.4*mrad );
  declareProperty( "MomError",    m_momError = 0.004 );

  
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleMaker::~MCParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCParticleMaker::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> MCParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }  

  IRndmGenSvc* r;
  sc = service("RndmGenSvc", r, true);
  if( sc.isFailure() || m_ranGauss.initialize(r,Rndm::Gauss(0,1)).isFailure()){
    log << MSG::FATAL << "    Unable to locate RndmGenSvc "
        << endreq;
    return sc;
  }  

  sc = toolSvc()->retrieveTool("MCDecayFinder", m_pMCDecFinder, this);
  if(sc.isFailure()){
    log << MSG::FATAL << " Unable to retrieve MCDecayFinder tool" << endreq;
    return sc;
  }

  // check for consistentcy of options
  if (m_useReconstructedCovariance && !m_onlyReconstructed ) {
    log << MSG::FATAL << " instructed to use covariance matrix of"
        << " reconstructed protoParticle"
        << " but told to also make particles wich are not reconstructed "
        << " please fix you configuration " << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_particleNames.size() == 0) {
    log << MSG::ERROR << " ParticleNames is empty. "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::const_iterator iName;
  for ( iName = m_particleNames.begin(); 
        iName != m_particleNames.end(); ++iName ) {
    ParticleProperty* partProp = m_ppSvc->find( *iName );
    if ( 0 == partProp )   {
      log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
          << *iName << "\" " << endreq;
      return StatusCode::FAILURE;
    }
    m_ids.push_back(partProp->jetsetID());
    // Print Debug message:
    log << MSG::DEBUG << " Particle Requested: Name = " << (*iName) 
        << " PID = " << partProp->jetsetID() << endreq;
  }
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* MCParticleMaker::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  log( msgSvc(), name() );
  
  log << MSG::DEBUG 
      << "==> MCParticleMaker::makeParticles() is running." 
      << endreq;
  
  SmartDataPtr<MCParticles> candidates ( eventSvc(),m_input);
  if ( !candidates || (0 == candidates->size()) ) { 
    log << MSG::INFO << "    No MCParticles retrieved from"  
        << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Log number of MCPartCandidates retrieved
  log << MSG::INFO << "    Number of MCParticles retrieved   = "
      << candidates->size() << endreq;

  std::vector<const MCParticle*> list;
  if( m_onlyDecayProducts) {
    const MCParticle* imc = 0;
    while( m_pMCDecFinder -> findDecay (candidates, imc) ) list.push_back(imc);
  } else {
    MCParticles::const_iterator icand;
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      list.push_back(*icand);    
    }
  }
      
  std::vector<const MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){
    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) 
         == m_ids.end()) continue;
    if ( m_onlyReconstructable && !reconstructable(**icand) )  continue;
    // covariance is in (x,y,z,sx,sy,p)
    std::auto_ptr<HepSymMatrix> covariance(0);
    if ( m_onlyReconstructed) {
      const Particle *measurement = reconstructed(**icand);
      if (measurement == 0)  continue;
      if ( m_useReconstructedCovariance ) {
#ifdef WIN32
        covariance=std::auto_ptr<HepSymMatrix>(fetchCovariance(*measurement));
#else
        covariance.reset( fetchCovariance(*measurement) );
#endif
      }
    }
    if (covariance.get() == 0)
#ifdef WIN32
      covariance = std::auto_ptr<HepSymMatrix>( generateCovariance((*icand)->momentum()) );
#else
      covariance.reset( generateCovariance((*icand)->momentum()) );
#endif
    std::auto_ptr<Particle> particle( new Particle() );
    StatusCode sc = fillParticle( **icand, *particle, *covariance);
    if(sc.isFailure()) continue;
    parts.push_back(particle.release());
    log << MSG::DEBUG << "==> MCParticleMaker::added a particle" 
        << endmsg;
  }
  return StatusCode::SUCCESS;
    
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode 
MCParticleMaker::fillParticle( const MCParticle& mc,
                                       Particle& particle,
                                       const HepSymMatrix& cov)
{
  MsgStream  log( msgSvc(), name() );

  // Start filling the particle:     
  particle.setOrigin(&mc);
  particle.setParticleID( mc.particleID() );
  particle.setConfLevel( 1 );
  particle.setIsResonance(false);  // Already in constructor
  
  // Set pointOnTrackErr: take typical errors at vertex...
  HepLorentzVector mom = mc.momentum();
  const MCVertex* vtx = mc.originVertex();
  HepPoint3D pos = vtx->position();
  if (m_smearParticle) {
    HepSymMatrix D(cov);
    HepMatrix U = diagonalize(&D);
    HepVector deviates(6,0); //  x,y,z,sx,sy,p
    for (int i=1;i<=6;++i) deviates(i) = m_ranGauss.shoot()*sqrt(D(i,i));
    deviates = U*deviates;
    pos.setX(  pos.x()  + deviates(1) );
    pos.setY(  pos.y()  + deviates(2) );
    pos.setZ(  pos.z()  + deviates(3) );
    double sx = mom.px()/mom.pz() + deviates(4);
    double sy = mom.py()/mom.pz() + deviates(5);
    double  p = mom.vect().mag()  + deviates(6);
    double  m = mom.m();
    double pz = p / sqrt(1+sx*sx+sy*sy);
    mom = HepLorentzVector( sx*pz,sy*pz,pz,sqrt(m*m+p*p) );
  }
  // NOTE: must set position and momentum before covariance matrix
  //       as otherwise the conversions on the covariance cannot be done
  particle.setPointOnTrack( pos );
  particle.setMomentum(mom);
  particle.setMass(mom.m());
  particle.setMassErr(0.0); // For the moment but already in constructor

  particle.setPointOnTrackErr(cov.sub(1,3));
  particle.setSlopesMomErr(cov.sub(4,6));
  HepMatrix c(3,3,0.0);
  for (int i=0;i<c.num_row();++i) {
    for (int j=0;j<c.num_col();++j) {
      c(1+i,1+j) = cov(1+i,4+j);
    }
  }
  particle.setPosSlopesCorr(c);

  // Print out informations 
  // particle.fillStream(cout) << endl;
    
  return StatusCode::SUCCESS;
}

//=====================================================================
// reconstructable
//=====================================================================
bool 
MCParticleMaker::reconstructable(const MCParticle& icand) const
{
  // FIXME: use Olivier Callot's associator...
  return true;
}

//=====================================================================
// reconstructed
//=====================================================================
const Particle *
MCParticleMaker::reconstructed(const MCParticle& icand) const
{
  // FIXME: use associator...
  return 0;
}

//=====================================================================
// fetchCovariance
//=====================================================================
HepSymMatrix *
MCParticleMaker::fetchCovariance(const Particle& p ) const
{
  HepSymMatrix *c = new HepSymMatrix(6,0.0);
  c->sub(1,p.pointOnTrackErr());
  c->sub(4,p.momentumErr());
  HepMatrix m = p.posSlopesCorr();
  for (int i=0;i<m.num_row();++i) {
    for (int j=0;j<m.num_col();++j) {
      (*c)(1+i,4+j) = m(1+i,1+j);
    }
  }
  return c;
}

//=====================================================================
// generateCovariance
//=====================================================================
HepSymMatrix *
MCParticleMaker::generateCovariance(const HepLorentzVector& p) const
{
  HepSymMatrix *c = new HepSymMatrix(6,0.0);

  double sip = m_ipErrorC0 + m_ipErrorC1/(p.perp()/GeV); 
  (*c)(1,1) = sip*sip/2;
  (*c)(2,2) = (*c)(1,1);
  (*c)(3,3) = 0;
  
  (*c)(4,4) = m_slopeError*m_slopeError;
  (*c)(5,5) = (*c)(4,4);
  (*c)(6,6) = m_momError*m_momError*p*p;
  return c;
}
