// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
#include "NeutralProtoPAlg.h"
// ============================================================================
/** @file
 *  Implementation file for class NeutralProtoPAlg
 *  @date 2006-06-09
 *  @author Olivier Deschamps
 */
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NeutralProtoPAlg )
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
  NeutralProtoPAlg::NeutralProtoPAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_hyposLocations    ()
  , m_caloTrMatch_bad   (  1.e+6 )
  , m_caloDepositID_bad ( -1.e+6 )
  , m_showerShape_bad   ( -1.e+6 )
  , m_clusterMass_bad   ( -1.e+6 )
  , m_photonID_bad      ( -1.e+6 )
  , m_light_mode        ( false )
  , m_estimator(0) {

  // declare the properties
  declareProperty ( "HyposLocations"        , m_hyposLocations   ) ;
  declareProperty ( "ProtoParticleLocation" ,  m_protoLocation   ) ;
  declareProperty ( "LightMode"      , m_light_mode ,
                    "Use 'light' mode and do not collect all information. Useful for Calibration." ) ;

  // default location from context()
  using namespace LHCb::CaloHypoLocation;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Photons      ) ) ;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s   ) ) ;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , SplitPhotons ) ) ;
  //  m_protoLocation = LHCb::CaloAlgUtils::PathFromContext( flag , LHCb::ProtoParticleLocation::Neutrals );
  m_protoLocation = LHCb::ProtoParticleLocation::Neutrals ;
}
// ============================================================================
// Destructor
// ============================================================================
NeutralProtoPAlg::~NeutralProtoPAlg() {}
// ============================================================================
// Initialization
// ============================================================================
StatusCode NeutralProtoPAlg::initialize(){
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc )
    info() << " Hypothesis loaded from " << *loc << endmsg;

  //
  if ( lightMode() )
    info() << "Neutral protoparticles will be created in 'Light' Mode" << endmsg ;


  m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimator",this);
  m_estimator->hypo2Calo()->_setProperty("Seed", "false").ignore();
  m_estimator->hypo2Calo()->_setProperty("PhotonLine", "true").ignore();
  m_estimator->hypo2Calo()->_setProperty("AddNeighbors", "false").ignore();


  return sc;
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode NeutralProtoPAlg::execute(){

  // create and register the output container
  LHCb::ProtoParticles* protos = NULL;
  if ( !lightMode() && exist<LHCb::ProtoParticles>(m_protoLocation) ){
    Warning( "Existing ProtoParticle container at " +m_protoLocation+ " found -> Will replace", StatusCode::SUCCESS, 1).ignore();
    counter("Replaced Proto")+=1;
    protos = get<LHCb::ProtoParticles>(m_protoLocation);
    protos->clear();
  }
  else{
    protos = new LHCb::ProtoParticles();
    put( protos , m_protoLocation ) ;
  }


  if( NULL == protos )return Warning("NeutralProto container points to NULL ",StatusCode::SUCCESS);


  //------ loop over all caloHypos and create the protoparticles
  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc ) {

    // Load the CaloHypo objects if the container exists
    const LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos>( *loc );
    if ( NULL == hypos ){
      if( msgLevel(MSG::DEBUG) ) debug()<< "No CaloHypo at '" << (*loc) + "'"<<endmsg ;
      counter("No " + (*loc) + " container") += 1;
      continue;
    }

    if ( msgLevel(MSG::DEBUG) )debug() << "CaloHypo loaded at " << *loc << " (# " << hypos->size()<<")"<< endmsg;
    int count = 0 ;
    for ( LHCb::CaloHypos::const_iterator ihypo = hypos->begin() ; hypos->end() != ihypo  ; ++ihypo ){
      const LHCb::CaloHypo* hypo = *ihypo ;
      if( 0 == hypo ) { continue ; }
      count++;
      LHCb::ProtoParticle* proto = new LHCb::ProtoParticle() ;
      protos->insert( proto ) ;

      // fill protoparticle
      proto-> addToCalo( hypo ) ;
      if ( !lightMode() ){
        std::ostringstream type("");
        type << hypo->hypothesis();
        std::string hypothesis = type.str();
        using namespace CaloDataType;
        proto -> addInfo ( LHCb::ProtoParticle::CaloTrMatch     ,  m_estimator->data(hypo, ClusterMatch, m_caloTrMatch_bad )) ;
        proto -> addInfo ( LHCb::ProtoParticle::PhotonID        ,  m_estimator->data(hypo, NeutralID   , m_photonID_bad    )) ;
        proto -> addInfo ( LHCb::ProtoParticle::IsPhoton        ,  m_estimator->data(hypo, isPhoton    , m_photonID_bad    )) ;
        double dep = (m_estimator->data(hypo,  ToSpdM ) > 0) ? -1. : +1.;
        dep *= m_estimator->data(hypo,  ToPrsE, m_caloDepositID_bad );
        proto -> addInfo ( LHCb::ProtoParticle::CaloDepositID   , dep ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ShowerShape     , m_estimator->data(hypo, Spread , m_showerShape_bad  ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ClusterMass     , m_estimator->data(hypo, HypoM  , m_clusterMass_bad   ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralSpd  , m_estimator->data(hypo, HypoSpdM )  ) ;
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralPrs  , m_estimator->data(hypo, HypoPrsE )  );
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralEcal , m_estimator->data(hypo, ClusterE )  );
        proto -> addInfo(LHCb::ProtoParticle::CaloNeutralID ,  m_estimator->data(hypo, CellID )  );
        proto -> addInfo(LHCb::ProtoParticle::CaloNeutralHcal2Ecal  ,  m_estimator->data(hypo, Hcal2Ecal )  );
        proto -> addInfo(LHCb::ProtoParticle::CaloNeutralE49        ,  m_estimator->data(hypo, E49 )  );
        // rejouter H/E, E49
        counter("ClusterMatch  for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloTrMatch    , m_caloTrMatch_bad   );
        counter("PhotonID      for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::PhotonID       , m_photonID_bad      );
        counter("CaloDepositID for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloDepositID  , m_caloDepositID_bad );
        counter("ShowerShape   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::ShowerShape    , m_showerShape_bad   );
        counter("ClusterMass   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::ClusterMass    , m_clusterMass_bad   );
        counter("NeutralSpd    for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralSpd , 0.  );
        counter("NeutralPrs    for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralPrs , 0.  );
        counter("NeutralEcal   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralEcal, 0.  );
        counter("NeutralHcal2Ecal   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralHcal2Ecal, 0.  );
        counter("NeutralE49   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralE49, 0.  );
      }

      if ( msgLevel(MSG::VERBOSE) ){
        MsgStream& log = verbose() ;
        log << "Neutral ProtoParticle created " << (*(proto->calo().begin() ))-> hypothesis() << endmsg;
        if ( !lightMode() ){
          log << "Estimator Chi2    = " << proto -> info(LHCb::ProtoParticle::CaloTrMatch ,m_caloTrMatch_bad) << endmsg;
          log << "Estimator Deposit = " << proto -> info(LHCb::ProtoParticle::CaloDepositID ,m_caloDepositID_bad) << endmsg;
          log << "Estimator ShShape = " << proto -> info(LHCb::ProtoParticle::ShowerShape,m_showerShape_bad) << endmsg;
          log << "Estimator ClMass  = " << proto -> info(LHCb::ProtoParticle::ClusterMass,m_clusterMass_bad) << endmsg;
          log << "Estimator Ph ID   = " << proto -> info(LHCb::ProtoParticle::PhotonID    ,m_photonID_bad) << endmsg;
          log << "Spd Hits          = " << proto -> info(LHCb::ProtoParticle::CaloNeutralSpd ,0.) << endmsg;
          log << "Prs deposit       = " << proto -> info(LHCb::ProtoParticle::CaloNeutralPrs ,0.) << endmsg;
          log << "Hcal/Ecal         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralHcal2Ecal ,0.) << endmsg;
          log << "E2x2/E3x3         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralE49 ,0.) << endmsg;
          log << "seedID            = " << LHCb::CaloCellID((int)proto -> info(LHCb::ProtoParticle::CaloNeutralID ,0)) << endmsg;
        }
      }
    } // loop over CaloHypos
    counter( *loc + "=>" + m_protoLocation) += count;
  } // loop over HyposLocations

  if ( msgLevel(MSG::DEBUG) )debug() << "# Neutral ProtoParticles created : " << protos -> size() << endmsg;

  counter ("#protos in " + m_protoLocation) += protos->size() ;
  return StatusCode::SUCCESS;
}
// ============================================================================
//  Finalize
// ============================================================================
StatusCode NeutralProtoPAlg::finalize(){
  //
  if ( lightMode() )
  { info() << "Neutral protoparticles have been created in 'Light' Mode" << endmsg ; }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
