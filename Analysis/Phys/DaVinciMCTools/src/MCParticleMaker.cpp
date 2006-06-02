// $Id: MCParticleMaker.cpp,v 1.25 2006-06-02 11:07:39 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
#include <memory>
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
#include "Kernel/IMCDecayFinder.h"
#include "DaVinciMCTools/IMCReconstructible.h"
#include "LHCbMath/MatrixManip.h"
// ============================================================================
// local
// ============================================================================
#include "MCParticleMaker.h"
// ============================================================================

/*-----------------------------------------------------------------------------
 * Implementation file for class : MCParticleMaker
 *
 * Description: This tool fills the particle class with information from
 * MCParticles and places it in the Transient Event Store
 *
 * 03/10/2002 : Gerhard Raven
 * May 2006: Gabriele Balbi & Stefania Vecchi
 *-----------------------------------------------------------------------------
 */
/// Returns false if is the MCPArticle is one of these stable particles (mu+/-,e+/-,pi+/-,K+/-,p/antip, gamma); true otherwise 
struct IsUnstable : std::unary_function<LHCb::MCParticle*,bool> {   
  bool operator()(const LHCb::MCParticle *p) const
  { int pid = p->particleID().abspid();
  return (pid!= 11 && pid!= 13 && pid!= 211 && pid!= 321 && pid!=2212 && pid!=22);
  }
};


// Declaration of the Tool Factory
static const  ToolFactory<MCParticleMaker>          s_factory ;
const        IToolFactory& MCParticleMakerFactory = s_factory ;

using namespace Gaudi::Units;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleMaker::MCParticleMaker
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : MCParticleMakerBase ( type, name , parent )
  ,m_onlyReconstructible(false)
  ,m_onlyReconstructed(false)
  ,m_ppSvc(0) 
  ,m_pMCDecFinder(0)
  ,m_reco(0)
{
  // Declaring implemented interface  
  declareInterface<IParticleMaker>(this);
  // Declare properties
  declareProperty( "InputLocation", m_input = LHCb::MCParticleLocation::Default);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  // if true also m_onlyDecayProducts HAS to be true
  declareProperty( "OnlyStableDecayProducts", m_onlyStableDecayProducts = false);
  /// @todo: restore option OnlyReconstructible when method is implemented
  //  declareProperty( "OnlyReconstructible", m_onlyReconstructible = false );
  /// @todo: restore option OnlyReconstructed when method is implemented
  //  declareProperty( "OnlyReconstructed",   m_onlyReconstructed   = false );
  declareProperty( "UseReconstructedCovariance", m_useReconstructedCovariance = false );

  declareProperty( "OnlyReconstructible",m_requireReco = true  );// Only reconstructible
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleMaker::~MCParticleMaker( ) { };


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCParticleMaker::initialize() 
{
  
  StatusCode sc = MCParticleMakerBase::initialize();
  if ( sc.isFailure() ) { return sc ; }

  debug() << "==> MCParticleMaker:Initialising" << endmsg;

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  debug() << "Looking for Particle Property Service." << endmsg;
  
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc",true);
   
  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this);
  
  if ( m_requireReco ){
    m_reco = tool<IMCReconstructible>("MCReconstructible");
    info() << "Will only look at reconstructible particles" << endmsg ;
  }
  
  // check for consistentcy of options
  if (m_useReconstructedCovariance && !m_onlyReconstructed ) {
    fatal() << "Instructed to use covariance matrix of"
            << " reconstructed protoParticle"
            << " but told to also make particles wich are not reconstructed "
            << " please fix you configuration " << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (!m_onlyDecayProducts && m_onlyStableDecayProducts ) {
    fatal() << "Instructed to use only Stable Decay Products"
            << " and NOT Decay Products. "
            << " please fix you configuration " << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_particleNames.size() == 0) {
    err() << " ParticleNames is empty. "  
          << "Please, initialize it in your job options file" <<  endmsg;
    return StatusCode::FAILURE;
  }
  
  std::vector<std::string>::const_iterator iName;
  for ( iName = m_particleNames.begin(); 
        iName != m_particleNames.end(); ++iName ) {
    ParticleProperty* partProp = m_ppSvc->find( *iName );
    if ( 0 == partProp )   {
      err() << "Cannot retrieve properties for particle \"" 
            << *iName << "\" " << endmsg;
      return StatusCode::FAILURE;
    }
    m_ids.push_back(partProp->jetsetID());
    // Print Debug message:
    debug() << " Particle Requested: Name = " << (*iName) 
            << " PID = " << partProp->jetsetID() << endmsg;
  }
  
  debug()<< "Correlation matrix rho "<< rho() <<endmsg;
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleMaker::makeParticles( LHCb::Particle::ConstVector & parts ) 
{
  
  debug() << "==> MCParticleMaker::makeParticles() is running." << endmsg;
  
  LHCb::MCParticle::Container* candidates = get<LHCb::MCParticle::Container>( m_input);
  //  LHCb::Particle::Container* Part_candidates = get<LHCb::Particle::Container>(LHCb::MCParticleMakerLocation::Default);
  if ( !candidates || (0 == candidates->size()) ){//|| !Part_candidates ) { 
    debug() << "    No MCParticles retrieved from" << m_input << " or Particle contained error" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  debug() << "    Number of MCParticles retrieved   = " << candidates->size() << endmsg;

  std::vector<const LHCb::MCParticle*> list;
  if( m_onlyDecayProducts) {
    const LHCb::MCParticle* imc = 0;
    if( m_onlyStableDecayProducts) {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) )  list = getFinalState(*imc);  
    } else {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) ) list.push_back(imc);
    }
  } else {
    list.resize( candidates->size() );
    std::copy(candidates->begin(), candidates->end(), list.begin() );
  }

  debug() << "    List contains   = " << list.size() << " MCParticles" << endmsg;

  //  LinkerWithKey<LHCb::MCParticle,LHCb::Particle> myLink(evtSvc(), msgSvc(), Part_candidates);
  // LinkerWithKey<LHCb::Particle,LHCb::MCParticle> myLink(evtSvc(), msgSvc(), LHCb::MCParticleLocation::Default);

      
  std::vector<const LHCb::MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){

    debug() << " New candidate of pid " << (*icand)->particleID().pid() << endmsg ;

    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) == m_ids.end()) continue;
    if ( m_onlyReconstructible && !reconstructible(**icand) )  continue;
    if (( m_requireReco ) && ( NULL!=m_reco ) && ( m_reco->reconstructible(*icand)==0)) {
      if ( (*icand)->p() > 1000 ) debug() << (*icand)->particleID().pid() << " not reconstructible " << endmsg ;
      continue ;
    } else debug() << (*icand)->particleID().pid() << " reconstructible " << endmsg ;
    
    // covariance is in (x,y,z,px,py,pz,m) order
    debug()<< "Build Covariance Matrix "<<endmsg;
    Gaudi::SymMatrix7x7 covariance;
    if ( m_onlyReconstructed) {
      const LHCb::Particle *measurement = reconstructed(**icand);
      if ( m_useReconstructedCovariance && measurement !=0 ) {
        fetchCovariance(*measurement,covariance);
      }else{
        generateCovariance((*icand)->momentum(),covariance);
      } 
    } else {
      generateCovariance((*icand)->momentum(),covariance);
    }
    debug() << "Got covariance matrix" << endmsg ;
    //std::auto_ptr<LHCb::Particle> particle( new LHCb::Particle() );
    LHCb::Particle* particle( new LHCb::Particle() );
    StatusCode sc = fillParticle( **icand, *particle, covariance);
    if(sc.isFailure()) continue;
    //    parts.push_back(particle.release());
    parts.push_back(particle);
    debug() << "Done candidate of pid " << (*icand)->particleID().pid() << endmsg ;

    //          myLink.link((*icand),particle, 1.);
    //      myLink.link(particle,(*icand), 1.);
    //    debug()<< " Created Linker MCParticle-Particle"<<endmsg;
  }

  debug() << " ==> MCParticleMaker created " << parts.size() << " particle in the desktop " << endmsg;
  return StatusCode::SUCCESS;
    
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode 
MCParticleMaker::fillParticle
( const LHCb::MCParticle& mc,
  LHCb::Particle& particle,
  const Gaudi::SymMatrix7x7& cov)
{
  /// use the function from the base class 
  return MCParticleMakerBase::fillParticle 
    ( mc.momentum()      , 
      mc.originVertex()->position() , 
      mc.particleID()    , 
      cov                , 
      particle           ) ;
}
//=====================================================================
// reconstructible
//=====================================================================
bool 
MCParticleMaker::reconstructible(const LHCb::MCParticle& icand) const
{
  /// @todo Use Olivier Callot's associator in reconstructible method
  verbose() << "reconstructible?" << endmsg ;
  return (icand.momentum().rho()>0);
}

