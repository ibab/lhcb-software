// $Id: 
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
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

DECLARE_TOOL_FACTORY(PhotonMaker)

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
  ,m_mas               (0.){
  declareProperty ( "Input"                      , m_input = LHCb::ProtoParticleLocation::Neutrals ) ;
  declareProperty ( "Particle"                   , m_part="gamma");
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
  declareProperty ( "PrsThreshold"               , m_minPrs = -1.   ) ;
  //
  declareProperty ( "AddHcalEnergy"              , m_addHcal = false);
  declareProperty ( "MaxHcalRatio"               , m_maxHcal = -1.);
  declareProperty ( "MinHcalRatio"               , m_minHcal = -1.);  
  declareProperty ( "MaxPrsEnergy"               , m_maxPrs  = -1.);
  // declare new interface
  declareInterface<ICaloParticleMaker> (this);
}
// ============================================================================


// ============================================================================
/// destructor
// ============================================================================
PhotonMaker::~PhotonMaker() {}
// ============================================================================

 StatusCode PhotonMaker::initialize    ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize GaudiTool",sc);}

  // ParticleProperty
  LHCb::IParticlePropertySvc* ppSvc = 0;
  sc = service("LHCb::ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    fatal() << "    Unable to locate Particle Property Service"	  << endmsg;
    return sc;
  }
  const LHCb::ParticleProperty* partProp;
  partProp = ppSvc->find( m_part );
  m_Id = (*partProp).pdgID().pid();
  m_mas = partProp->mass();
  //
  m_count[0]=0;
  m_count[1]=0;
  m_count[2]=0;
  


  // CL techniques
  debug() << " Following techniques will be used for CL evaluation : " << endmsg;
  if( m_useCaloTrMatch    )
  { debug() << "  CaloTrMatch : matching with reconstructed tracks " << endmsg ; }
  if( m_useCaloDepositID  )
  { debug() << " CaloDepositID: Spd/Prs combined analysis          " << endmsg ; }
  if( m_useShowerShape  )
  { debug() << " ShowerShape  : Ecal Cluster shape/size            " << endmsg ; }
  if( m_useClusterMass  )
  { debug() << " ClusterMass  : Ecal Cluster pi0 mass              " << endmsg ; }
  if( m_usePhotonID     )
  { debug() << " PhotonID     : Tracking/CALO combined Photon id   " << endmsg ; }

  if( !m_useCaloTrMatch   &&
      !m_useCaloDepositID &&
      !m_useShowerShape   &&
      !m_useClusterMass   &&
      !m_usePhotonID     )
    { Warning(" No PID techniques are selected for CL evaluation" ) ; }

  if( m_useCaloTrMatch    ){ debug()<< "  For CaloTrMatch assume Gauss distribution (wrong?)"<< endmsg; }
  if( m_useCaloDepositID  ){ debug()<< "      CaloDepositID is not implemented yet " <<endmsg ; }
  if( m_useShowerShape    ){ debug()<< "      ShowerShape   is not implemented yet " << endmsg ; }
  if( m_useClusterMass    ){ debug()<< "  For ClusterMass assume exponential distribution (wrong?)"<<endmsg; }
  
  if( m_converted && m_unconverted)
  {debug()<< "BOTH converted (SPD) and unconverted (no SPD) photons are to be created"<< endmsg;  }
  else if ( m_converted )
  {debug()<< "ONLY converted (SPD) photons are to be created"<< endmsg;}
  else if ( m_unconverted )
  {debug()<< "ONLY unconverted (no SPD) photons are to be created"<< endmsg;}
  else 
  {warning()<<" BOTH converted and unconverted photon are to be skipped !!!" << endmsg; }

  return StatusCode::SUCCESS ;
}
// ============================================================================

