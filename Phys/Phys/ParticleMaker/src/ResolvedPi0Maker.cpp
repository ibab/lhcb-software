// $Id
// $Id: ResolvedPi0Maker.cpp,v 1.8 2009-04-21 15:26:14 odescham Exp $
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h" 
// local
#include "ResolvedPi0Maker.h"

/** @file 
 *  
 *  Implementation file for class : ResolvedPi0Maker
 *  Almost identical to PhotonParticleMaker
 * 
 *  replace ComonParticles/ResolvedPi0Alg
 *
 *  @date 2006-08-25 
 *  @author Olivier Deschamps odescham@in2p3.fr
 *
 */

// ============================================================================
/** @var ResolvedPi0MakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_TOOL_FACTORY( ResolvedPi0Maker );

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
ResolvedPi0Maker::ResolvedPi0Maker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool           ( type, name , parent ) 
    , m_point            () 
    , m_pointErr         ()
    , m_photonMakerType  ()
    , m_photonMaker      ()
    , m_singlePhotonUse  ()
    , m_independantPhotons()
  // cut
    , m_pi0MassWin ()
    , m_pi0PtCut   ()
{
  declareProperty( "SinglePhotonUse"   , m_singlePhotonUse  = false);
  declareProperty( "IndependantPhotons", m_independantPhotons= false);
  declareProperty ( "PhotonMakerType"  , m_photonMakerType = "PhotonMaker") ;
  // Filter 
  declareProperty( "MassWindow"     , m_pi0MassWin = 30. * Gaudi::Units::MeV);
  declareProperty( "PtCut"          , m_pi0PtCut = 0. * Gaudi::Units::MeV);
  declareProperty( "Particle"       , m_part = "pi0");
  //
  m_point = Gaudi::XYZPoint();
  m_pointErr = Gaudi::SymMatrix3x3();
  // declare new interface
  declareInterface<ICaloParticleMaker> (this);
};
// ============================================================================



// ============================================================================
/// destructor
// ============================================================================
ResolvedPi0Maker::~ResolvedPi0Maker() {};
// ============================================================================

StatusCode ResolvedPi0Maker::initialize    ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize GaudiTool",sc);}

  // ParticleProperty
  const LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"	  << endreq;
    return sc;
  }
  const LHCb::ParticleProperty* partProp;
  partProp  = ppSvc->find( m_part );
  if(partProp == NULL){
    Error("Requested particle '" + m_part + "' is unknown").ignore();
    return StatusCode::FAILURE;
  }
  m_Id      = (*partProp).pdgID().pid();
  m_pi0Mass = (*partProp).mass();
  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;
  

  // Retrieve PhotonMaker tool
  m_photonMaker = tool< ICaloParticleMaker>( m_photonMakerType ,  this ) ;
  if( 0 == m_photonMaker ) {
    fatal() << "    Unable to locate PhotonMaker tool "	  << endreq;
    return StatusCode::FAILURE ; 
  }
  

  return StatusCode::SUCCESS ;
};
// ============================================================================

StatusCode ResolvedPi0Maker::finalize      ()
{
  info() << " - ResolvedPi0Maker Summary -----" << endreq;
  info() << " Created : " << (float) m_count[1]/m_count[0] << " Resolved " << m_part << "per event" << endreq;
  info() << " --------------------------------" << endreq;
  // finalize the base class
  return GaudiTool::finalize ();
};

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode ResolvedPi0Maker::makeParticles (LHCb::Particle::Vector & pi0s )
{
  // avoid some long names
  if( !pi0s.empty() ){
    Warning( "makeParticles(): extend non-empty vector of Particles" ).ignore() ; 
  }
  
  // make photon data
  
  m_photonMaker->setPoint(m_point, m_pointErr);
  
  LHCb::Particle::Vector photons;
  StatusCode sc = m_photonMaker->makeParticles(photons);
  if (!sc) return sc;
  if( 0 == photons.size() ) { 
    Warning("PhotonMaker return empty container - No resolved "+ m_part + " to be created").ignore();
    return StatusCode::SUCCESS; 
  }

  unsigned long nGamma = photons.size() ;
  unsigned long nPi0 = 0 ;
  unsigned long nSkip = 0 ;
  m_count[0] += 1;


  std::vector< std::pair< LHCb::CaloParticle ,bool> > orderedPhotons;

  // sort photon candidates according to Pt reconstructed
  for ( unsigned int i=0 ; i < nGamma ; ++i ){
    double ptmax=-1.;
    LHCb::Particle::Vector::iterator part,ip;
    for ( ip = photons.begin() ; ip != photons.end() ; ++ip ){
      // Convert to CaloMomentum
      LHCb::CaloMomentum photon( (*ip)->proto() ,m_point,m_pointErr);
      if (photon.pt() >= ptmax){ 
        ptmax=photon.pt();     
        part=ip;
      }
    }
    LHCb::CaloParticle photonMax( *part ,m_point , m_pointErr);
    orderedPhotons.push_back( std::pair< LHCb::CaloParticle , bool > (photonMax,false) );
    photons.erase(part);
  }
  if ( 0 != photons.size() ) {
    err() <<"Photon candidate sorting was wrong..."<<endreq;
    return StatusCode::FAILURE;
  }
  
  // Make Pi0 candidates from sorted list
  std::vector< std::pair< LHCb::CaloParticle ,bool> >::iterator ip1, ip2;
  for( ip1 = orderedPhotons.begin() ; ip1 != orderedPhotons.end()-1 ; ++ip1 ) {
    if ( m_singlePhotonUse && (*ip1).second )continue;
    for( ip2 = ip1+1 ; ip2 != orderedPhotons.end() ; ++ip2 ) {
      if ( m_singlePhotonUse && (*ip2).second ) continue;

      if( !selPi0( (*ip1).first , (*ip2).first) )continue;

      // create pi0
      LHCb::Particle* pi0 = new LHCb::Particle(); 
      nPi0++;
      StatusCode sc = makePi0( (*ip1).first , (*ip2).first , pi0);
      if(StatusCode::SUCCESS != sc ){
        delete pi0;
        nSkip++;
        warning() << "Unable to fill Resolved " << m_part << " parameters, skip particle [" << nSkip << "]"<< endreq;
        continue;
      }
      // fill container
      pi0s.push_back(pi0);
      m_count[1] += 1;

      // Flag the photons as used to built a pi0
      (*ip1).second = true ;
      (*ip2).second = true ;

      // print out
      LHCb::Particle* g1 = ((*ip1).first).particle();
      LHCb::Particle* g2 = ((*ip2).first).particle();
      verbose() << " ---- Resolved " << m_part <<" found [" << nPi0 << "]"<< endreq;
      verbose() << "Point   : " << pi0->referencePoint() << endreq;
      verbose() << "Pt("<<m_part<<") : "  << pi0->momentum().Pt() << endreq;
      verbose() << "Pt(g1)  : "  << g1->momentum().Pt() << endreq;
      verbose() << "Pt(g2)  : "  << g2->momentum().Pt() << endreq;
      verbose() << "CL("<<m_part<<") : "  << pi0->confLevel() << endreq;
      verbose() << "CL(g1)  : "  << g1->confLevel() << endreq;
      verbose() << "CL(g2)  : "  << g2->confLevel() << endreq;
      verbose() << "Mass    : "  << pi0->momentum().M()  << endreq;
    }
  }


  // Clean the unused photons
  unsigned long nDel = 0;
  std::vector< std::pair< LHCb::CaloParticle ,bool> >::iterator ip ;
  for ( ip = orderedPhotons.begin() ; ip != orderedPhotons.end() ; ++ip ){
    if( !(*ip).second || m_independantPhotons ){
      nDel++;
      delete ( (*ip).first ).particle();
    }
  }

  debug() << " " << endreq;
  debug() << "-----------------------" << endreq;
  debug() << " Filtered and created :" << endreq;
  debug() << " --> " << nPi0 << " Resolved " << m_part <<"s " << endreq;
  debug() << " --> " << nGamma-nDel <<" photons have been used among the " << nGamma << " selected " << endreq;
  debug() << " Skipped " << m_part <<" : " << nSkip << endreq;
  debug() << "-----------------------" << endreq;
  return StatusCode::SUCCESS ;
};


StatusCode ResolvedPi0Maker::makePi0(LHCb::CaloParticle g1,  LHCb::CaloParticle g2,
                                     LHCb::Particle* pi0){
  pi0->setParticleID( LHCb::ParticleID (m_Id) );

  LHCb::Particle* gg1;
  LHCb::Particle* gg2;
  if(m_independantPhotons ){
    gg1 = (g1.particle())->clone();
    gg2 = (g2.particle())->clone();
  }else{
    gg1= g1.particle();
    gg2= g2.particle();
  }
  
  pi0->setConfLevel   ( gg1->confLevel()*gg2->confLevel() ) ;
  pi0->addToDaughters ( gg1 );
  pi0->addToDaughters ( gg2 );

  // Momentum and covariance
  LHCb::CaloParticle caloPi0(pi0 , m_point , m_pointErr); 
  caloPi0.updateTree(); 
  pi0->setMeasuredMass( caloPi0.mass()     ) ;
  pi0->setMeasuredMassErr( caloPi0.emass() ) ; 

  if( caloPi0.status() !=0 ){
    warning() << "CaloParticle status/flag : " << caloPi0.status() << "/" << caloPi0.flag();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

// ============================================================================

bool ResolvedPi0Maker::selPi0(LHCb::CaloParticle g1, LHCb::CaloParticle g2){
  //
  bool isGood=false;

  LHCb::CaloParticle pi0(g1.particle() ,m_point ,m_pointErr);
  pi0.addCaloPosition(g2.particle() );

  if (fabs(pi0.mass()-m_pi0Mass) < m_pi0MassWin && pi0.pt() > m_pi0PtCut ) isGood=true;
  return isGood;

};



