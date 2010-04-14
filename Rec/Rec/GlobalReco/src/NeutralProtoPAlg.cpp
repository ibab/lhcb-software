// $Id: NeutralProtoPAlg.cpp,v 1.23 2010-04-14 12:50:19 odescham Exp $
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
  , m_caloTrMatch_bad   (  1.e+6 )
  , m_caloDepositID_bad ( -1.e+6 )
  , m_showerShape_bad   ( -1.e+6 )
  , m_clusterMass_bad   ( -1.e+6 )
  , m_photonID_bad      ( -1.e+6 )
  , m_calo              ( DeCalorimeterLocation:: Ecal )
  , m_light_mode        ( false )
  , m_first(true){
  
  // declare the properties
  declareProperty ( "HyposLocations"        , m_hyposLocations   ) ;
  declareProperty ( "ClusterMatchLocation"  , m_matchLocation    ) ;
  declareProperty ( "NeutralIDLocations"    , m_pidLocations      ) ;
  declareProperty ( "ProtoParticleLocation" ,  m_protoLocation   ) ;
  declareProperty ( "LightMode"      , m_light_mode , 
                    "Use 'light' mode and donto colelct all information, useful for Calibration" ) ;  
  
  // default location from context()
  std::string flag = context();
  using namespace LHCb::Calo2Track;
  using namespace LHCb::CaloIdLocation;
  using namespace LHCb::CaloHypoLocation;
  m_matchLocation = LHCb::CaloAlgUtils::PathFromContext( flag , ClusterMatch );
  m_pidLocations["Photon"]    = LHCb::CaloAlgUtils::PathFromContext( flag , PhotonID );
  m_pidLocations["Pi0Merged"]  = LHCb::CaloAlgUtils::PathFromContext( flag , MergedID );
  m_pidLocations["PhotonFromMergedPi0"]  = LHCb::CaloAlgUtils::PathFromContext( flag , PhotonFromMergedID );
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , Photons      ) ) ;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , MergedPi0s   ) ) ;
  m_hyposLocations.push_back( LHCb::CaloAlgUtils::PathFromContext( flag , SplitPhotons ) ) ;
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
  
  debug() << "==> Initialize" << endmsg;
  
  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc )
    info() << " Hypothesis loaded from " << *loc << endmsg;
  
  //
  if ( lightMode() )
    info() << "Neutral protoparticles will be created in 'Light' Mode" << endmsg ;


  // Spd/Prs matching based on CaloHypo2Calo :
  m_toCalo = tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2Calo" , this );
  // Warning : the algorithm settings overwrite the tool settings #TODO delegate to a dedicated tool ALL cluster parameters
  m_toCalo->_setProperty("Seed", "false" ).ignore();
  m_toCalo->_setProperty("PhotonLine", "true").ignore();
  m_toCalo->_setProperty("AddNeighbors","false").ignore();


  return sc;
}
// ============================================================================
// Main execution
// ============================================================================
StatusCode NeutralProtoPAlg::execute(){
  
  // create and register the output container
  LHCb::ProtoParticles* protos = NULL;
  if ( !lightMode() && exist<LHCb::ProtoParticles>(m_protoLocation) ){
    if( m_first ){
      Warning( "Existing ProtoParticle container at " + m_protoLocation + " found -> Will replace", StatusCode::SUCCESS).ignore();
      m_first = false;
    }
    counter("Replaced Proto")+=1;
    protos = get<LHCb::ProtoParticles>(m_protoLocation);
    protos->clear();
  }
  else{
    protos = new LHCb::ProtoParticles();
    put( protos , m_protoLocation ) ;
  }
  

  if( NULL == protos )return Warning("NeutralProto container points to NULL ",StatusCode::SUCCESS);


  // get the relation tables
  const LHCb::Calo2Track::IClusTrTable* table =  0 ;
  if ( !lightMode() ){
    // ClusterTrack matching table
    if ( exist<LHCb::Calo2Track::IClusTrTable>( m_matchLocation ) )
      table = get<LHCb::Calo2Track::IClusTrTable> ( m_matchLocation ) ;
    else{
      Warning ( "No matching table at '"+ m_matchLocation + "'" ,StatusCode::SUCCESS ).ignore()  ;
      counter( "No matching table " + m_matchLocation) += 1;
    }
    if ( 0 == table     )Error("Matching Table* points to NULL!",StatusCode::SUCCESS).ignore();

    // PID tables
    for( std::map<std::string,std::string>::iterator l = m_pidLocations.begin(); m_pidLocations.end()!=l; ++l){
      std::string loc = l->second;
      std::string  hypothesis =  l->first;
      if ( exist<LHCb::Calo2Track::IHypoEvalTable>( loc ) ){        
        m_idTable[hypothesis] = get<LHCb::Calo2Track::IHypoEvalTable> ( loc ) ;
        if( NULL == m_idTable[hypothesis])Error("ID table for '"+hypothesis+"' at " + loc + " points to NULL !"
                                                ,StatusCode::SUCCESS).ignore();
      }
      else{
        Warning ("No ID table at '"+ loc + "' for "  + hypothesis ,StatusCode::SUCCESS ).ignore()  ;
        counter( "Missing "+ loc) += 1; 
      }
    }
  }
  
  //------ loop over all caloHypos and create the protoparticles
  for ( std::vector<std::string>::const_iterator loc = m_hyposLocations.begin() ; m_hyposLocations.end() != loc ; ++loc ) {
    
    // Load the CaloHypo objects if the container exists
    if ( !exist<LHCb::CaloHypos>( *loc )){
      Warning ( "No CaloHypo at '" + (*loc) + "'",StatusCode::SUCCESS).ignore() ;
      continue;
    }

    const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( *loc );
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
        const LHCb::Calo2Track::IHypoEvalTable* idTable =  NULL ;
        std::map<std::string, LHCb::Calo2Track::IHypoEvalTable*>::iterator it = m_idTable.find(hypothesis);
        if( it != m_idTable.end())idTable = it->second;


        if( NULL != table   )proto -> addInfo ( LHCb::ProtoParticle::CaloTrMatch     , caloTrMatch  ( hypo , table  ) ) ; 
        if( NULL != idTable )proto -> addInfo ( LHCb::ProtoParticle::PhotonID        , neutralID    ( hypo , idTable) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::CaloDepositID   , caloDepositID( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ShowerShape     , showerShape  ( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::ClusterMass     , clusterMass  ( hypo ) ) ;
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralSpd  , CaloSpd  ( hypo ) );
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralPrs  , CaloPrs  ( hypo ) );
        proto -> addInfo ( LHCb::ProtoParticle::CaloNeutralEcal , CaloEcal ( hypo ) );
        counter("ClusterMatch  for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloTrMatch    , m_caloTrMatch_bad   );
        counter("PhotonID      for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::PhotonID       , m_photonID_bad      );
        counter("CaloDepositID for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloDepositID  , m_caloDepositID_bad );
        counter("ShowerShape   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::ShowerShape    , m_showerShape_bad   );
        counter("ClusterMass   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::ClusterMass    , m_clusterMass_bad   );
        counter("NeutralSpd    for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralSpd , 0.  );
        counter("NeutralPrs    for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralPrs , 0.  );
        counter("NeutralEcal   for '" +hypothesis+"'") += proto->info(LHCb::ProtoParticle::CaloNeutralEcal, 0.  );
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
          log << "Spd Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralSpd ,0.) << endmsg;
          log << "Prs Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralPrs ,0.) << endmsg;
          log << "Ecal Cluster      = " << proto -> info(LHCb::ProtoParticle::CaloNeutralEcal ,0.) << endmsg;
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
//=============================================================================
/*  helpful technical method to evaluate
 *  the ProtoParticle::CaloTrMatch etimator
 *  @see ProtoParticle
 *  @return the value of the estimator
 */
//=============================================================================
double NeutralProtoPAlg::caloTrMatch ( const LHCb::CaloHypo* hypo  ,const LHCb::Calo2Track::IClusTrTable* table )  const{  
  if( NULL == table )return m_caloTrMatch_bad;
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
  
  if( LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
    LHCb::CaloMomentum momentum(hypo);
    return momentum.mass() ;          
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
double NeutralProtoPAlg::showerShape ( const LHCb::CaloHypo*  hypo  )  const{
  // get the position
  const LHCb::CaloPosition* position = hypo->position();
  if( 0 == position ) {
    if( hypo->hypothesis() != LHCb::CaloHypo::Pi0Merged )Warning("showerShape(): CaloPosition* points to NULL!"); 
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
double  NeutralProtoPAlg::caloDepositID ( const LHCb::CaloHypo*  hypo  )  const{

  if( hypo->hypothesis() != LHCb::CaloHypo::Pi0Merged ){
    double spd = m_toCalo -> energy( *hypo, "Spd" ) ; 
    double prs = m_toCalo -> energy( *hypo, "Prs" ) ;
    return (spd == 0 ) ? prs : -1.*prs;
  } 

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
double NeutralProtoPAlg::neutralID  ( const LHCb::CaloHypo*  hypo  , const LHCb::Calo2Track::IHypoEvalTable* table)  const{
  if( NULL == table )return m_photonID_bad;
  const LHCb::Calo2Track::IHypoEvalTable::Range range = table->relations( hypo ) ;
  if ( range.empty()            )return m_photonID_bad;  
  return range.front().to();
}
// ============================================================================
double NeutralProtoPAlg::CaloSpd  ( const LHCb::CaloHypo*  hypo  )  const{
  //
  if( 0 == hypo) return 0;
  const LHCb::CaloHypo::Digits& digits = hypo->digits();
  //
  LHCb::CaloDataFunctor::IsFromCalo<const LHCb::CaloDigit* > isSpd ( DeCalorimeterLocation::Spd );
  //
  LHCb::CaloHypo::Digits::const_iterator it = 
    std::find_if ( digits.begin(),digits.end(),isSpd );
  return  ( it == digits.end() ) ? 0. : +1.;
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
  for( LHCb::CaloHypo::Digits::const_iterator id = digits.begin(); id != digits.end() ; ++id){ 
    if ( 0 != *id && isPrs ( *id ) ) { caloPrs += (*id)->e(); } 
}
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