StatusCode PhotonMaker::finalize      ()
{  
  double f = (m_count[0] == 0 ) ? 0. :  (float) ( m_count[1]+m_count[2])/(float)m_count[0];
  double fc =  (m_count[1]+m_count[2] ==0)? 0 : (float) m_count[2]/(float)(m_count[1] + m_count[2])*100.0;
  
  info() << name() << " -  Summary -----" << endmsg;
  info() << " Created : " << f << " " << m_part << " per event" << endmsg;
  info() << " " << fc  << "% have SPD hits"<< endmsg;
  info() << " ---------------------------" << endmsg;
  // finalize the base class
  return GaudiTool::finalize ();
}

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
  const LHCb::ProtoParticles* pps = NULL;
  if ( exist<LHCb::ProtoParticles>( m_input ))
    pps = get< LHCb::ProtoParticles > (m_input);
  else 
    return Warning("No ProtoParticles at " + m_input + " 0 photons created",StatusCode::SUCCESS);  
  if( NULL == pps)return Warning("ProtoParticles container " + m_input + " points to NULL",StatusCode::SUCCESS);

  unsigned long nConverted = 0 ;
  unsigned long nUnconverted = 0 ;
  unsigned long nSelConverted = 0 ;
  unsigned long nSelUnconverted = 0 ;
  unsigned long nSkip =0;
  m_count[0] += 1;
  
  // Loop over PP
  for( LHCb::ProtoParticles::const_iterator ipp = pps->begin() ;
       pps->end() != ipp ; ++ipp ){
    
    const LHCb::ProtoParticle* pp = *ipp ;
    
    // skip invalid and charged
    if ( 0 == pp || 0 != pp->track() )   { continue ; }
    
    const SmartRefVector<LHCb::CaloHypo>& hypos  = pp->calo() ;
    if ( hypos.empty() ) { continue ; }
    
    // Check the hypothesis 
    const LHCb::CaloHypo*   hypo  = *(hypos.begin());
    
    if ( 0 == hypo || LHCb::CaloHypo::Photon != hypo->hypothesis() ) { continue ; }

    // skip negative energy
    if( hypo->e() <= 0 ) continue;
    
    // Photon conversion (Spd-based for late conversion after magnet)
    bool cnv = ( pp->info(LHCb::ProtoParticle::CaloDepositID, 0.)<0) ;  // temporary re-definition of conversion
    //    bool cnv = (bool) pp->info(LHCb::ProtoParticle::CaloNeutralSpd, 0.) ; // failed due to bug in NeutralProtoPAlg
    debug() << " Conversion " << cnv << " " << pp->info(LHCb::ProtoParticle::CaloDepositID,0.) << endmsg;
    
    // selected (un)converted photons
    if( !cnv && !m_unconverted  ) { continue ; }
    if( cnv  && !m_converted    ) { continue ; }
    double ePrs =  pp->info(LHCb::ProtoParticle::CaloNeutralPrs,0);
    if( m_minPrs >= 0 && ePrs < m_minPrs )continue;
    if( m_maxPrs >= 0 && ePrs > m_maxPrs )continue;
    cnv ? ++nConverted : ++nUnconverted;
    
    // evaluate the Confidence Level
    const double CL = confLevel( pp );
    if ( CL  < m_clCut                      ) continue ;


    // Hcal energy
    double eHcal=pp->info(LHCb::ProtoParticle::CaloNeutralHcal2Ecal,0);
    if( m_maxHcal >=0 && eHcal/(1.+eHcal) > m_maxHcal)continue;
    if( m_minHcal >=0 && eHcal/(1.+eHcal) < m_minHcal)continue;

    
    //Kinematical properties 
    LHCb::CaloMomentum momentum(pp , m_point , m_pointErr);
    if ( 0 != momentum.status()  ) { continue ; }


    double pT = momentum.pt();
    double E = momentum.e();
    double px= momentum.px();
    double py= momentum.py();
    double pz= momentum.pz();
    double p = E;
    // Massive object or Hcal correction
    if( m_addHcal || m_mas > 0 ){
      if( m_addHcal ){
        E *= ( 1. + eHcal );
        px *= (1. + eHcal);
        py *= (1. + eHcal);
        pz *= (1. + eHcal);
        pT *= (1. + eHcal);
        p  = E;
      }
      if( m_mas > 0 ){
        p  = sqrt( E*E - m_mas*m_mas);   
        px *= (p/E);
        py *= (p/E);
        pz *= (p/E);
        pT *= (p/E);
      }    
    }
    if ( pT < m_ptCut ) { continue; }
    cnv ? ++nSelConverted : ++nSelUnconverted;

    
    // create new particle and fill it
    LHCb::Particle* particle = new LHCb::Particle( ); // photon constructor
    particle -> setParticleID( LHCb::ParticleID (m_Id) );
    particle -> setProto( pp ) ;

    // mass and mass uncertainties
    particle -> setMeasuredMass( m_mas   ) ; 
    particle -> setMeasuredMassErr( 0   ) ; // the mass is EXACT zero!
    
    // confidence level
    particle  -> setConfLevel  ( CL    ) ;



    // fill photon parameters (4-momentum, vertex and correlations)
    LHCb::CaloParticle calopart(particle, m_point , m_pointErr);
    calopart.updateParticle();

    // Massive object or Hcal correction
    // Warning : covariant matrix should be modified accordingly -> to be included in CaloParticle ...
    if( m_addHcal || m_mas > 0 )particle->setMomentum( Gaudi::LorentzVector(px,py,pz,E) );
    

    if( calopart.status() !=0 ){
      delete particle ;
      nSkip++;
      warning() << "Unable to fill photon parameters, skip particle [" << nSkip << "]"<< endmsg;
      warning() << " CaloParticle status/flag : " << calopart.status() << "/" << calopart.flag();
      cnv ? --nSelConverted : --nSelUnconverted;
      continue ;                                                
    }
    
    //
    if (msgLevel(MSG::VERBOSE)){
      verbose() << "----- Single "<<m_part << " found [" << nSelConverted+nSelUnconverted << "]"<<endmsg;
      verbose() << "Pt : " << momentum.pt() << endmsg;
      verbose() << "CL : " << CL << endmsg;
      verbose() << "Chi2 " << pp->info(LHCb::ProtoParticle::CaloTrMatch,-999.) << endmsg;
      verbose() << "Converted (SPD digits)? : " << cnv << endmsg;      
      verbose() << "CaloDeposit : " <<pp->info(LHCb::ProtoParticle::CaloDepositID,-999.) <<endmsg;
      if( m_addHcal)verbose() << " Hcal factor : " << (1.+eHcal) << endmsg;
      if( m_mas> 0.)verbose() << " Mass : " << m_mas << endmsg;
      
      verbose() << " "<< endmsg;

    }
    
    // add the particle to the container
    particles.push_back( particle );
  }
  m_count[1] += nSelUnconverted ;
  m_count[2] += nSelConverted ;
  counter("Created converted photons")+= nSelConverted;
  counter("Created unConverted photons")+= nSelUnconverted;
  

  if (msgLevel(MSG::DEBUG)){
    debug() << " " << endmsg;
    debug() << "-----------------------" << endmsg;
    debug() << " Filtered and created :" << endmsg;
    debug() << " --> " << nSelConverted + nSelUnconverted << " photons (among " 
            << nConverted + nUnconverted <<")"<< endmsg;
    debug() << "   -> " << nSelConverted 
            << " Converted Photons (among " << nConverted <<")"<< endmsg;
    debug() << "   -> " << nSelUnconverted 
            << " Unconverted Photons (among " << nUnconverted <<")"<< endmsg;
    debug() << " Skipped : " << nSkip << endmsg;
    debug() << "-----------------------" << endmsg;
  }
  
  return StatusCode::SUCCESS ;
}

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
      double pid = pp->info(LHCb::ProtoParticle::PhotonID, 1.  ) ;
      CL *= 0.5*(tanh(pid)+1);
    }
    else{ 
      Warning("confLevel(): PhotonID is not available" ) ; }
  }

  return CL ;
}
