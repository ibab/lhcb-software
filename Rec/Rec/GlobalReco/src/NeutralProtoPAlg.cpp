// $Id: NeutralProtoPAlg.cpp,v 1.2 2006-06-15 10:05:22 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// from kernel
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Vector4DTypes.h"
// from CaloUtils
#include "CaloUtils/Calo2Track.h"
// local
#include "NeutralProtoPAlg.h"



/** @class NeutralProtoPAlg NeutralProtoPAlg.cpp
 *  
 *  Creator of the neutral ProtoParticles from CaloHypos 
 *
 *  The current version fill following estimators for ProtoParticle
 *  
 *  <ul>
 *  <li>  <i>CaloTrMatch</i>     as <b>minimal</b> of this estimator for all 
 *        linked <i>CaloHypo</i> objects. The value is extracted from 
 *        the relation table/associator as a relation weigth between 
 *        <i>CaloCluster</i> and <i>TrStoredTrack</i> objects </li>
 *  <li>  <i>CaloDepositID</i>   as <b>maximal</b> of this estimator for all 
 *        linked <i>CaloHypo</i> objects using Spd/Prs estimator tool 
 *        written by Frederic Machefert </li>
 *  <li>  <i>CaloShowerShape</i> as <b>maximal</b> of the estimator for 
 *        all linked <i>CaloHypo</i> objects. Estimator is equal to the 
 *        sum of diagonal elements of cluster spread matrix (2nd order 
 *        moments of the cluster) </li>
 *  <li>  <i>ClusterMass</i>     as <b>maximal</b> of the estimator of 
 *        cluster mass using smart algorithm by Olivier Deschamp </li>
 *  <li>  <i>PhotonID</i>        as the estimator of PhotonID
 *        using nice identifiaction tool 
 *        CaloPhotonEstimatorTool by Frederic Machefert * 
 *  </ul>
 *  
 *   
 *  @author Olivier Deschamps
 *  @date   2006-06-09
 *  Adapted from NeutralPPsFromCPsAlg class (Vanya Belyaev Ivan.Belyaev@itep.ru)
 */

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
{
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::Photons      );
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::MergedPi0s   );
  m_hyposLocations.push_back( LHCb::CaloHypoLocation::SplitPhotons );
  
  // declare the properties 
  declareProperty( "HyposLocations" , m_hyposLocations   ) ;
  declareProperty( "MatchLocation"  , m_matchLocation    ) ;
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
StatusCode NeutralProtoPAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;



  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ; 
        m_hyposLocations.end() != location ; ++location ) 
  {
    info() << " Hypothesis loaded from " << *location << endreq;
  };
  
  // locate tools
  m_spdprs   = tool< ICaloHypoLikelihood>( m_spdprsType   ,  m_spdprsName ) ;
  if( 0 == m_spdprs ) { return StatusCode::FAILURE ; }
  
  m_photonID = tool< ICaloHypoLikelihood>( m_photonIDType ,  m_photonIDName ) ;
  if( 0 == m_photonID ) { return StatusCode::FAILURE ; }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode NeutralProtoPAlg::execute() {

  debug() << "==> Execute" << endmsg;

  
  /// create and register the output container 
  LHCb::ProtoParticles* protos = new LHCb::ProtoParticles();
  put( protos , m_protoLocation ) ;
    
  /// get the relation table 

  if ( !exist<LHCb::Calo2Track::IClusTrTable>( m_matchLocation ))  {
    warning() << "No matching table at " << m_matchLocation<< endreq ;
    return StatusCode::SUCCESS;
  }
  const LHCb::Calo2Track::IClusTrTable* table = get<LHCb::Calo2Track::IClusTrTable> ( m_matchLocation ) ;
  if ( 0 == table     ) { return Error("Table* points to NULL!");} 
  /// loop over all caloHypos and create the protoparticles  
  for ( std::vector<std::string>::const_iterator location = m_hyposLocations.begin() ; 
        m_hyposLocations.end() != location ; ++location ) {

    // Load the CaloHypo objects if the container exists
    if ( !exist<LHCb::CaloHypos>( *location ))  {
      warning() << "No CaloHypo at " << *location  << endreq ;
      continue;
    }
    const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( *location );
    debug() << "CaloHypo loaded at " << *location  << " (# " << hypos->size()<<")"<< endreq ;
     
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

      /* DEBUG 
      debug() << "Neutral ProtoParticle created " << (*(proto->calo().begin() ))-> hypothesis() << endreq;
      debug() << "Estimator Chi2    =" << proto -> info(LHCb::ProtoParticle::CaloTrMatch ,-1.) << endreq;
      debug() << "Estimator Deposit = " << proto -> info(LHCb::ProtoParticle::CaloDepositID ,-1.) << endreq;
      debug() << "Estimator ShShape = " << proto -> info(LHCb::ProtoParticle::ShowerShape,-1.) << endreq;
      debug() << "Estimator ClMass  = " << proto -> info(LHCb::ProtoParticle::ClusterMass,-1.) << endreq;
      debug() << "Estimator Ph ID   = " << proto -> info(LHCb::ProtoParticle::PhotonID    ,-1.) << endreq;
      */

    } // loop over CaloHypos
  } // loop over HyposLocations
  
  debug() << "# Neutral ProtoParticles created : " << protos -> size() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode NeutralProtoPAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

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

  // reset the value 
  double chi2 = m_caloTrMatch_bad ;

  // select the first Ecal cluster 
  const LHCb::CaloHypo::Clusters& clusters = hypo->clusters();
  LHCb::CaloHypo::Clusters::const_iterator cluster =
    std::find_if( clusters.begin() , clusters.end() , m_calo );
  if ( clusters.end() == cluster ) { return chi2 ; } 
  
  // get to all related tracks
  const LHCb::Calo2Track::IClusTrTable::Range range = table->relations( *cluster ) ;
  if ( range.empty()            )  { return chi2 ; }  
  
  // get minimal value 
  const double match = range.front().weight();
  return match ;
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

  // reset the value 
  double mass = m_clusterMass_bad ;
  // CaloMomentum no longer available in the new Event Model
  // reconstruct clusterMass of pi0 from the SmartRef'ed splitPhotons
  // This estiimator could be generalized to Single Photon hypothesis (need to adapt CaloReco)
 
 if( LHCb::CaloHypotheses::Pi0Merged == hypo->hypothesis() ) {
    const LHCb::CaloHypo* g1 = *(hypo->hypos().begin());
    const LHCb::CaloHypo* g2 = *(hypo->hypos().begin()+1);
    Gaudi::XYZVector p1 = Gaudi::XYZVector(g1->position()->x(), 
                                           g1->position()->y(), 
                                           g1->position()->z());
    Gaudi::XYZVector p2 = Gaudi::XYZVector(g2->position()->x(), 
                                           g2->position()->y(), 
                                           g2->position()->z());
    double e1=g1->position()->e();
    double e2=g2->position()->e();
    Gaudi::LorentzVector pi0 = Gaudi::LorentzVector(e1+e2, 
                                                    e1*p1.X()/p1.R()+e2*p2.X()/p2.R(),
                                                    e1*p1.Y()/p1.R()+e2*p2.Y()/p2.R(),
                                                    e1*p1.Z()/p1.R()+e2*p2.Z()/p2.R());
    return pi0.M() ;
  }  
  return mass ;
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
  // reset the value 
  double shape = m_showerShape_bad ;
   
  // get the position 
  const LHCb::CaloPosition* position = hypo->position();
  if( 0 == position ) {
    if( hypo->hypothesis() != LHCb::CaloHypotheses::Pi0Merged ) 
    { Warning("showerShape(): CaloPosition* points to NULL!"); }
    return shape;     
  }
  shape = position->spread()( 0 , 0 )+ position->spread()( 1 , 1 ) ;
  return shape ;
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
  // reset the value 
  double dep = m_caloDepositID_bad ;
  
  if( hypo->hypothesis() != LHCb::CaloHypotheses::Pi0Merged ) 
  { dep = m_spdprs -> likelihood( hypo ) ; }
  
  return dep ;
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
  double phID = m_photonID_bad ;
  
  if( hypo->hypothesis() == LHCb::CaloHypotheses::Photon               || 
      hypo->hypothesis() == LHCb::CaloHypotheses::PhotonFromMergedPi0  || 
      hypo->hypothesis() == LHCb::CaloHypotheses::BremmstrahlungPhoton || 
      hypo->hypothesis() == LHCb::CaloHypotheses::BremmstrahlungPhoton  ) 
  { phID = m_photonID -> likelihood( hypo ) ; }
  
  return phID  ;
};
