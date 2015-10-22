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
// ============================================================================
// CaloUtils
// ============================================================================
#include  "CaloUtils/CaloMomentum.h"
#include  "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// local
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class CaloHypoMonitor CaloHypoMonitor.cpp
 *
 *  The algorithm for trivial monitoring of "CaloHypo" container
 *  The algorithm produces 10 histograms:
 *  <ol>
 *  <li> @p CaloHypo multiplicity                           </li>
 *  <li> @p CaloHypo energy distribution                    </li>
 *  <li> @p CaloHypo transverse momentum distribution       </li>
 *  <li> @p CaloHypo mass distribution                      </li>
 *  <li> @p CaloHypo x distribution                         </li>
 *  <li> @p CaloHypo y distribution                         </li>
 *  <li> multiplicity of     @p CaloCluster per @p CaloHypo </li>
 *  <li> multiplicity of Spd @p CaloDigit   per @p CaloHypo </li>
 *  <li> multiplicity of Prs @p CaloDigit   per @p CaloHypo </li>
 *  <li> CaloHypo x vs y distribution                       </li>
 *  </ol>
 *  Histograms reside in the directory @p /stat/"Name" , where
 *  @ "Name" is the name of the algorithm
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */
class CaloHypoMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoMonitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize(){ 
    StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1(  "1", "# of Hypos    " + inputData(),  m_multMin   ,    m_multMax   , m_multBin  );
    hBook1(  "2", "Hypo Energy   " + inputData(),  m_energyMin ,    m_energyMax , m_energyBin );
    hBook1(  "3", "Hypo Pt       " + inputData(),  m_etMin     ,    m_etMax     , m_etBin);
    if( inputData() == "Rec/Calo/MergedPi0s" || inputData() == "Hlt/Calo/MergedPi0s" )
      hBook1(  "4", "Hypo Mass     " + inputData(),  m_massMin   ,    m_massMax   , m_massBin );
    hBook1(  "5", "Hypo X        " + inputData(),  m_xMin      ,    m_xMax      , m_xBin    );
    hBook1(  "6", "Hypo Y        " + inputData(),  m_yMin      ,    m_yMax      , m_yBin );
    hBook1(  "7", "Clusters/Hypo " + inputData(),  m_clusMin, m_clusMax, m_clusBin );
    hBook1(  "8", "Spd/Hypo      " + inputData(),  m_spdMin,  m_spdMax , m_spdBin  );
    hBook1(  "9", "Prs/Hypo      " + inputData(),  m_prsMin,  m_prsMax , m_prsBin  );
    hBook2( "10", "Hypo barycenter position x vs y   " + inputData(),  m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
    hBook2( "11", "Energy-weighted hypo barycenter position x vs y " + inputData(),m_xMin,m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
    hBook1( "14", "#Hypo/#Cluster" + inputData(), 0., 1.,100);
    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloHypoMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){
    declareProperty("NClusterMin" ,m_clusMin = 0.);
    declareProperty("NClusterMax" ,m_clusMax = 5.);
    declareProperty("NClusterBin" ,m_clusBin = 5);
    declareProperty("NSpdMin" ,m_spdMin = 0.);
    declareProperty("NSpdMax" ,m_spdMax = 10.);
    declareProperty("NSpdBin" ,m_spdBin = 10);
    declareProperty("NPrsMin" ,m_prsMin = 0.);
    declareProperty("NPrsMax" ,m_prsMax = 10.);
    declareProperty("NPrsBin" ,m_prsBin = 10);

    m_clLoc =  LHCb::CaloAlgUtils::CaloClusterLocation( name, context() ) ;
    m_multMax = 250;
    m_multBin =  50;
    setInputData( LHCb::CaloAlgUtils::CaloHypoLocation( name , context() ) );
  }
  /// destructor (virtual and protected)
  virtual ~CaloHypoMonitor() {}
private:
  /// default  construstor  is  private
  CaloHypoMonitor();
  /// copy     construstor  is  private
  CaloHypoMonitor( const CaloHypoMonitor& );
  /// assignement operator  is  private
  CaloHypoMonitor &operator=( const CaloHypoMonitor& );
private:

  int m_clusBin;
  double m_clusMax;
  double m_clusMin;
  int m_spdBin;
  double m_spdMax;
  double m_spdMin;
  int m_prsBin;
  double m_prsMax;
  double m_prsMin;
  std::string m_clLoc;
  
};
DECLARE_ALGORITHM_FACTORY( CaloHypoMonitor )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloHypoMonitor::execute(){

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
  // get functor
  LHCb::CaloDataFunctor::DigitFromCalo spd( DeCalorimeterLocation::Spd );
  LHCb::CaloDataFunctor::DigitFromCalo prs( DeCalorimeterLocation::Prs );

  initCounters();
  for( Hypos::const_iterator hypo = hypos->begin(); hypos->end () != hypo ; ++hypo ){ 

    if ( 0 == *hypo ) continue;
    LHCb::CaloMomentum momentum( *hypo );
    const double e = momentum.e();
    const double et= momentum.pt();

    const double mass=momentum.momentum().mass();
    if(e    < m_eFilter)continue;
    if(et   < m_etFilter)continue;
    if(mass < m_massFilterMin || mass > m_massFilterMax)continue;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    if ( (*hypo)->clusters().size() > 0 ){
      SmartRef<LHCb::CaloCluster> cluster= *((*hypo)->clusters().begin());
      if( 0 != cluster)id = (*cluster).seed();      
    }


    count(id);
    hFill1(id, "2", e  );
    hFill1(id, "3", et );
    if( inputData() == "Rec/Calo/MergedPi0s" || inputData() == "Hlt/Calo/MergedPi0s" )hFill1(id, "4", mass );
    const LHCb::CaloHypo::Position *position = (*hypo)->position();
    if ( 0 != position ){
      hFill1(id, "5", position->x() );
      hFill1(id, "6", position->y() );
    }
    hFill1(id, "7", (*hypo)->clusters().size() );
    const LHCb::CaloHypo::Digits& digits = (*hypo)->digits();
    hFill1(id, "8",  std::count_if( digits.begin(), digits.end(), spd ) );
    hFill1(id, "9", std::count_if( digits.begin(), digits.end(), prs )  );
    if( NULL != position){
      hFill2(id, "10", position->x(),position->y() );
      hFill2(id, "11", position->x(),position->y() , e);
    }
    if( !(id == LHCb::CaloCellID()) ){
      if(doHisto("12"))fillCalo2D("12", id , 1. ,  "Hypo position 2Dview " + inputData() );
      if(doHisto("13"))fillCalo2D("13", id , e  ,  "Hypo Energy 2Dview " + inputData() );
    }
    
  }
  // fill multiplicity histogram
  fillCounters("1");


  //cluster fraction (no area-splittable so far)
  int nClus=0;
  if(exist<LHCb::CaloClusters>(m_clLoc)){
    LHCb::CaloClusters* clusters = get<LHCb::CaloClusters> ( m_clLoc );
    if( NULL != clusters )nClus = clusters->size();
  }
  double frac = 0;
  if( 0 < nClus )frac = (double) m_count / (double) nClus;
  hFill1("14",frac);
  
  return StatusCode::SUCCESS;
}
