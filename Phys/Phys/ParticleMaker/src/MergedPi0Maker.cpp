// $Id: MergedPi0Maker.cpp,v 1.12 2010-09-01 14:54:00 odescham Exp $
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "Kernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloDet/DeCalorimeter.h"
// local
#include "MergedPi0Maker.h"

/** @file
 *
 *  Implementation file for class : MergedPi0Maker
 *
 *  @date 2006-08-25
 *  @author Olivier Deschamps odescham@in2p3.fr
 *
 */

// ============================================================================
/** @var MergedPi0MakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( MergedPi0Maker )

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name
 *  @param parent tool parent
 */
// ============================================================================
MergedPi0Maker::MergedPi0Maker( const std::string& name,ISvcLocator* pSvcLocator )
: Pi0MakerBase ( name , pSvcLocator ),
  m_calo       ( NULL )
{
  declareProperty ( "Input" , m_input = LHCb::ProtoParticleLocation::Neutrals ) ;
  //
  declareProperty ( "UseCaloTrMatch"             , m_useCaloTrMatch   = false ) ;
  declareProperty ( "UseClusterMass"             , m_useClusterMass   = false ) ;
  declareProperty ( "UseIsNotH"                  , m_useIsNotH        = false ) ;
  declareProperty ( "UseIsNotE"                  , m_useIsNotE        = false ) ;
  declareProperty ( "UsePhotonDLL"               , m_usePhotonDLL     = false ) ;
  declareProperty ( "UseIsPhoton"                , m_useIsPhoton      = true  ) ;
  // Filter
  declareProperty( "ConfLevelCut"      , m_clCut = -1     ) ; 
  declareProperty( "GammaPtCut"        , m_gPtCut = 0. * Gaudi::Units::MeV );
  declareProperty( "GammaGammaDistCut" , m_ggDistCut = 1.8 ); // Unit = cellSize
  declareProperty( "Chi2Cut"           , m_chi2Cut   = 1.  );
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
MergedPi0Maker::~MergedPi0Maker() {}
// ============================================================================

StatusCode MergedPi0Maker::initialize    ()
{
  // initialize the base class
  const StatusCode sc = Pi0MakerBase::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize Pi0MakerBase",sc);}

  // DeCalorimeter
  m_calo =  getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;

  // check vectors of paramters
  // CL techniques
  if ( msgLevel(MSG::DEBUG)){
    debug() << " Following techniques will be used for CL evaluation : " << endmsg;
    if( m_useCaloTrMatch    )debug() << "CL uses CaloTrMatch  : matching with reconstructed tracks " << endmsg ; 
    if( m_useClusterMass    )debug() << "CL uses ClusterMass  : Ecal Cluster  mass              " << endmsg ; 
    if( m_useIsNotH         )debug() << "CL uses IsNotH       : Tracking/CALO NN-based combined  photon id   " << endmsg ; 
    if( m_useIsNotE         )debug() << "CL uses IsNotE       : Tracking/CALO NN-based combined  photon id   " << endmsg ; 
    if( m_usePhotonDLL      )debug() << "CL uses PhotonDLL    : Tracking/CALO DLL-based combined photon id   " << endmsg ;
    if( m_useIsPhoton       )debug() << "CL uses IsPhoton separation variable" << endmsg;
  }

  if( !m_useCaloTrMatch   &&
      !m_useClusterMass   &&
      !m_useIsPhoton      &&
      !m_useIsNotH      &&
      !m_usePhotonDLL      ){ Warning(" No PID techniques are selected for CL evaluation" ).ignore() ; }

  if ( msgLevel(MSG::DEBUG)){
    if( m_useCaloTrMatch    )debug()<< "  For CaloTrMatch assume Gauss distribution (wrong?)" << endmsg;
    if( m_useClusterMass    )debug()<< "  For ClusterMass assume exponential distribution (wrong?)"<< endmsg; 
  }
  return sc ;
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode MergedPi0Maker::makeParticles (LHCb::Particle::Vector & particles ){
  // avoid some long names

  if( !particles.empty() )
  {
    Warning( "makeParticles(): extend non-empty vector of Particles" ).ignore() ;
  }

  // locate input data
  const LHCb::ProtoParticle::ConstVector& pps = protos() ;

  unsigned long nPp    = 0 ;
  unsigned long nSelPp = 0 ;
  unsigned long nSkip  = 0 ;
  m_count[0] += 1;

  // Loop over PP
  for( LHCb::ProtoParticle::ConstVector::const_iterator ipp = pps.begin() ;
       pps.end() != ipp ; ++ipp ){

    const LHCb::ProtoParticle* pp = *ipp ;

    // ---- skip invalid and charged
    if ( NULL == pp || NULL != pp->track() )   { continue ; }

    // ---- Check the hypothesis
    const LHCb::CaloHypo*   hypo  = *( (pp->calo()).begin() );
    if(LHCb::CaloHypo::Pi0Merged != hypo->hypothesis() )continue;

    ++nPp;

    // Filters
    LHCb::CaloMomentum pi0Momentum( pp , m_point , m_pointErr);

    // ---- apply mass window
    double mass = pi0Momentum.mass();
    if (m_MassWin < fabs(mass-m_Mass) ){continue;}

    // ---- apply Pt(pi0) cut
    if ( m_PtCut  > pi0Momentum.pt()     ){continue;}

    // ---- apply chi2(Tr,cluster) cut 
    const double chi2 = pp->info(LHCb::ProtoParticle::CaloTrMatch,+1.e+06);
    if ( m_chi2Cut >= 0 && chi2          < m_chi2Cut                ){continue;}

    // ---- apply CL cut 
    const double CL = confLevel( pp );
    if ( m_clCut >= 0 && CL          < m_clCut                ){continue;}

    // == extract SplitPhotons hypos
    const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
    const LHCb::CaloHypo* g1 = *(hypos.begin() );
    const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
    LHCb::CaloMomentum g1Momentum( g1 , m_point , m_pointErr );
    LHCb::CaloMomentum g2Momentum( g2 , m_point , m_pointErr );


    // ---- Apply SplitPhoton pT cut
    if ( m_gPtCut    > g1Momentum.pt()      ){continue;}
    if ( m_gPtCut    > g2Momentum.pt()      ){continue;}


    // Gamma-Gamma Min distance
    // retrieve cellID by position
    // (WARNING USE g1 split photon 'position')
    const LHCb::CaloPosition* hypoPos = g1->position();
    if ( NULL == hypoPos){
      Warning("CaloPosition point to null").ignore();
      return StatusCode::FAILURE ;
    }
    const Gaudi::XYZPoint point( hypoPos->x(), hypoPos->y(), hypoPos->z() );

    LHCb::CaloCellID     cellID = m_calo->Cell( point );
    double CellSize =  m_calo->cellSize( cellID  );
    double zpos     =  m_calo->cellZ   ( cellID ) ;
    double epi0     =  pi0Momentum.e();
    double dmin     =  ( epi0 * CellSize > 0 ) ? zpos * 2. * m_Mass / epi0 / CellSize : +9999.; // rare FPE ( hypo outside Calo acceptance ?)
    if ( m_ggDistCut < dmin                 ){continue;}

    ++nSelPp;

    // === create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( );
    particle -> setParticleID( LHCb::ParticleID(m_Id) );
    particle -> setProto( pp ) ;

    // --- set confidence level
    particle  -> setConfLevel  ( CL    ) ;

    // --- set MergedPi0 parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();
    if( calopart.status() != 0 ){
      delete particle ;
      ++nSkip;
      debug() << "CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      debug() << "Unable to fill Merged " << m_pid << " parameters, skip particle [" << nSkip << "]"<< endmsg;
      --nSelPp;
      continue ;
    }

    //-- set mass and mass uncertainties
    particle -> setMeasuredMass( mass ) ; 
    particle -> setMeasuredMassErr( calopart.emass() ) ;

    //-- counter & debug
    m_count[1] += 1;
    if (msgLevel(MSG::VERBOSE)){
      verbose() << " ---- Merged " << m_pid << " found ["<< nSelPp << "]" <<  endmsg;
      verbose() << "Pt    "  << pi0Momentum.pt() << endmsg;
      verbose() << "CL / Chi2  "  << chi2   << " / "<<pp->info(LHCb::ProtoParticle::CaloTrMatch,+1.e+06) << ")"<<endmsg;
      verbose() << "dist(gg)"<< dmin << endmsg;
    }

    // --- add the particle to the container
    particles.push_back( particle );

  }

  counter("Created merged " + m_pid) += nSelPp;
  if (msgLevel(MSG::DEBUG)){
    debug() << " " << endmsg;
    debug() << "-----------------------" << endmsg;
    debug() << " Filtered and created :" << endmsg;
    debug() << " --> " << nSelPp << " Merged " << m_pid << " (among " << nPp <<")"<< endmsg;
    debug() << " Skipped : " << nSkip << endmsg;
    debug() << "--------------------" << endmsg;
  }

  return StatusCode::SUCCESS ;
}

// ============================================================================
double MergedPi0Maker::confLevel( const LHCb::ProtoParticle* pp ) const
{
  if( 0 == pp )
  { Error("confLevel(): ProtoParticle* points to NULL!"); return -1 ; };

  double CL = 1.0 ;

  // track matching
  if( m_useCaloTrMatch  ){
    if( pp->hasInfo(LHCb::ProtoParticle::CaloTrMatch) ){
      // assume gaussian distribution (it is wrong!)
      CL *= ( 1.0 - std::exp( -0.5 * pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) )) ;
    }
    else{
      CL *= 1.;
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "confLevel(): CaloTrMatch is not available - assume above threshold" << endmsg ; 
    }
  }

  // Cluster Mass
  if( m_useClusterMass ){
    if( pp->hasInfo(LHCb::ProtoParticle::ClusterMass)){
      // assume exponential distribution (it is wrong!)
      CL *= ( std::exp( -0.5 * pp->info(LHCb::ProtoParticle::ClusterMass,0.)
                        / ( 25 * Gaudi::Units::MeV ) ) );
    }  // Update CL
    else{
      Warning("confLevel(): ClusterMass is not available" ).ignore() ; 
      counter("unavailable ClusterMass") += 1;
    }
    
  }

  // NN-based PhotonID IsNotH
  if( m_useIsNotH ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotH )){
      double v= pp->info(LHCb::ProtoParticle::IsNotH , 0. );
      if( v > 1.) v=1.;
      if( v < 0.) v=0.;
      CL *= v;
    }else{
      Warning("confLevel(): NN-based PhotonID IsNotH is not available" ) ;
      counter("unavailable IsNotE") += 1;
    }    
  }

  // NN-based PhotonID IsNotE
  if( m_useIsNotE ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsNotE )){
      double v= pp->info(LHCb::ProtoParticle::IsNotE , 0. );
      if( v > 1.) v=1.;
      if( v < 0.) v=0.;
      CL *= v;
    }else{
      Warning("confLevel(): NN-based PhotonID IsNotE is not available" ) ;
      counter("unavailable IsNotE") += 1;
    }    
  }

  // IsPhoton
  if( m_useIsPhoton ){
    if( pp->hasInfo(LHCb::ProtoParticle::IsPhoton )){
      double v =  pp->info(LHCb::ProtoParticle::IsPhoton,+1.);
      if( v > 1. ) v = 1.;
      if( v < 0. ) v = 0.;
      CL *= (1-v);
    }else{
      Warning("confLevel(): NN-based IsPhoton is not available" ) ;
      counter("unavailable IsPhoton") += 1;
    }    
  }


    //(obsolete) DLL-based photonID
  if( m_usePhotonDLL ){
    if ( pp->hasInfo(LHCb::ProtoParticle::PhotonID) ){
      double pid = pp->info(LHCb::ProtoParticle::PhotonID, -1.  ) ;
      CL *= 0.5*(std::tanh(pid)+1);
  }
    else
      Warning("confLevel(): DLL-based PhotonID is not available" ) ;
      counter("unavailable PhotonDLL") += 1;
  }


  return CL ;
}


