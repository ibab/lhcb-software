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
  declareProperty ( "UseCaloTrMatch"             , m_useCaloTrMatch   = true  ) ;
  declareProperty ( "UseCaloDepositID"           , m_useCaloDepositID = false ) ;
  declareProperty ( "UseShowerShape"             , m_useShowerShape   = false ) ;
  declareProperty ( "UseClusterMass"             , m_useClusterMass   = false ) ;
  // Filter
  declareProperty ( "ConfLevelCut"     , m_clCut = 0.4 ) ; // Chi2  > 1
  declareProperty( "GammaPtCut"        , m_gPtCut = 0. * Gaudi::Units::MeV );
  declareProperty( "GammaGammaDistCut" , m_ggDistCut = 1.8 ); // Unit = cellSize
  // Mass correction
  m_parMas.push_back( 1.00 );
  m_parMas.push_back( 0.00 );
  m_parMas.push_back( 0.00 );
  declareProperty( "ParMas", m_parMas );
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
  if( 3 != m_parMas.size() )
  {
    return Error( "Invalid number of parameters" );
  }
  // CL techniques
  if ( msgLevel(MSG::DEBUG))
  {
    debug() << " Following techniques will be used for CL evaluation : " << endmsg;
    if( m_useCaloTrMatch    )
    { debug() << "  CaloTrMatch : matching with reconstructed tracks " << endmsg ; }
    if( m_useCaloDepositID  )
    { debug() << " CaloDepositID: Spd/Prs combined analysis          " << endmsg ; }
    if( m_useShowerShape  )
    { debug() << " ShowerShape  : Ecal Cluster shape/size            " << endmsg ; }
    if( m_useClusterMass  )
    { debug() << " ClusterMass  : Ecal Cluster  mass              " << endmsg ; }
  }

  if( !m_useCaloTrMatch   &&
      !m_useCaloDepositID &&
      !m_useShowerShape   &&
      !m_useClusterMass   )
  { Warning(" No PID techniques are selected for CL evaluation" ).ignore() ; }

  if ( msgLevel(MSG::DEBUG))
  {
    if( m_useCaloTrMatch    ){debug()<< "  For CaloTrMatch assume Gauss distribution (wrong?)" << endmsg;}
    if( m_useCaloDepositID  ){debug()<< "      CaloDepositID is not implemented yet " << endmsg ; }
    if( m_useShowerShape    ){debug()<< "      ShowerShape   is not implemented yet " << endmsg ; }
    if( m_useClusterMass    ){debug()<< "  For ClusterMass assume exponential distribution (wrong?)"<< endmsg; }
  }

  return sc ;
}

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode MergedPi0Maker::makeParticles (LHCb::Particle::Vector & particles )
{
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
       pps.end() != ipp ; ++ipp )
  {

    const LHCb::ProtoParticle* pp = *ipp ;

    // skip invalid and charged
    if ( NULL == pp || NULL != pp->track() )   { continue ; }

    // Check the hypothesis
    const LHCb::CaloHypo*   hypo  = *( (pp->calo()).begin() );
    if(LHCb::CaloHypo::Pi0Merged != hypo->hypothesis() )continue;

    ++nPp;

    // evaluate the Confidence Level
    const double CL = confLevel( pp );
    if ( CL          < m_clCut                ){continue;}

    // Filters
    LHCb::CaloMomentum pi0Momentum( pp , m_point , m_pointErr);

    // Mass correction (Warning : mass correction not propagated to the pi0 4-momentum)
    double umas = pi0Momentum.mass();
    double ene  = pi0Momentum.e()/Gaudi::Units::GeV;
    double fact = m_parMas[0]+m_parMas[1]*ene+m_parMas[2]*ene*ene;
    double mass = umas/fact;
    if (m_MassWin < fabs(mass-m_Mass) ){continue;}

    // Pt(pi0)
    if ( m_PtCut  > pi0Momentum.pt()     ){continue;}

    // Split Photons
    const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
    const LHCb::CaloHypo* g1 = *(hypos.begin() );
    const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
    LHCb::CaloMomentum g1Momentum( g1 , m_point , m_pointErr );
    LHCb::CaloMomentum g2Momentum( g2 , m_point , m_pointErr );
    if ( m_gPtCut    > g1Momentum.pt()      ){continue;}
    if ( m_gPtCut    > g2Momentum.pt()      ){continue;}

    // Gamma-Gamma Min distance
    // retrieve cellID by position
    // (WARNING USE g1 split photon 'position')
    const LHCb::CaloPosition* hypoPos = g1->position();
    if ( NULL == hypoPos)
    {
      Warning("CaloPosition point to null").ignore();
      return StatusCode::FAILURE ;
    }
    const Gaudi::XYZPoint point( hypoPos->x(), hypoPos->y(), hypoPos->z() );

    LHCb::CaloCellID     cellID = m_calo->Cell( point );
    double CellSize =  m_calo->cellSize( cellID  );
    double zpos     =  m_calo->cellZ   ( cellID ) ;
    double epi0     =  pi0Momentum.e();
    double dmin     =  zpos * 2. * m_Mass / epi0 / CellSize;
    if ( m_ggDistCut < dmin                 ){continue;}

    ++nSelPp;

    // create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( );
    particle -> setParticleID( LHCb::ParticleID(m_Id) );
    particle -> setProto( pp ) ;

    // confidence level
    particle  -> setConfLevel  ( CL    ) ;

    // fill Merged parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();
    if( calopart.status() != 0 )
    {
      delete particle ;
      ++nSkip;
      debug() << "CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      debug() << "Unable to fill Merged " << m_pid << " parameters, skip particle [" << nSkip << "]"<< endmsg;
      --nSelPp;
      continue ;
    }

    // mass and mass uncertainties
    particle -> setMeasuredMass( mass ) ; // corrected mass
    particle -> setMeasuredMassErr( calopart.emass() ) ;


    m_count[1] += 1;
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << " ---- Merged " << m_pid << " found ["<< nSelPp << "]" <<  endmsg;
      verbose() << "Pt    "  << pi0Momentum.pt() << endmsg;
      verbose() << "Corrected Mass (uncorrected) " << mass << " (" << umas << ")"<<endmsg;
      verbose() << "CL (Chi2)  " << CL   << " ("<<pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) << ")"<<endmsg;
      verbose() << "dist(gg)"<< dmin << endmsg;
    }

    // add the particle to the container
    particles.push_back( particle );

  }

  counter("Created merged " + m_pid) += nSelPp;
  if (msgLevel(MSG::DEBUG))
  {
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
  if( m_useCaloTrMatch  )
  {
    if( pp->hasInfo(LHCb::ProtoParticle::CaloTrMatch) )
    {
      // assume gaussian distribution (it is wrong!)
      CL *= ( 1.0 - std::exp( -0.5 * pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) )) ;
    }
    else
    {
      Warning("confLevel(): CaloTrMatch is not available" ).ignore() ; }
  }

  // CaloDepositID
  if( m_useCaloDepositID )
  {
    if( pp->hasInfo(LHCb::ProtoParticle::CaloDepositID) )
    {
      Warning("confLevel(): usage of CaloDepositID is not implemented").ignore();
    }   // Update CL
    else
    {
      Warning("confLevel(): CaloDepositID is not available" ).ignore() ; }
  }

  // ShowerShape
  if( m_useShowerShape )
  {
    if( pp->hasInfo(LHCb::ProtoParticle::ShowerShape ) )
    {
      Warning("confLevel(): usage of CaloShowerShape is not implemented").ignore();
    }   // Update CL
    else
    {
      Warning("confLevel(): CaloShowerShape is not available" ).ignore() ;
    }
  }


  // Cluster Mass
  if( m_useClusterMass )
  {
    if( pp->hasInfo(LHCb::ProtoParticle::ClusterMass))
    {
      // assume exponential distribution (it is wrong!)
      CL *= ( std::exp( -0.5 * pp->info(LHCb::ProtoParticle::ClusterMass,0.)
                        / ( 25 * Gaudi::Units::MeV ) ) );
    }  // Update CL
    else
    {
      Warning("confLevel(): ClusterMass is not available" ).ignore() ; }
  }

  return CL ;
}
