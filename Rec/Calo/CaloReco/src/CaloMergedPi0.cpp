// ============================================================================
// ============================================================================
#include <numeric>
#include <algorithm>
#include <cmath>
#include "Event/CaloDataFunctor.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CellID.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Kernel/CaloCellID.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CaloMergedPi0.h"
#include "CaloUtils/CaloMomentum.h"
// ============================================================================
/** @file CaloMergedPi0.cpp
 *
 *  Implementation file for class : CaloMergedPi0
 *
 *  @author Olivier Deschamps
 *  @date 05/05/2014
 *
 *  New implementation of CaloMergedPi0 algorithm
 *
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloMergedPi0 )

  CaloMergedPi0::CaloMergedPi0( const std::string& name    ,
                                      ISvcLocator*       svcloc  ): GaudiAlgorithm ( name , svcloc )
  , m_clusters      (LHCb::CaloClusterLocation::Ecal            )
  , m_mergedPi0s    (LHCb::CaloHypoLocation::MergedPi0s         )
  , m_splitPhotons  ( LHCb::CaloHypoLocation::    SplitPhotons  )
  , m_splitClusters ( LHCb::CaloClusterLocation:: EcalSplit     )
  , m_etCut         ( 1500 * Gaudi::Units::MeV                  )
  , m_iter          ( 25                                        )
  , m_photonTools   ()
  , m_pi0Tools      ()
  , m_oTool (NULL)
  , m_cov   (NULL)
  , m_spread(NULL)
  , m_taggerE()
  , m_taggerP()
  , m_det (DeCalorimeterLocation::Ecal)
  , m_covParams() {

  declareProperty ( "InputData"               , m_clusters    ) ;
  declareProperty ( "MergedPi0s"              , m_mergedPi0s   ) ;
  declareProperty ( "SplitPhotons"            , m_splitPhotons  ) ;
  declareProperty ( "SplitClusters"           , m_splitClusters ) ;
  declareProperty ( "PhotonTools"             , m_photonTools       ) ;
  declareProperty ( "Pi0Tools"                , m_pi0Tools       ) ;
  declareProperty ( "EtCut"                   , m_etCut  ) ;
  declareProperty ( "MaxIterations"           , m_iter ) ;
  declareProperty ( "CreateSplitClustersOnly" , m_createClusterOnly = false) ;
  declareProperty ( "Verbose"                 , m_verbose  = false );
  declareProperty ( "SplitPhotonMinET"        , m_minET=0.);
  declareProperty ( "Detector"                , m_det) ;

  // following properties are inherited by the selector tool when defined:
  declareProperty( "EnergyTags"   , m_taggerE      ) ;
  declareProperty( "PositionTags" , m_taggerP      ) ;

  // following properties are be inherited by the covariance tool
  declareProperty( "CovarianceParameters" , m_covParams    ) ; // KEEP IT UNSET ! INITIAL VALUE WOULD BYPASS DB ACCESS

  // default context-dependent locations
  m_clusters  = LHCb::CaloAlgUtils::CaloClusterLocation ( "Ecal"     , context()    );  // input : neutral CaloCluster's
  m_mergedPi0s = LHCb::CaloAlgUtils::CaloHypoLocation   ("MergedPi0s" , context()   );  // output : mergedPi0 CaloHypo's
  m_splitPhotons  = LHCb::CaloAlgUtils::CaloHypoLocation("SplitPhotons", context() );  // output : splitPhoton CaloHypo's
  m_splitClusters = LHCb::CaloAlgUtils::CaloSplitClusterLocation(context()         );  // output : splitCluster CaloClusters
}

bool CaloMergedPi0::isNeighbor(LHCb::CaloCellID id0 , LHCb::CaloCellID id1){
  if( id0 == LHCb::CaloCellID() || id1 == LHCb::CaloCellID() )return false;
  if( abs(int(id0.row()) - int(id1.row()) ) > 1 )return false;
  if( abs(int(id0.col()) - int(id1.col()) ) > 1 )return false;
  return true;
}

// ============================================================================
// destructor
// ============================================================================
CaloMergedPi0::~CaloMergedPi0() {}

StatusCode CaloMergedPi0::initialize(){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< "==> Initialise" << endmsg;
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() )return Error("Could not initialize the base class!",sc);

  // Always skip negative-energy clusters :
  if(m_minET<0.)m_minET=0.;

  if(m_createClusterOnly)info() << "Producing SplitClusters only" << endmsg;

  // get detectorElement
  m_detector = getDet<DeCalorimeter>( m_det ) ;
  if( !m_detector ){ return Error("could not locate calorimeter '"+m_det+"'");}

  //==== get tools

  // - main tool :
  m_oTool=tool<ICaloShowerOverlapTool>("CaloShowerOverlapTool","SplitPhotonShowerOverlap",this);

  // - cluster  tools
  m_cov     = tool<ICaloClusterTool>      ( "ClusterCovarianceMatrixTool" , "EcalCovariance"     , this ) ;
  m_spread  = tool<ICaloClusterTool>      ( "ClusterSpreadTool"           , "EcalSpread"         , this ) ;
  m_tagger  = tool<SubClusterSelectorTool>( "SubClusterSelectorTool"      , "EcalClusterTag"     , this );

  // - hypo tools
  for ( std::vector<std::string>::const_iterator it = m_photonTools.begin() ;m_photonTools.end() != it ; ++it ){
    ICaloHypoTool* t = tool<ICaloHypoTool>( *it , this );
    if( 0 == t ) { return StatusCode::FAILURE ; }
    m_gTools.push_back( t ) ;
  }

  for ( std::vector<std::string>::const_iterator it = m_pi0Tools.begin() ;m_pi0Tools.end() != it ; ++it ){
    ICaloHypoTool* t = tool<ICaloHypoTool>( *it , this );
    if( 0 == t ) { return StatusCode::FAILURE ; }
    m_pTools.push_back( t ) ;
  }

  return sc;
}

StatusCode CaloMergedPi0::finalize(){
  m_pTools.clear() ;
  m_gTools.clear() ;
  return GaudiAlgorithm::finalize();
}

StatusCode CaloMergedPi0::execute(){

  using namespace  LHCb::CaloDataFunctor;
  typedef LHCb::CaloClusters Clusters;
  typedef Clusters::iterator Iterator;

  //======== load input :
  Clusters* clusters = getIfExists<Clusters>( m_clusters );
  if( NULL == clusters )return Warning("No cluster input container : no merged Pi0 !", StatusCode::SUCCESS);


  //======== create cluster output :
  //- split clusters (check it does not exist first)
  LHCb::CaloClusters* splitclusters = nullptr;
  if (exist<LHCb::CaloClusters>(m_splitClusters)) {
    Warning("Existing SplitCluster container at "+ m_splitClusters + " found -> will replace",StatusCode::SUCCESS,1).ignore();
    splitclusters=get<LHCb::CaloClusters>( m_splitClusters );
    splitclusters->clear();
  } else {
    splitclusters = new LHCb::CaloClusters();
    put(splitclusters, m_splitClusters );
  }

  //- pi0s & SPlitPhotons
  LHCb::CaloHypos* pi0s = new LHCb::CaloHypos();
  LHCb::CaloHypos* phots = new LHCb::CaloHypos();

  // put on TES when requested
  if(!m_createClusterOnly){
    put( pi0s , m_mergedPi0s );
    put( phots , m_splitPhotons);
  }


  //- load SPD container :
  LHCb::CaloDigits* spds = getIfExists<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Spd );

  // - setup the estimator of cluster transverse energy
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,const DeCalorimeter*> eT ( m_detector ) ;

    // define entry status'

  LHCb::CaloDigitStatus::Status used   =
    LHCb::CaloDigitStatus::UseForEnergy  |
    LHCb::CaloDigitStatus::UseForPosition |
    LHCb::CaloDigitStatus::UseForCovariance  ;
  LHCb::CaloDigitStatus::Status seed   = LHCb::CaloDigitStatus::SeedCell |
    LHCb::CaloDigitStatus::LocalMaximum | used ;

  // ============ loop over all clusters ==============
  for( Iterator icluster = clusters->begin() ; clusters->end() != icluster ; ++icluster ){
    LHCb::CaloCluster* cluster = *icluster ;
    if( 0 == cluster )                { continue ; }
    if ( 0 < m_etCut &&  m_etCut > eT( cluster ) ) { continue ; }

    // -- remove small clusters :
    if( 1 >=  cluster->entries().size() )continue;

    // -- locate cluster Seed
    const LHCb::CaloCluster::Digits::iterator iSeed =
      clusterLocateDigit( cluster->entries().begin() , cluster->entries().end  () , LHCb::CaloDigitStatus::SeedCell     );
    LHCb::CaloDigit* dig1 = iSeed->digit() ;
    if( 0 == dig1) continue ;
    LHCb::CaloCellID  seed1 = dig1->cellID() ;

    double seede  = dig1->e();

    // -- get spd hit in front of seed1
    int spd1 = 0 ;
    const LHCb::CaloDigit* spddigit1 = (spds == NULL) ? NULL : spds->object( dig1->key() );
    if( NULL != spddigit1 )spd1 = (spddigit1->e() > 0.) ? 1 : 0 ;

    // -- locate seed2
    double sube   = 0. ; // 2nd seed should must have a positive energy !
    LHCb::CaloDigit*  dig2  = NULL;
    for( LHCb::CaloCluster::Digits::iterator it =cluster->entries().begin() ; cluster->entries().end() != it ; ++it ){
      LHCb::CaloDigit* dig = it->digit() ;
      if( 0 == dig) { continue ; }
      LHCb::CaloCellID seed  = dig->cellID() ;
      double ecel = dig->e()*it->fraction();
      if (ecel > sube && ecel < seede && isNeighbor( seed1, seed) && !(seed==seed1)){
        //if (ecel > sube && ecel < seede && isNeighbor( seed1, seed) ){
        sube=ecel;
        dig2=dig;
      }
    }

    if ( NULL == dig2 ){
      counter("Cluster without 2nd seed found") += 1;
      continue ;
    }

    LHCb::CaloCellID  seed2 = dig2->cellID() ;
    // -- get spd hit in front of seed2
    int spd2 = 0 ;
    const LHCb::CaloDigit* spddigit2 = (spds == NULL) ? NULL : spds->object( dig2->key() );
    if( NULL != spddigit2 ) { spd2 = (spddigit2->e() > 0.) ? 1 : 0 ; }


    // -- create and fill sub-cluster
    LHCb::CaloCluster* cl1 = new LHCb::CaloCluster();
    cl1->setSeed( seed1 );
    cl1->setType( LHCb::CaloCluster::Area3x3);
    LHCb::CaloCluster* cl2 = new LHCb::CaloCluster();
    cl2->setSeed( seed2 );
    cl2->setType( LHCb::CaloCluster::Area3x3);

    for( LHCb::CaloCluster::Digits::const_iterator it2 =cluster->entries().begin() ; cluster->entries().end() != it2 ; ++it2 ){
      const LHCb::CaloDigit* dig = it2->digit() ;
      if( 0 == dig)continue ;
      const LHCb::CaloCellID  id = dig->cellID() ;
      double fraction = it2->fraction();

      // -- tag 3x3 area for energy and position
      if ( isNeighbor( seed1, id ) ){
        LHCb::CaloDigitStatus::Status status = ( seed1 == id ) ? seed : used;
        // set initial weights
        double weight1 = fraction;
        if( seed2 == id )weight1=0.;
        else if( seed1 == id )weight1=fraction;
        else if (isNeighbor(seed2,id))weight1=dig1->e() / (dig1->e() + dig2->e())*fraction;
        cl1->entries().push_back(LHCb::CaloClusterEntry( dig , status,weight1 ));
      }
      if ( isNeighbor( seed2, id ) ){
        LHCb::CaloDigitStatus::Status status = ( seed2 == id ) ? seed : used;
        //set initial weights
        double weight2 = fraction;
        if( seed1 == id )weight2=0.;
        else if(seed2 == id)weight2=fraction;
        else if (isNeighbor(seed1,id))weight2=dig2->e() / (dig1->e() + dig2->e())*fraction;
        cl2->entries().push_back(LHCb::CaloClusterEntry( dig , status,weight2 ));
      }
    }


    // --  apply position tagger (possibly replacing the 3x3 already set)
    // --  needed to apply hypo S/L-corrections with correct parameters internally
    // --  keep the 3x3 energy tag for the time being (to be applied after the overlap subtraction)
    StatusCode sc;
    sc = m_tagger->tagPosition(  cl1  ) ;
    sc = m_tagger->tagPosition(  cl2  ) ;
    if( sc.isFailure() )Warning("SplitCluster tagging failed - keep the initial 3x3 tagging").ignore();

    // == apply the mergedPi0 tool : subtract shower overlap
    m_oTool->process(cl1,cl2, spd1*10+spd2, m_iter,true); // 'true' means the initial entries weight is propagated
    if( LHCb::CaloMomentum(cl1).pt() <= m_minET || LHCb::CaloMomentum(cl2).pt() <= m_minET ){ // skip negative energy "clusters"
      delete cl1;
      delete cl2;
      continue;
    }

    // == prepare outputs :

    // apply loose mass window : (TODO ??)

    // == APPLY CLUSTER TOOLS : Energy tagger,  covariance & spread (position tagger already applied):
    if  (m_tagger  -> tagEnergy    ( cl1 ).isFailure() )counter("Fails to tag(E) cluster (1)")+=1;
    if  (m_tagger  -> tagEnergy    ( cl2 ).isFailure() )counter("Fails to tag(E) cluster (2)")+=1;
    if  (m_cov   -> process( cl1 ).isFailure() )counter("Fails to set covariance (1)")+=1;
    if  (m_cov   -> process( cl2 ).isFailure() )counter("Fails to set covariance (2)")+=1;
    if  (m_spread-> process( cl1 ).isFailure() )counter("Fails to set spread (1)")    +=1;
    if  (m_spread-> process( cl2 ).isFailure() )counter("Fails to set spread (2)")    +=1;


    // == insert splitClusters into their container
    splitclusters->insert( cl1 ) ;
    splitclusters->insert( cl2 ) ;

    // == create CaloHypos if needed
    if(!m_createClusterOnly){
      // new CaloHypos for splitPhotons
      LHCb::CaloHypo* g1   = new LHCb::CaloHypo() ;
      g1 -> setHypothesis( LHCb::CaloHypo::PhotonFromMergedPi0 ) ;
      g1 -> addToClusters( cluster )                ;
      g1 -> addToClusters( cl1       )                ;
      g1 -> setPosition( new LHCb::CaloPosition( cl1->position()) );

      LHCb::CaloHypo* g2   = new LHCb::CaloHypo() ;
      g2 -> setHypothesis( LHCb::CaloHypo::PhotonFromMergedPi0 ) ;
      g2 -> addToClusters( cluster )                ;
      g2 -> addToClusters( cl2       )                ;
      g2 -> setPosition( new LHCb::CaloPosition( cl2->position() ) );

      // new CaloHypo for mergedPi0
      LHCb::CaloHypo* pi0 = new LHCb::CaloHypo();
      pi0 -> setHypothesis( LHCb::CaloHypo::Pi0Merged ) ;
      pi0 -> addToClusters( cluster );
      pi0 -> addToHypos ( g2 );
      pi0 -> addToHypos ( g1 );

      //--  Apply hypo tools : E/S/L-corrections
      int i = 0;
      for( std::vector<ICaloHypoTool*>::iterator it = m_gTools.begin() ; m_gTools.end() != it ; ++it ){
        i++;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << " apply SplitPhoton tool " << i << "/" << m_gTools.size() << endmsg;
        ICaloHypoTool* t = *it ;
        if( 0 == t )  continue;
        StatusCode sc         = StatusCode::SUCCESS ;
        sc                    = (*t) ( g1 ) ;
        if( sc.isFailure() ) Error("Error from 'Tool' for g1 " , sc ).ignore() ;
        sc                    = (*t) ( g2 ) ;
        if( sc.isFailure() )Error("Error from 'Tool' for g2 " , sc ).ignore() ;
      }

      i = 0;
      for( std::vector<ICaloHypoTool*>::iterator it = m_pTools.begin() ; m_pTools.end() != it ; ++it ){
        i++;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << " apply MergedPi0 tool " << i << "/" << m_pTools.size() << endmsg;
        ICaloHypoTool* t = *it ;
        if( 0 == t ) { continue; }
        StatusCode sc         = StatusCode::SUCCESS ;
        sc                    = (*t) ( pi0 ) ;
        if( sc.isFailure() )Error("Error from 'Tool' for pi0 " , sc ).ignore() ;
      }


      // skip negative energy CaloHypos
      if( LHCb::CaloMomentum(g1).pt() < m_minET || LHCb::CaloMomentum(g2).pt() < m_minET ){
        delete g1;
        delete g2;
        delete pi0;
      } else {
        if( m_verbose )info() << " >> MergedPi0 hypo Mass : "  << LHCb::CaloMomentum(pi0).mass() << endmsg;
        phots ->insert( g1  ) ;
        phots ->insert( g2  ) ;
        pi0s -> insert( pi0 ) ;
      }
    }
  }

  // ====================== //
  counter ( m_clusters + "=>" + m_mergedPi0s )   += pi0s   -> size() ;
  counter ( m_clusters + "=>" + m_splitPhotons)  += phots  -> size() ;
  counter ( m_clusters + "=>" + m_splitClusters) += splitclusters -> size() ;

  // delete (empty) container* if not on TES
  StatusCode sc = StatusCode::SUCCESS;
  if(m_createClusterOnly){
    if( 0 != pi0s->size() || 0 != phots->size() ){
      sc = Error( "Container* to be deleted are not empty", StatusCode::FAILURE);
    }
    delete pi0s;
    delete phots;
  }
  return sc;
}
