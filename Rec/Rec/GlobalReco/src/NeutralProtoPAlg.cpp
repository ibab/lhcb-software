// $Id: NeutralProtoPAlg.cpp,v 1.19 2009-08-21 17:08:03 odescham Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloMomentum.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// local
// ============================================================================
#include "NeutralProtoPAlg.h"
// ============================================================================
/** @file 
 *  Implementation file for class NeutralProtoPAlg
 *  @date 2006-06-09 
 *  @author Olivier Deschamps
 */
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NeutralProtoPAlg );
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
NeutralProtoPAlg::NeutralProtoPAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_hyposLocations    ()
    , m_matchLocation     ()
    , m_spdprsType        ( "CaloSingleGammaTool")
    , m_spdprsName        ("SpdPrsID"   )
    , m_spdprs            ( 0           )
    , m_photonIDType      ( "CaloPhotonEstimatorTool")
    , m_photonIDName      ( "PhotonPID"  )
    , m_photonID          ( 0            )
    , m_caloTrMatch_bad   (  1.e+6 )
    , m_caloDepositID_bad ( -1.e+6 )
    , m_showerShape_bad   ( -1.e+6 )
    , m_clusterMass_bad   ( -1.e+6 )
    , m_photonID_bad      ( -1.e+6 )
    , m_calo              ( DeCalorimeterLocation:: Ecal )
    , m_light_mode        ( false ){
  
  // declare the properties
  declareProperty ( "HyposLocations"        , m_hyposLocations   ) ;
  declareProperty ( "ClusterMatchLocation"  , m_matchLocation    ) ;
  declareProperty ( "ProtoParticleLocation" ,  m_protoLocation   ) ;
  declareProperty ( "SpdPrsIDType"   , m_spdprsType   ) ;
  declareProperty ( "SpdPrsIDName"   , m_spdprsName   ) ;
  declareProperty ( "PhotonIDType"   , m_photonIDType ) ;
  declareProperty ( "PhotonIDName"   , m_photonIDName ) ;
  declareProperty ( "LightMode"      , m_light_mode , 
                    "Use 'light' mode and donto colelct all information, useful for Calibration" ) ;  
  
  // default location from context()
  std::string flag = context();
  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  using namespace LHCb::CaloHypoLocation;
  m_matchLocation = LHCb::CaloAlgUtils::PathFromContext( flag , ClusterMatch , ClusterMatchHlt );
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , Photons      , PhotonsHlt      ) );
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , SplitPhotons , SplitPhotonsHlt ) );
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , MergedPi0s   , MergedPi0sHlt   ) );
  m_protoLocation = LHCb::CaloAlgUtils::PathFromContext( flag , LHCb::ProtoParticleLocation::Neutrals, 
                                                      LHCb::ProtoParticleLocation::HltNeutrals);
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
  
  debug() << "==> Initialize" << endmsg;
  
  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ; 
        m_hyposLocations.end() != location ; ++location )
    info() << " Hypothesis loaded from " << *location << endmsg;
  
  //
  if ( lightMode() )
    info() << "Neutral protoparticles will be created in 'Light' Mode" << endmsg ;

  return sc;
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode NeutralProtoPAlg::execute(){
  
  // create and register the output container
  LHCb::ProtoParticles* protos = NULL;
  if ( exist<LHCb::ProtoParticles>(m_protoLocation) ){
    // get existing contianer, clear, and reuse
    Warning( "Existing ProtoParticle container at " + m_protoLocation + " found -> Will replace", StatusCode::SUCCESS );
    protos = get<LHCb::ProtoParticles>(m_protoLocation);
    protos->clear();
  }
  else{
    protos = new LHCb::ProtoParticles();
    put( protos , m_protoLocation ) ;
  }
  
  // get the relation table
  const LHCb::Calo2Track::IClusTrTable* table =  0 ;
  if ( !lightMode() ){
    if ( !exist<LHCb::Calo2Track::IClusTrTable>( m_matchLocation )){
      return Warning ( "No matching table at '"+ m_matchLocation + "'" ,StatusCode::SUCCESS )  ;
    }
    table = get<LHCb::Calo2Track::IClusTrTable> ( m_matchLocation ) ;
    if ( 0 == table     ) { return Error("Table* points to NULL!");}
  }
  
  // loop over all caloHypos and create the protoparticles
  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ;
        m_hyposLocations.end() != location ; ++location ) {
    
    // Load the CaloHypo objects if the container exists
    if ( !exist<LHCb::CaloHypos>( *location )){
      Warning ( "No CaloHypo at '" + (*location) + "'").ignore() ;
      continue;
    }
    const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( *location );
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "CaloHypo loaded at " << *location  
              << " (# " << hypos->size()<<")"<< endmsg;
    }
    int count = 0 ;
    for ( LHCb::CaloHypos::const_iterator ihypo = hypos->begin() ;
          hypos->end() != ihypo  ; ++ihypo ){
      const LHCb::CaloHypo* hypo = *ihypo ;
      if( 0 == hypo ) { continue ; }
      
      //
      count++;
      LHCb::ProtoParticle* proto = new LHCb::ProtoParticle() ;
      protos->insert( proto ) ;
      
      // fill protoparticle
      proto-> addToCalo( hypo ) ;
      
      if ( !lightMode() ){
        //
        if ( 0 != table ) { proto -> addInfo ( LHCb::ProtoParticle::CaloTrMatch     , caloTrMatch  ( hypo , table ) ) ; }
        //
        proto -> addInfo ( LHCb::ProtoParticle::CaloDepositID   , caloDepositID( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ShowerShape     , showerShape  ( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ClusterMass     , clusterMass  ( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::PhotonID        , photonID     ( hypo ) ) ;
        // Spd digit info (photon conversion) + Prs + Ecal Cluster
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralSpd  , CaloSpd  ( hypo ) );
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralPrs  , CaloPrs  ( hypo ) );
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralEcal , CaloEcal ( hypo ) );
      }
      
      if ( msgLevel(MSG::VERBOSE) ){
        MsgStream& log = verbose() ;
        log << "Neutral ProtoParticle created " << (*(proto->calo().begin() ))-> hypothesis() << endmsg;
        if ( !lightMode() ){
          log << "Estimator Chi2    = " << proto -> info(LHCb::ProtoParticle::CaloTrMatch ,-1.) << endmsg;
          log << "Estimator Deposit = " << proto -> info(LHCb::ProtoParticle::CaloDepositID ,-1.) << endmsg;
          log << "Estimator ShShape = " << proto -> info(LHCb::ProtoParticle::ShowerShape,-1.) << endmsg;
          log << "Estimator ClMass  = " << proto -> info(LHCb::ProtoParticle::ClusterMass,-1.) << endmsg;
          log << "Estimator Ph ID   = " << proto -> info(LHCb::ProtoParticle::PhotonID    ,-1.) << endmsg;
          log << "Spd Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralSpd ,0.) << endmsg;
          log << "Prs Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralPrs ,0.) << endmsg;
          log << "Ecal Cluster      = " << proto -> info(LHCb::ProtoParticle::CaloNeutralEcal ,0.) << endmsg;
        }
      }
    } // loop over CaloHypos
    counter( *location ) += count;
  } // loop over HyposLocations
  
  if ( msgLevel(MSG::DEBUG) ){ 
    debug() << "# Neutral ProtoParticles created : " << protos -> size() << endmsg;
  }
  counter ("#proto") += protos->size() ;
  return StatusCode::SUCCESS;
}
// ============================================================================
//  Finalize
// ============================================================================
StatusCode NeutralProtoPAlg::finalize() 
{
  m_spdprs   = 0 ;
  m_photonID = 0 ;
  //
  if ( lightMode() ) 
  { info() << "Neutral protoparticles have been created in 'Light' Mode" << endmsg ; }  
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::CaloTrMatch etimator
 *  @see ProtoParticle
 *  @return the value of the estimator
 */
//=============================================================================
double NeutralProtoPAlg::caloTrMatch
( const LHCb::CaloHypo*                 hypo  , 
  const LHCb::Calo2Track::IClusTrTable* table )  const{
  
  // select the first Ecal cluster
  const LHCb::CaloHypo::Clusters& clusters = hypo->clusters();
  LHCb::CaloHypo::Clusters::const_iterator cluster = std::find_if( clusters.begin() , clusters.end() , m_calo );
  if ( clusters.end() == cluster ) { return m_caloTrMatch_bad; }
  
  // get to all related tracks
  const LHCb::Calo2Track::IClusTrTable::Range range = table->relations( *cluster ) ;
  if ( range.empty()            )  { return m_caloTrMatch_bad; }
  
  // get minimal value
  return range.front().weight();
}
// ============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::ClusterMass etimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo
 *  @return the value of the estimator
 */
// ============================================================================
double NeutralProtoPAlg::clusterMass ( const LHCb::CaloHypo*  hypo  )  const
{
  // CaloMomentum no longer available in the new Event Model
  // reconstruct clusterMass of pi0 from the SmartRef'ed splitPhotons
  // This estiimator could be generalized to Single Photon hypothesis 
  // (need to adapt CaloReco)
  
  if( LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ) 
  {
    LHCb::CaloMomentum momentum(hypo);
    return momentum.mass() ;                                          // RETURN 
  }
  return m_clusterMass_bad ;
}
// ============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::ShowerShape etimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo
 *  @return the value of the estimator
 */
// ============================================================================
double NeutralProtoPAlg::showerShape ( const LHCb::CaloHypo*  hypo  )  const
{
  // get the position
  const LHCb::CaloPosition* position = hypo->position();
  if( 0 == position ) {
    if( hypo->hypothesis() != LHCb::CaloHypo::Pi0Merged )
    { Warning("showerShape(): CaloPosition* points to NULL!"); }
    return m_showerShape_bad;
  }
  return position->spread()( 0 , 0 )+ position->spread()( 1 , 1 ) ;
}
// ============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::CaloDepositID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo
 *  @return the value of the estimator
 */
// ============================================================================
double  NeutralProtoPAlg::caloDepositID ( const LHCb::CaloHypo*  hypo  )  const
{
  if( hypo->hypothesis() != LHCb::CaloHypo::Pi0Merged )
  { return spdprs() -> likelihood( hypo ) ; }                         // REUTRN 
  //
  return  m_caloDepositID_bad ;
}
// ============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::PhotonID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo
 *  @return the value of the estimator
 */
// ============================================================================
double NeutralProtoPAlg::photonID  ( const LHCb::CaloHypo*  hypo  )  const
{
  if( hypo->hypothesis() == LHCb::CaloHypo::Photon               ||
      hypo->hypothesis() == LHCb::CaloHypo::PhotonFromMergedPi0  ||
      hypo->hypothesis() == LHCb::CaloHypo::BremmstrahlungPhoton ||
      hypo->hypothesis() == LHCb::CaloHypo::BremmstrahlungPhoton  )
  { return photonID() -> likelihood( hypo ) ; }                       // RETURN 
  return m_photonID_bad;
}
// ============================================================================
double NeutralProtoPAlg::CaloSpd  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0;
  const LHCb::CaloHypo::Digits& digits = hypo->digits();
  //
  LHCb::CaloDataFunctor::IsFromCalo<const LHCb::CaloDigit* > isSpd ( DeCalorimeterLocation::Spd );
  //
  LHCb::CaloHypo::Digits::const_iterator it = 
    std::find_if ( digits.begin(),digits.end(),isSpd );
  return  ( it == digits.begin() ) ? 0. : +1.;
}
// ============================================================================
double NeutralProtoPAlg::CaloPrs  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0;
  const LHCb::CaloHypo::Digits& digits = hypo->digits();
  //
  LHCb::CaloDataFunctor::IsFromCalo<const LHCb::CaloDigit* > isPrs( DeCalorimeterLocation::Prs );
  //
  double caloPrs = 0. ;
  //
  for( LHCb::CaloHypo::Digits::const_iterator id = digits.begin(); id != digits.end() ; ++id)
  { if ( 0 != *id && isPrs ( *id ) ) { caloPrs += (*id)->e(); } }
  //
  return caloPrs  ;
}
// ============================================================================
double NeutralProtoPAlg::CaloEcal  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0.;
  const SmartRefVector<LHCb::CaloCluster>& clusters = hypo->clusters();
  if( 0 == clusters.size())return 0.;
  SmartRefVector<LHCb::CaloCluster>::const_iterator icluster = clusters.begin();
  const LHCb::CaloCluster* cluster = *icluster;
  if( NULL == cluster) return 0;
  return cluster->e();
}
// ============================================================================
// The END 
// ============================================================================
