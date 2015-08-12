// Include files 

 // from Gaudi
#include "CaloUtils/CaloAlgUtils.h"
// local
#include "CaloShowerOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloShowerOverlap
//
// 2014-06-02 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloShowerOverlap )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloShowerOverlap::CaloShowerOverlap( const std::string& name,
                                      ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
  , m_input( LHCb::CaloClusterLocation::Ecal )
  , m_taggerP            () 
  , m_det (DeCalorimeterLocation::Ecal){
  declareProperty("DistanceThreshold"      , m_dMin        = 4);
  declareProperty("MinEtThreshold"         , m_etMin = 50.  ); // ( ET1 > x && ET2 > x)
  declareProperty("MaxEtThreshold"         , m_etMin2 = 150. ); // ( ET2 > y || ET2 > y)
  declareProperty("Iterations"             , m_iter = 5);
  declareProperty("InputData"              , m_input= LHCb::CaloClusterLocation::Ecal) ;  
  declareProperty("Detector"               , m_det) ;
  // following properties are inherited by the selector tool when defined :
  declareProperty( "PositionTags" , m_taggerP      ) ;

  m_input = LHCb::CaloAlgUtils::CaloClusterLocation( name , context() );
  m_det   = LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
}
//=============================================================================
// Destructor
//=============================================================================
CaloShowerOverlap::~CaloShowerOverlap() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloShowerOverlap::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_oTool   = tool<ICaloShowerOverlapTool>("CaloShowerOverlapTool","PhotonShowerOverlap",this);
  m_tagger  = tool<SubClusterSelectorTool>( "SubClusterSelectorTool" , "EcalClusterTag" , this );

  m_detector  = getDet<DeCalorimeter> ( m_det );
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloShowerOverlap::execute() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG) ) )debug() << "==> Execute" << endmsg;
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,const DeCalorimeter*> eT(m_detector);

  // locate data
  LHCb::CaloClusters* clusters = get<LHCb::CaloClusters>( m_input );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }

  LHCb::CaloDigits* spds = getIfExists<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Spd );

  for( LHCb::CaloClusters::iterator i1 = clusters->begin() ; clusters->end() != i1 ; ++i1 ){
    double et1 = eT( *i1 );
    if( et1 < m_etMin )continue; // neglect overlap from/to low ET clusters
    const LHCb::CaloCellID id1=(*i1)->seed();     
    const LHCb::CaloDigit* spd1 = (spds == NULL) ? NULL : spds->object( (*i1)->seed() );
    for( LHCb::CaloClusters::iterator i2 = i1+1 ; clusters->end() != i2 ; ++i2 ){
      double et2=eT( *i2 );
      if(  et2 < m_etMin )continue; // neglect overlap from/to low ET clusters
      if(  et1 < m_etMin2 && et2 < m_etMin2 )continue; // require at least one cluster above threshold (speed-up)
      const LHCb::CaloCellID id2=(*i2)->seed();    
      if( id1.area() != id2.area() ) continue;
      if( abs( int(id1.col()) - int(id2.col()) ) > m_dMin || abs( int(id1.row()) - int(id2.row()) ) > m_dMin )continue; 
      const LHCb::CaloDigit* spd2 = (spds == NULL) ? NULL : spds->object( (*i2)->seed() );
      int  s1= (spd1 == NULL) ? 0 : int(spd1->e() > 0.) ;
      int  s2= (spd2 == NULL) ? 0 : int(spd2->e() > 0.) ;


      // initial weights for shared cells
      LHCb::CaloCluster* cl1 = *i1;
      LHCb::CaloCluster* cl2 = *i2;
      for( LHCb::CaloCluster::Entries::iterator e1 = cl1->entries().begin() ; cl1->entries().end() != e1 ; ++e1 ){
        for( LHCb::CaloCluster::Entries::iterator e2 = cl2->entries().begin() ; cl2->entries().end() != e2 ; ++e2 ){
          if( e1->digit()->cellID() == e2->digit()->cellID() ){
            e1->setFraction( cl1->e() / (cl1->e() + cl2->e() ));
            e2->setFraction( cl2->e() / (cl1->e() + cl2->e() ));            
          }    
        }
      }
      // tag the cluster position to have correct corrections
      StatusCode sc;
      sc = m_tagger->tagPosition(  cl1  ) ;
      sc = m_tagger->tagPosition(  cl2  ) ;
      if( sc.isFailure() )Warning("Cluster tagging failed - keep the initial 3x3 tagging").ignore();
      // correct entry weight for shower overlap (assuming EM cluster)
      m_oTool->process(*i1,*i2, s1*10+s2, m_iter);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloShowerOverlap::finalize() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG) )) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
