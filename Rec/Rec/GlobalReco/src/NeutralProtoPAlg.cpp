// $Id: NeutralProtoPAlg.cpp,v 1.15 2008-04-23 07:59:38 odescham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Math
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
// from CaloUtils
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloMomentum.h"
// From Event
#include "Event/CaloDataFunctor.h"
// From CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "NeutralProtoPAlg.h"


//-----------------------------------------------------------------------------
// Implementation file for class : NeutralProtoPAlg
//
// 2006-06-09 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NeutralProtoPAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NeutralProtoPAlg::NeutralProtoPAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_hyposLocations   ()
  , m_matchLocation    ( LHCb::CaloIdLocation::ClusterMatch )
  , m_spdprsType       ("CaloSingleGammaTool")
  , m_spdprsName       ("SpdPrsID"   )
  , m_spdprs           ( 0           )
  , m_photonIDType     ( "CaloPhotonEstimatorTool")
  , m_photonIDName     ( "PhotonPID"  )
  , m_photonID         ( 0            )
  , m_caloTrMatch_bad   (  1.e+6 )
  , m_caloDepositID_bad ( -1.e+6 )
  , m_showerShape_bad   ( -1.e+6 )
  , m_clusterMass_bad   ( -1.e+6 )
  , m_photonID_bad      ( -1.e+6 )
  , m_calo             ( DeCalorimeterLocation:: Ecal )
  , m_nEvts        ( 0 )
{
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::Photons      );
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::SplitPhotons );
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::MergedPi0s   );

  // declare the properties
  declareProperty( "HyposLocations"        , m_hyposLocations   ) ;
  declareProperty( "ClusterMatchLocation"  , m_matchLocation    ) ;

  declareProperty( "ProtoParticleLocation",
                   m_protoLocation =LHCb::ProtoParticleLocation::Neutrals ) ;

  declareProperty( "SpdPrsIDType"   , m_spdprsType   ) ;

  declareProperty( "SpdPrsIDName"   , m_spdprsName   ) ;
  declareProperty( "PhotonIDType"   , m_photonIDType ) ;
  declareProperty( "PhotonIDName"   , m_photonIDName ) ;
}
//=============================================================================
// Destructor
//=============================================================================
NeutralProtoPAlg::~NeutralProtoPAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode NeutralProtoPAlg::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ;
        m_hyposLocations.end() != location ; ++location )
  {
    info() << " Hypothesis loaded from " << *location << endreq;
  }

  // Initialize counters
  m_counts["All"] = 0;
  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ;
        m_hyposLocations.end() != location ; ++location ) {
    m_counts[(*location).substr(9) ] = 0;
  }
  


  // locate tools
  m_spdprs   = tool< ICaloHypoLikelihood>( m_spdprsType   ,  m_spdprsName ) ;
  if( 0 == m_spdprs ) { return StatusCode::FAILURE ; }

  m_photonID = tool< ICaloHypoLikelihood>( m_photonIDType ,  m_photonIDName ) ;
  if( 0 == m_photonID ) { return StatusCode::FAILURE ; }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode NeutralProtoPAlg::execute()
{

  ++m_nEvts;

  // create and register the output container
  LHCb::ProtoParticles* protos = NULL;
  if ( exist<LHCb::ProtoParticles>(m_protoLocation) )
  {
    // get existing contianer, clear, and reuse
    Warning( "Existing ProtoParticle container at " + m_protoLocation + 
             " found -> Will replace", StatusCode::SUCCESS );
    protos = get<LHCb::ProtoParticles>(m_protoLocation);
    protos->clear();
  }
  else
  {
    protos = new LHCb::ProtoParticles();
    put( protos , m_protoLocation ) ;
  }

  // get the relation table

  if ( !exist<LHCb::Calo2Track::IClusTrTable>( m_matchLocation ))  
  {
    warning() << "No matching table at " << m_matchLocation<< endreq ;
    return StatusCode::SUCCESS;
  }
  const LHCb::Calo2Track::IClusTrTable* table = get<LHCb::Calo2Track::IClusTrTable> ( m_matchLocation ) ;
  if ( 0 == table     ) { return Error("Table* points to NULL!");}
  // loop over all caloHypos and create the protoparticles
  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ;
        m_hyposLocations.end() != location ; ++location ) {

    // Load the CaloHypo objects if the container exists
    if ( !exist<LHCb::CaloHypos>( *location ))  {
      warning() << "No CaloHypo at " << *location  << endreq ;
      continue;
    }
    const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( *location );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "CaloHypo loaded at " << *location  << " (# " << hypos->size()<<")"<< endmsg;

    for ( LHCb::CaloHypos::const_iterator ihypo = hypos->begin() ;
          hypos->end() != ihypo  ; ++ihypo )  {
      const LHCb::CaloHypo* hypo = *ihypo ;
      if( 0 == hypo ) { continue ; }

      //
      LHCb::ProtoParticle* proto = new LHCb::ProtoParticle() ;
      protos->insert( proto ) ;


      // fill protoparticle
      proto-> addToCalo( hypo ) ;
      proto-> addInfo(LHCb::ProtoParticle::CaloTrMatch  , caloTrMatch  ( hypo , table ) ) ;
      proto-> addInfo(LHCb::ProtoParticle::CaloDepositID, caloDepositID( hypo ) ) ;
      proto-> addInfo(LHCb::ProtoParticle::ShowerShape  , showerShape  ( hypo ) ) ;
      proto-> addInfo(LHCb::ProtoParticle::ClusterMass  , clusterMass  ( hypo ) ) ;
      proto-> addInfo(LHCb::ProtoParticle::PhotonID     , photonID     ( hypo ) ) ;
      // Spd digit info (photon conversion) + Prs + Ecal Cluster
      proto->addInfo(LHCb::ProtoParticle::CaloNeutralSpd, CaloSpd( hypo ));
      proto->addInfo(LHCb::ProtoParticle::CaloNeutralPrs, CaloPrs( hypo ));
      proto->addInfo(LHCb::ProtoParticle::CaloNeutralEcal,CaloEcal(hypo ));

      ++m_counts["All"];
      ++m_counts[(*location).substr(9) ] ;

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Neutral ProtoParticle created " << (*(proto->calo().begin() ))-> hypothesis() << endmsg;
        verbose() << "Estimator Chi2    =" << proto -> info(LHCb::ProtoParticle::CaloTrMatch ,-1.) << endmsg;
        verbose() << "Estimator Deposit = " << proto -> info(LHCb::ProtoParticle::CaloDepositID ,-1.) << endmsg;
        verbose() << "Estimator ShShape = " << proto -> info(LHCb::ProtoParticle::ShowerShape,-1.) << endmsg;
        verbose() << "Estimator ClMass  = " << proto -> info(LHCb::ProtoParticle::ClusterMass,-1.) << endmsg;
        verbose() << "Estimator Ph ID   = " << proto -> info(LHCb::ProtoParticle::PhotonID    ,-1.) << endmsg;
        verbose() << "Spd Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralSpd ,0.) << endmsg;
        verbose() << "Prs Digit         = " << proto -> info(LHCb::ProtoParticle::CaloNeutralPrs ,0.) << endmsg;
        verbose() << "Ecal Cluster      = " << proto -> info(LHCb::ProtoParticle::CaloNeutralEcal ,0.) << endmsg;
      }

    } // loop over CaloHypos
  } // loop over HyposLocations

  if ( msgLevel(MSG::DEBUG) ) debug() << "# Neutral ProtoParticles created : " << protos -> size() << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode NeutralProtoPAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  
  if(m_nEvts > 0 ){

    info() << " -----------------------"<< endreq;
    info() << " Created = " << (double) m_counts["All"] / (double) m_nEvts
           << " Neutral ProtoParticles/event" << endreq;
    for( std::map<std::string,long>::iterator iprototype = m_counts.begin();
         iprototype != m_counts.end(); iprototype ++){
      if((*iprototype).first != "All")
        info() << "  -> " << (double) (*iprototype).second / (double) m_nEvts
               << " proto/event with CaloHypo::Hypothesis = " << (*iprototype).first
               << endreq;;
    }
    info() << " -----------------------"<< endreq;
  }
  else{
    info() << " 0 event processed ==> 0 Neutral ProtoParticle produced ! " << endreq; 
  }
  

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
/** helpful technical method to evaluate
 *  the ProtoParticle::CaloTrMatch etimator
 *  @see ProtoParticle
 *  @return the value of the estimator
 */