//=====================================================================
// reconstructed
//=====================================================================
const LHCb::Particle *
MCParticleMaker::reconstructed(const LHCb::MCParticle& icand) const
{
  /// @todo Use Olivier Callot's associator in reconstructed method
  verbose() << "reconstructed?" << endmsg ;
  if (icand.momentum().rho()>0) return NULL;
  else  return NULL;
}

//=====================================================================
// fetchCovariance
//=====================================================================
StatusCode MCParticleMaker::fetchCovariance(const LHCb::Particle& p ,Gaudi::SymMatrix7x7 &c) {
  debug()<<"Fetch covariance"<<endmsg;

  Gaudi::Matrix7x7 cTmp = Gaudi::Matrix7x7();
  cTmp.Place_at(p.posCovMatrix(),0,0);
  cTmp.Place_at(p.momCovMatrix(),3,3);
  cTmp.Place_at(p.posMomCovMatrix(), 0, 3);
  cTmp.Place_at(ROOT::Math::Transpose(p.posMomCovMatrix()), 3, 0); /// @todo Check that this actually works!
  c = Gaudi::Math::Symmetrize(cTmp);

  return StatusCode::SUCCESS;
}

//=====================================================================
// getFinalState
//=====================================================================
/// method to retrieve the list of the STABLE DECAY PRODUCTS of the decay channel defined in JobOption
  std::vector<const LHCb::MCParticle*> MCParticleMaker::getFinalState(const LHCb::MCParticle& m){
  typedef std::vector<const LHCb::MCParticle*> PV;
  PV pv;
  pv.push_back(&m);
  for (;;) {
    PV::iterator i = find_if(pv.begin(),pv.end(),IsUnstable());
    if ( i == pv.end() ) break;
    typedef const SmartRefVector<LHCb::MCVertex> SRVV;
    SRVV& vv = (*i)->endVertices(); // non zero by construction
    pv.erase(i);
    for (SRVV::const_iterator j=vv.begin();j!=vv.end();++j) {
      typedef const SmartRefVector<LHCb::MCParticle> SRVP;
      SRVP& vp = (*j)->products();
      for (SRVP::const_iterator k=vp.begin();k!=vp.end();++k) {
        pv.push_back(*k);
      }
    }
  }
  return pv;
}


// ============================================================================
// The END 
// ============================================================================
