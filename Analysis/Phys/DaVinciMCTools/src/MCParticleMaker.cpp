// $Id: MCParticleMaker.cpp,v 1.34 2007-08-24 07:27:22 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.33  2007/08/22 16:51:10  pkoppenb
// No constructor from key
//
// Revision 1.32  2007/06/15 16:18:38  jpalac
// Fix includes for interfaces formerly coming from Kernel, now in MCInterfaces.
//
// Revision 1.31  2007/01/12 14:03:52  ranjard
// v9r0 - use Gaudi v19r0
//
// Revision 1.30  2006/10/11 14:19:19  jpalac
// Major restructuring, increase version to v8r0
//
// Revision 1.29  2006/10/04 11:58:30  ibelyaev
//  add (optional, switched-off in default) relation tables
//
// Revision 1.28  2006/09/26 10:45:56  ibelyaev
//  make MCParticleMakerBase vizible outside package
//
// ============================================================================
// Include files
// ============================================================================
#include <memory>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "LHCbMath/MatrixManip.h"
// ============================================================================
// local
// ============================================================================
#include "MCParticleMaker.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Relation1D.h"
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
/** Returns false if is the MCPArticle is one of these stable particles 
 *  (mu+/-,e+/-,pi+/-,K+/-,p/antip, gamma); true otherwise 
 */
struct IsUnstable : std::unary_function<LHCb::MCParticle*,bool> {   
  bool operator()(const LHCb::MCParticle *p) const
  { int pid = p->particleID().abspid();
  return (pid!= 11 && pid!= 13 && pid!= 211 && pid!= 321 && pid!=2212 && pid!=22);
  }
};


// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( MCParticleMaker )

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
  declareProperty( "OnlyReconstructible",m_requireReco = true  );// Only reconstructible particles
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleMaker::~MCParticleMaker( ) { }

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
  
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true);
   
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
    const LHCb::ParticleProperty* partProp = m_ppSvc->find( *iName );
    if ( 0 == partProp )   {
      err() << "Cannot retrieve properties for particle \"" 
            << *iName << "\" " << endmsg;
      return StatusCode::FAILURE;
    }
    m_ids.push_back(partProp->particleID().pid());
    debug() << " Particle Requested: Name = " << (*iName) 
            << " PID = " << partProp->particleID().pid() << endmsg;
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
  
  typedef LHCb::Relation1D<LHCb::Particle,LHCb::MCParticle> Table ;
  Table* table = 0 ;
  if ( !outputTable().empty() ) 
  {
    table = new Table(100) ;
    put ( table , outputTable() ) ;
  }
  
  LHCb::MCParticle::Container* candidates = get<LHCb::MCParticle::Container>( m_input);
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

  std::vector<const LHCb::MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){
    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) == m_ids.end()) continue;
    if ( m_onlyReconstructible && !reconstructible(**icand) )  continue;
    if (( m_requireReco ) && ( NULL!=m_reco ) && ( m_reco->reconstructible(*icand)==0)) {
      if ( (*icand)->p() > 1000 ) debug() << (*icand)->particleID().pid() << " not reconstructible " << endmsg ;
      continue ;
    } else debug() << (*icand)->particleID().pid() << " reconstructible " << endmsg ;
    
    // covariance is in (x,y,z,px,py,pz,m) order
    debug()<< "Build Covariance Matrix for candidate of candidate of pid " << (*icand)->particleID().pid() << endmsg ;
    Gaudi::SymMatrix7x7 covariance;
    if ( m_onlyReconstructed) {
      const LHCb::Particle *measurement = reconstructed(**icand);
      if ( m_useReconstructedCovariance && measurement !=0 ) {
        covariance = measurement->covMatrix();  /// get reconstructed covariance matrix
      }else{
        generateCovariance((*icand)->momentum(),covariance); /// generate a realistic covariance matrix
      } 
    } else {
      generateCovariance((*icand)->momentum(),covariance); /// generate a realistic covariance matrix
    }

    KeyedObject<int>::key_type  mcKey=(*icand)->key(); /// get the MCParticle key for a later link to Particle

    //    std::auto_ptr<LHCb::Particle> particle( new LHCb::Particle(mcKey) );
    LHCb::Particle* particle = new LHCb::Particle((*icand)->particleID(), 
                                                  mcKey) ;
    //    particle->setKeyedObjectKey(mcKey);

    StatusCode sc = fillParticle( **icand, *particle, covariance); /// fill the particle contents
    if(sc.isFailure()) continue;
    //parts.push_back(particle.release());
    
    // FILL relation table 
    if ( 0 != table ) { table-> i_relate ( particle , *icand ) ; }
    
    parts.push_back(particle);    
    
    debug() << "Done candidate of pid " << (*icand)->particleID().pid() << " and Key " << particle->key() << endmsg ;
  }
  
  debug() << " ==> MCParticleMaker created " << parts.size() << " particle in the desktop " << endmsg;
  
  // some decorations 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  {
    counter("#particles") += parts.size() ;
    if ( 0 != table ) { counter("#links") += table->relations().size() ; }
  }
  //
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