//=============================================================================
double NeutralProtoPAlg::caloTrMatch
( const LHCb::CaloHypo*   hypo  , const LHCb::Calo2Track::IClusTrTable* table )  const
{


  // select the first Ecal cluster
  const LHCb::CaloHypo::Clusters& clusters = hypo->clusters();
  LHCb::CaloHypo::Clusters::const_iterator cluster =
    std::find_if( clusters.begin() , clusters.end() , m_calo );
  if ( clusters.end() == cluster ) { return m_caloTrMatch_bad; }

  // get to all related tracks
  const LHCb::Calo2Track::IClusTrTable::Range range = table->relations( *cluster ) ;
  if ( range.empty()            )  { return m_caloTrMatch_bad; }

  // get minimal value
  return range.front().weight();
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate
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
  // This estiimator could be generalized to Single Photon hypothesis (need to adapt CaloReco)

  if( LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ) {
    LHCb::CaloMomentum momentum(hypo);
    return momentum.mass() ;
  }
  return m_clusterMass_bad ;
};

// ============================================================================
/** helpful technical method to evaluate
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
};
// ============================================================================


// ============================================================================
/** helpful technical method to evaluate
 *  the ProtoParticle::CaloDepositID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo
 *  @return the value of the estimator
 */
// ============================================================================
double  NeutralProtoPAlg::caloDepositID ( const LHCb::CaloHypo*  hypo  )  const
{

  if( hypo->hypothesis() != LHCb::CaloHypo::Pi0Merged )
  { return m_spdprs -> likelihood( hypo ) ; }

  return  m_caloDepositID_bad ;
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate
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
      hypo->hypothesis() == LHCb::CaloHypo::BremmstrahlungPhoton  ){ return m_photonID -> likelihood( hypo ) ; }
  return m_photonID_bad;
};
// ============================================================================
double NeutralProtoPAlg::CaloSpd  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isSpd( DeCalorimeterLocation::Spd );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isSpd );
  return  ( it == digits.begin() ) ? 0. : +1.;
};
// ============================================================================
double NeutralProtoPAlg::CaloPrs  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0;
  LHCb::CaloHypo::Digits digits = hypo->digits();
  LHCb::CaloDataFunctor::IsFromCalo< LHCb::CaloDigit* > isPrs( DeCalorimeterLocation::Prs );
  LHCb::CaloHypo::Digits::iterator it = std::stable_partition ( digits.begin(),digits.end(),isPrs );
  double CaloPrs = 0. ;
  for(LHCb::CaloHypo::Digits::iterator id = digits.begin(); id != it ; ++id){
    if(0 != *id)CaloPrs += (*id)->e();
  }
  return CaloPrs  ;
};
// ============================================================================
double NeutralProtoPAlg::CaloEcal  ( const LHCb::CaloHypo*  hypo  )  const
{
  //
  if( 0 == hypo) return 0.;
  SmartRefVector<LHCb::CaloCluster> clusters = hypo->clusters();
  if( 0 == clusters.size())return 0.;
  SmartRefVector<LHCb::CaloCluster>::iterator icluster = clusters.begin();
  LHCb::CaloCluster* cluster = *icluster;
  if(NULL == cluster) return 0;
  return cluster->e();
};
