// ============================================================================
// Include files
// ============================================================================
// CaloDet
// ============================================================================
#include  "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event
// ============================================================================
#include  "Event/CaloHypo.h"
#include  "Event/CaloDataFunctor.h"
#include  "Event/L0CaloCandidate.h"
#include  "Event/L0DUBase.h"
#include  "CaloUtils/CaloMomentum.h"
#include  "CaloUtils/CaloAlgUtils.h"


// ============================================================================
// local
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class L0CaloScale L0CaloScale.cpp
 */
class L0CaloScale : public CaloMoniAlg{
  /// friend factory for instantiation
  friend class AlgFactory<L0CaloScale>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize(){ 
    StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1(  "0", "matched L0Calo type " + inputData(),  0   ,    2   , 2  );
    h1binLabel("0",1,"L0Electron");
    h1binLabel("0",2,"L0Photon");    

    hBook1(  "1", "L0Calo(Et)/CaloHypo(Et) " + inputData(),  m_ratMin   ,    m_ratMax   , m_ratBin  );
    hBook1(  "2", "L0Calo(Et)/CaloCluster(Et) " + inputData(),  m_ratMin   ,    m_ratMax   , m_ratBin  );
    hBook1(  "3", "CaloCluster/CaloHypo(Et) " + inputData(),  m_ratMin   ,    m_ratMax   , m_ratBin  );
    m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  L0CaloScale( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_ecal(NULL)
  {
    declareProperty("RatioMin" ,m_ratMin = 0.2);
    declareProperty("RatioMax" ,m_ratMax = 1.7);
    declareProperty("RatioBin" ,m_ratBin = 150);

    m_clLoc =  LHCb::CaloAlgUtils::CaloClusterLocation( name, context() ) ;
    setInputData( LHCb::CaloAlgUtils::CaloHypoLocation( name , context() ) );
  }
  /// destructor (virtual and protected)
  virtual ~L0CaloScale() {}
private:
  /// default  construstor  is  private
  L0CaloScale();
  /// copy     construstor  is  private
  L0CaloScale( const L0CaloScale& );
  /// assignement operator  is  private
  L0CaloScale &operator=( const L0CaloScale& );
private:

  int m_ratBin;
  double m_ratMax;
  double m_ratMin;
  DeCalorimeter* m_ecal;
  std::string m_clLoc;
};
DECLARE_ALGORITHM_FACTORY( L0CaloScale )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode L0CaloScale::execute(){

  typedef const LHCb::CaloHypo::Container Hypos;
  // produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  // get input data
  if( !exist<Hypos> ( inputData() ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no hypo container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS ;
  };

  Hypos *hypos = get<Hypos> ( inputData() );
  // check data
  if ( hypos -> empty() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No hypo found in " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }  


  if ( ! exist< LHCb::L0CaloCandidates >( LHCb::L0CaloCandidateLocation::Full) ) {
    Warning( "No data at " + LHCb::L0CaloCandidateLocation::Full).ignore() ;
    return StatusCode::SUCCESS ;
  }

  LHCb::L0CaloCandidates * candidates = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full);
  if ( NULL == candidates ) return StatusCode::SUCCESS ;



  initCounters();
  for( Hypos::const_iterator hypo = hypos->begin(); hypos->end () != hypo ; ++hypo ){ 
    if ( 0 == *hypo ) continue;
    LHCb::CaloMomentum momentum( *hypo );
    const double et= momentum.pt();

    if(et   < m_etFilter)continue;

    SmartRef<LHCb::CaloCluster> cluster;
  LHCb::CaloCellID id = LHCb::CaloCellID();
    if ( (*hypo)->clusters().size() > 0 ){
      cluster= *((*hypo)->clusters().begin());
      if( 0 != cluster)id = (*cluster).seed();      
    }else{
      continue;
    }
    
    

    // L0Calo matching
    int l0et = 0;
    int type = -1;
    bool found=false;
    for ( LHCb::L0CaloCandidates::iterator  icand = candidates -> begin() ; candidates -> end() != icand ;++icand ) { 
      LHCb::L0CaloCandidate* cand = *icand ;
      LHCb::CaloCellID l0id = cand -> id() ; 
      if( l0id.area() != id.area() || abs((int)l0id.row()-(int)id.row())>1 || abs((int)l0id.col()-(int)id.col())>1)continue;
      type = cand -> type() ;
      if( type != L0DUBase::CaloType::Electron && type != L0DUBase::CaloType::Photon)continue;
      l0et   = cand -> etCode();
      found=true;
      break;
    }
    if( !found ){
      counter("Matching L0cluster not found")+=1;
      continue;
    }
    counter("Matching L0type") += type;
    double hratio = 20.*double(l0et) / et;
    double cratio = 20.*double(l0et) / (cluster->e() * m_ecal->cellSine( id )) ;
    double ratio  = (cluster->e() * m_ecal->cellSine( id )) / et;

    count(id);
    hFill1(id, "0", type  );
    hFill1(id, "1", hratio  );
    hFill1(id, "2", cratio );
    hFill1(id, "3", ratio );

    if(doHisto("4"))fillCalo2D("4", id , hratio ,  "L0Cluster(Et)/CaloHypo(Et) 2Dview" );
    if(doHisto("5"))fillCalo2D("5", id , cratio  ,  "L0Cluster(Et)/CaloCuster(Et) 2Dview" );
    if(doHisto("6"))fillCalo2D("6", id , cratio  ,  "CaloCluster(Et)/CaloHypo(Et) 2Dview" );
    
    
  }
  
  return StatusCode::SUCCESS;
}
