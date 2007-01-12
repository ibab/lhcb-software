// $Id: 
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "CaloUtils/CaloParticle.h" 
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDataFunctor.h"
//#include "CaloUtils/CaloHypoFilter.h"
// local
#include "PhotonMaker.h"

/** @file 
 *  
 *  Implementation file for class : PhotonMaker
 * 
 *  @date 2003-01-19 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *
 *  @ modified 2006-08-25 for DC06
 *  @ Olivier Deschamps odescham@in2p3.fr
 */



// ============================================================================
/** @var PhotonMakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================

DECLARE_TOOL_FACTORY(PhotonMaker);

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
PhotonMaker::PhotonMaker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool           ( type, name , parent ) 
    , m_input            ()
  // Origin vertex
    , m_point            () 
    , m_pointErr         ()
  // PID technque
    , m_useCaloTrMatch   ()
    , m_useCaloDepositID ()
    , m_useShowerShape   ()
    , m_useClusterMass   ()
    , m_usePhotonID      ()
  // Filters
    , m_clCut            () 
    , m_ptCut            () 
    , m_converted        ()
    , m_unconverted      ()
{
  declareProperty ( "Input"                      , m_input = LHCb::ProtoParticleLocation::Neutrals ) ;
  //
  declareProperty ( "UseCaloTrMatch"             , m_useCaloTrMatch   = false) ;
  declareProperty ( "UseCaloDepositID"           , m_useCaloDepositID = false) ;
  declareProperty ( "UseShowerShape"             , m_useShowerShape   = false) ;
  declareProperty ( "UseClusterMass"             , m_useClusterMass   = false) ;
  declareProperty ( "UsePhotonID"                , m_usePhotonID      = true ) ;
  //
  declareProperty ( "ConfLevelCut"               , m_clCut       = -1.0   ) ;
  declareProperty ( "PtCut"                      , m_ptCut       = 150.0  ) ;
  declareProperty ( "ConvertedPhotons"           , m_converted   = true   ) ;
  declareProperty ( "UnconvertedPhotons"         , m_unconverted = true   ) ;

  // declare new interface
  declareInterface<ICaloParticleMaker> (this);
};
// ============================================================================


// ============================================================================
/// destructor
// ============================================================================
PhotonMaker::~PhotonMaker() {};
// ============================================================================

 StatusCode PhotonMaker::initialize    ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize GaudiTool",sc);}

  // ParticleProperty
  IParticlePropertySvc* ppSvc = 0;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"	  << endreq;
    return sc;
  }
  ParticleProperty* partProp;
  partProp = ppSvc->find( "gamma" );
  m_Id = (*partProp).jetsetID();

  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;
  

  // CL techniques
  debug() << " Following techniques will be used for CL evaluation : " << endreq;
  if( m_useCaloTrMatch    )
  { debug() << "  CaloTrMatch : matching with reconstructed tracks " << endreq ; }
  if( m_useCaloDepositID  )
  { debug() << " CaloDepositID: Spd/Prs combined analysis          " << endreq ; }
  if( m_useShowerShape  )
  { debug() << " ShowerShape  : Ecal Cluster shape/size            " << endreq ; }
  if( m_useClusterMass  )
  { debug() << " ClusterMass  : Ecal Cluster pi0 mass              " << endreq ; }
  if( m_usePhotonID     )
  { debug() << " PhotonID     : Tracking/CALO combined Photon id   " << endreq ; }

  if( !m_useCaloTrMatch   &&
      !m_useCaloDepositID &&
      !m_useShowerShape   &&
      !m_useClusterMass   &&
      !m_usePhotonID     )
    { Warning(" No PID techniques are selected for CL evaluation" ) ; }

  if( m_useCaloTrMatch    )
    { Warning( "  For CaloTrMatch assume Gauss distribution (wrong?)"); }
  if( m_useCaloDepositID  )
    { Warning( "      CaloDepositID is not implemented yet " ) ; }
  if( m_useShowerShape    )
    { Warning( "      ShowerShape   is not implemented yet " ) ; }
  if( m_useClusterMass    )
    { Warning( "  For ClusterMass assume exponential distribution (wrong?)"); }

  if( m_converted && m_unconverted)
  {debug()<< "BOTH converted (SPD) and unconverted (no SPD) photons are to be created"<< endreq;  }
  else if ( m_converted )
  {debug()<< "ONLY converted (SPD) photons are to be created"<< endreq;}
  else if ( m_unconverted )
  {debug()<< "ONLY unconverted (no SPD) photons are to be created"<< endreq;}
  else 
  {warning()<<" BOTH converted and unconverted photon are to be skipped !!!" << endreq; }

  return StatusCode::SUCCESS ;
};
// ============================================================================

StatusCode PhotonMaker::finalize      ()
{
  info() << " - PhotonMaker Summary -----" << endreq;
  info() << " Created : " << (float) ( m_count[1]+m_count[2])/m_count[0] << " photons per event" << endreq;
  info() << " " <<  (float) m_count[2]/(m_count[1] + m_count[2])*100.0 << "% are converted (SPD based)"<< endreq;
  info() << " ---------------------------" << endreq;
  // finalize the base class
  return GaudiTool::finalize ();
};

// ============================================================================
/** Make the particles
 *  @see ICaloParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode PhotonMaker::makeParticles (LHCb::Particle::Vector & particles )
{
  // avoid some long names

  if( !particles.empty() ){
    Warning( "makeParticles(): extend non-empty vector of Particles" ) ; 
  }
  

  // locate input data
  const LHCb::ProtoParticles* pps = get< LHCb::ProtoParticles > (m_input);
  if( 0 == pps ) { return StatusCode::FAILURE ; }

  
  unsigned long nConverted = 0 ;
  unsigned long nUnconverted = 0 ;
  unsigned long nSelConverted = 0 ;
  unsigned long nSelUnconverted = 0 ;
  unsigned long nSkip =0;
  m_count[0] += 1;
  
  // Loop over PP
  for( LHCb::ProtoParticles::const_iterator ipp = pps->begin() ;
       pps->end() != ipp ; ++ipp ){

    LHCb::ProtoParticle* pp = *ipp ;

    // skip invalid and charged
    if ( 0 == pp || 0 != pp->track() )   { continue ; }        

    
    // Check the hypothesis 
    const LHCb::CaloHypo*   hypo  = *( (pp->calo()).begin() );
    if(LHCb::CaloHypo::Photon != hypo->hypothesis() )continue;    

    // Photon conversion (Spd-based for late conversion after magnet)
    bool cnv = (bool) pp->info(LHCb::ProtoParticle::CaloNeutralSpd, 0.) ;
    debug() << " Conversion " << cnv << " " << pp->info(LHCb::ProtoParticle::CaloNeutralSpd,0.) << endreq;
    
    // selected (un)converted photons
    if( !cnv && !m_unconverted  ) { continue ; }
    if( cnv  && !m_converted    ) { continue ; }
    
    cnv ? ++nConverted : ++nUnconverted;
    
    // evaluate the Confidence Level
    const double CL = confLevel( pp );
    if ( CL  < m_clCut                      ) continue ;
  
    // Pt cut
    LHCb::CaloMomentum momentum(pp , m_point , m_pointErr);
    if(momentum.pt() < m_ptCut)continue;
    

    cnv ? ++nSelConverted : ++nSelUnconverted;

    
    // create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( ); // photon constructor
    particle -> setParticleID( m_Id );
    particle -> setProto( pp ) ;

    // mass and mass uncertainties
    particle -> setMeasuredMass( 0   ) ; // photon is mass-less
    particle -> setMeasuredMassErr( 0   ) ; // the mass is EXACT zero!
    
    // confidence level
    particle  -> setConfLevel  ( CL    ) ;
    
    // fill photon parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();
    if( calopart.status() !=0 ){
      delete particle ;
      nSkip++;
      warning() << "Unable to fill photon parameters, skip particle [" << nSkip << "]"<< endreq;
      warning() << " CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      cnv ? --nSelConverted : --nSelUnconverted;
      continue ;                                                
    }
    
    //
    verbose() << "----- Single photon found [" << nSelConverted+nSelUnconverted << "]"<<endreq;
    verbose() << "Pt : " << momentum.pt() << endreq;
    verbose() << "CL : " << CL << endreq;
    verbose() << "Chi2 " << pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) << endreq;
    verbose() << "Converted (SPD digits)? : " << cnv << endreq;      
    verbose() << "CaloDeposit : " <<pp->info(LHCb::ProtoParticle::CaloDepositID,-999.) <<endreq;
    verbose() << " "<< endreq;
    
    // add the particle to the container
    particles.push_back( particle );
  }
  m_count[1] += nSelUnconverted ;
  m_count[2] += nSelConverted ;
  
  debug() << " " << endreq;
  debug() << "-----------------------" << endreq;
  debug() << " Filtered and created :" << endreq;
  debug() << " --> " << nSelConverted + nSelUnconverted << " photons (among " 
          << nConverted + nUnconverted <<")"<< endreq;
  debug() << "   -> " << nSelConverted 
          << " Converted Photons (among " << nConverted <<")"<< endreq;
  debug() << "   -> " << nSelUnconverted 
          << " Unconverted Photons (among " << nUnconverted <<")"<< endreq;
  debug() << " Skipped : " << nSkip << endreq;
  debug() << "-----------------------" << endreq;
  return StatusCode::SUCCESS ;
};

// ============================================================================
double PhotonMaker::confLevel( const LHCb::ProtoParticle* pp ) const
{
  // avoid long typing
  typedef const std::vector<std::pair<int,double> >  PIDs  ;

  if( 0 == pp )
    { Error("confLevel(): ProtoParticle* points to NULL!"); return -1 ; };

  double CL = 1.0 ;

  // track matching
  if( m_useCaloTrMatch  ){
    if( pp->hasInfo(LHCb::ProtoParticle::CaloTrMatch) ){
      // assume gaussian distribution (it is wrong!)
      CL *= ( 1.0 - exp( -0.5 * pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) )) ; 
    }
    else{ 
      Warning("confLevel(): CaloTrMatch is not available" ) ; }
  }

  // CaloDepositID
  if( m_useCaloDepositID ){
    if( pp->hasInfo(LHCb::ProtoParticle::CaloDepositID) ){
      Warning("confLevel(): usage of CaloDepositID is not implemented");
    }   // Update CL
    else{ 
      Warning("confLevel(): CaloDepositID is not available" ) ; }
  }
  
  // ShowerShape
  if( m_useShowerShape ){
    if( pp->hasInfo(LHCb::ProtoParticle::ShowerShape ) ){
      Warning("confLevel(): usage of CaloShowerShape is not implemented");
    }   // Update CL
    else{
      Warning("confLevel(): CaloShowerShape is not available" ) ;
    }
  }
  

  // Cluster Mass
  if( m_useClusterMass ){
    if( pp->hasInfo(LHCb::ProtoParticle::ClusterMass)){
      // assume exponential distribution (it is wrong!)
      CL *= exp( -0.5 * pp->info(LHCb::ProtoParticle::ClusterMass,0.)
                 / ( 25 * Gaudi::Units::MeV ) ) ;
    }  // Update CL
    else{ 
      Warning("confLevel(): ClusterMass is not available" ) ; }
  }

  // PhotonID estimator
  if( m_usePhotonID ){
    if( pp->hasInfo(LHCb::ProtoParticle::PhotonID  )){
      CL *= pp->info(LHCb::ProtoParticle::PhotonID, 1.  ) ;
    }
    else{ 
      Warning("confLevel(): PhotonID is not available" ) ; }
  }

  return CL ;
};
