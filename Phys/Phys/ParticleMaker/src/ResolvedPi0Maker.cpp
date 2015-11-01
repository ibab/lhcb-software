// $Id
// $Id: ResolvedPi0Maker.cpp,v 1.11 2010-04-14 12:42:40 odescham Exp $
// ============================================================================
// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h"
// local
#include "ResolvedPi0Maker.h"
// STL
#include <sstream>

/** @file
 *
 *  Implementation file for class : ResolvedPi0Maker
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

DECLARE_ALGORITHM_FACTORY( ResolvedPi0Maker )

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name
 *  @param parent tool parent
 */
// ============================================================================
ResolvedPi0Maker::ResolvedPi0Maker
( const std::string& name,ISvcLocator* pSvcLocator  )
: Pi0MakerBase           ( name, pSvcLocator )
  , m_photonMakerType  ()
  , m_photonMaker      (NULL)
  , m_singlePhotonUse  ()
  , m_independantPhotons()
{
  declareProperty( "SinglePhotonUse"   , m_singlePhotonUse  = false);
  declareProperty( "IndependantPhotons", m_independantPhotons= false);
  declareProperty( "PhotonMakerType"  , m_photonMakerType = "PhotonMaker") ;
  declareProperty( "MaxBalance"       , m_maxbal=-1.);
  declareProperty( "MinBalance"       , m_minbal=-1.);
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
ResolvedPi0Maker::~ResolvedPi0Maker() {}
// ============================================================================

StatusCode ResolvedPi0Maker::initialize    ()
{
  // initialize the base class
  const StatusCode sc = Pi0MakerBase::initialize();
  if ( sc.isFailure() ) { return Error(" Unable to initialize Pi0MakerBase",sc);}

  // Retrieve PhotonMaker tool
  m_photonMaker = tool< ICaloParticleMaker>( m_photonMakerType ,  this ) ;

  return sc ;
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode ResolvedPi0Maker::makeParticles (LHCb::Particle::Vector & pi0s )
{
  // avoid some long names
  if( !pi0s.empty() )
  {
    Warning( "makeParticles(): extend non-empty vector of Particles" ).ignore() ;
  }

  // make photon data
  m_photonMaker->setPoint(m_point, m_pointErr);
  if( m_setPV ){
    // .. reset origin to first PV when exists
    const LHCb::RecVertex::Range&  pvs = this->primaryVertices();
    if( pvs.size() != 0 ){
      m_photonMaker->setPoint((LHCb::Vertex*) pvs.front() );
      setPoint((LHCb::Vertex*) pvs.front() );
    }      
  }

  LHCb::Particle::Vector photons;
  StatusCode sc = m_photonMaker->makeParticles(photons);
  if (!sc) return sc;
  if ( photons.empty() ) 
  {
    return Warning( "PhotonMaker return empty container - No resolved "+ m_pid + " to be created",
                    StatusCode::SUCCESS, 0 );
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
    orderedPhotons.push_back( std::pair< LHCb::CaloParticle , bool >
                              (LHCb::CaloParticle( *part ,m_point , m_pointErr), false) );
    photons.erase(part);
  }
  if ( !photons.empty() )return Error( "Photon candidate sorting was wrong..." );

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
      const StatusCode scc = makePi0( (*ip1).first , (*ip2).first , pi0);
      if ( scc.isFailure() ){
        delete pi0;
        nSkip++;
        std::ostringstream mess;
        mess << "Unable to fill Resolved " << m_pid << " parameters, skip particle [" << nSkip << "]";
        Warning(mess.str()).ignore();
        continue;
      }
      // fill container
      pi0s.push_back(pi0);
      m_count[1] += 1;

      // Flag the photons as used to built a pi0
      (*ip1).second = true ;
      (*ip2).second = true ;


      LHCb::Particle* g1 = ((*ip1).first).particle();
      LHCb::Particle* g2 = ((*ip2).first).particle();
      // statistics
      counter("Created resolved " + m_pid) += 1;
      bool cnv1 = (g1->proto()->info(LHCb::ProtoParticle::CaloDepositID,0.) <0);
      bool cnv2 = (g2->proto()->info(LHCb::ProtoParticle::CaloDepositID,0.) <0);
      if( cnv1 && cnv2 )counter("Created resolved "+m_pid+"->(ee)(ee)")+=1;
      else if( cnv1 || cnv2)counter("Created resolved "+m_pid+"->g(ee)")+=1;
      else if( !cnv1 && !cnv2)counter("Created resolved "+m_pid+"->gg")+=1;


      // print out
      if ( msgLevel(MSG::VERBOSE))
      {
        verbose() << " ---- Resolved " << m_pid <<" found [" << nPi0 << "]"<< endmsg;
        verbose() << "Point   : " << pi0->referencePoint() << endmsg;
        verbose() << "Pt("<<m_pid<<") : "  << pi0->momentum().Pt() << endmsg;
        verbose() << "Pt(g1)  : "  << g1->momentum().Pt() << endmsg;
        verbose() << "Pt(g2)  : "  << g2->momentum().Pt() << endmsg;
        verbose() << "CL("<<m_pid<<") : "  << pi0->confLevel() << endmsg;
        verbose() << "CL(g1)  : "  << g1->confLevel() << endmsg;
        verbose() << "CL(g2)  : "  << g2->confLevel() << endmsg;
        verbose() << "Mass    : "  << pi0->momentum().M()  << endmsg;
      }
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

  if (msgLevel(MSG::DEBUG))
  {
    debug() << " " << endmsg;
    debug() << "-----------------------" << endmsg;
    debug() << " Filtered and created :" << endmsg;
    debug() << " --> " << nPi0 << " Resolved " << m_pid <<"s " << endmsg;
    debug() << " --> " << nGamma-nDel <<" photons have been used among the "
            << nGamma << " selected " << endmsg;
    debug() << " Skipped " << m_pid <<" : " << nSkip << endmsg;
    debug() << "-----------------------" << endmsg;
  }
  return sc ;
}
//=========================================================================
//  make Pi0
//=========================================================================
StatusCode ResolvedPi0Maker::makePi0(const LHCb::CaloParticle& g1,
                                     const LHCb::CaloParticle& g2,
                                     LHCb::Particle* pi0){
  pi0->setParticleID( LHCb::ParticleID (m_Id) );

  LHCb::Particle* gg1;
  LHCb::Particle* gg2;
  if(m_independantPhotons ){
    gg1 = (g1.particle())->clone();
    gg2 = (g2.particle())->clone();
    // CRJ : Are these saved or deleted later on ? Looks to me like they aren't ...
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

  if( caloPi0.status() !=0 )
  {
    std::ostringstream mess;
    mess << "CaloParticle status/flag : " << caloPi0.status() << "/" << caloPi0.flag();
    return Warning( mess.str() );
  }
  return StatusCode::SUCCESS;
}

// ============================================================================

bool ResolvedPi0Maker::selPi0(const LHCb::CaloParticle& g1,
                              const LHCb::CaloParticle& g2){
  //
  bool isGood=false;

  LHCb::CaloParticle pi0(g1.particle() ,m_point ,m_pointErr);
  pi0.addCaloPosition(g2.particle() );
  double bal = fabs( (g1.particle()->momentum().e() - g2.particle()->momentum().e())
                     / (g1.particle()->momentum().e() + g2.particle()->momentum().e() ) );
  if (fabs(pi0.mass()-m_Mass) < m_MassWin
      && pi0.pt() > m_PtCut
      && ( m_minbal <0 || bal > m_minbal )
      && ( m_maxbal <0 || bal < m_maxbal ) ) isGood=true;
  return isGood;
}



