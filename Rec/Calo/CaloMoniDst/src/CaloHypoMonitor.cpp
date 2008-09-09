// $Id: CaloHypoMonitor.cpp,v 1.7 2008-09-09 15:37:24 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
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
    StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1(  "1", "# of Hypos    " + inputData(),  m_multMin   ,    m_multMax   , m_multBin  );
    hBook1(  "2", "Hypo Energy   " + inputData(),  m_energyMin ,    m_energyMax , m_energyBin );
    hBook1(  "3", "Hypo Pt       " + inputData(),  m_etMin     ,    m_etMax     , m_etBin);
    hBook1(  "4", "Hypo Mass     " + inputData(),  m_massMin   ,    m_massMax   , m_massBin );
    hBook1(  "5", "Hypo X        " + inputData(),  m_xMin      ,    m_xMax      , m_xBin    );
    hBook1(  "6", "Hypo Y        " + inputData(),  m_yMin      ,    m_yMax      , m_yBin );
    hBook1(  "7", "Clusters/Hypo " + inputData(),  m_clusMin, m_clusMax, m_clusBin );
    hBook1(  "8", "Spd/Hypo      " + inputData(),  m_spdMin,  m_spdMax , m_spdBin  );
    hBook1(  "9", "Prs/Hypo      " + inputData(),  m_prsMin,  m_prsMax , m_prsBin  );
    hBook2( "10", "Hypo x vs y   " + inputData(),  m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
  virtual StatusCode finalize();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloHypoMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){
    declareProperty("NClusterMin" ,m_clusMin = 0.);
    declareProperty("NClusterMin" ,m_clusMax = 10.);
    declareProperty("NClusterMin" ,m_clusBin = 10);
    declareProperty("NSpdMax" ,m_spdMin = 0.);
    declareProperty("NSpdMax" ,m_spdMax = 20.);
    declareProperty("NSpdMax" ,m_spdBin = 20);
    declareProperty("NPrsBin" ,m_prsMin = 0.);
    declareProperty("NPrsBin" ,m_prsMax = 20.);
    declareProperty("NPrsBin" ,m_prsBin = 20);

    m_multMax = 150;
    
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
  

};
DECLARE_ALGORITHM_FACTORY( CaloHypoMonitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloHypoMonitor::execute()
{ typedef const LHCb::CaloHypo::Container Hypos;

  // produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

  // get input data
  Hypos *hypos = get<Hypos> ( inputData() );
  if ( 0 == hypos ) return StatusCode::SUCCESS;


  if ( hypos -> empty() ) return StatusCode::SUCCESS;

  LHCb::CaloDataFunctor::DigitFromCalo spd( DeCalorimeterLocation::Spd );
  LHCb::CaloDataFunctor::DigitFromCalo prs( DeCalorimeterLocation::Prs );

  long count = 0;
  for( Hypos::const_iterator hypo = hypos->begin();
       hypos->end () != hypo ; ++hypo ){ 
    if ( 0 == *hypo ) continue;

    LHCb::CaloMomentum momentum( *hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    const double mass=momentum.momentum().mass();
    if(e    < m_eFilter)continue;
    if(et   < m_etFilter)continue;
    if(mass < m_massFilterMin || mass > m_massFilterMax)continue;

    count++;
    hFill1( "2", e );
    hFill1( "3", et );
    hFill1( "4", mass );
    

    const LHCb::CaloHypo::Position *position = (*hypo)->position();
    if ( 0 != position ){
      hFill1( "5", position->x() );
      hFill1( "6", position->y() );
    }

    hFill1( "7", (*hypo)->clusters().size() );

    const LHCb::CaloHypo::Digits& digits = (*hypo)->digits();
    hFill1( "8",  std::count_if( digits.begin(), digits.end(), spd ) );
    hFill1( "9", std::count_if( digits.begin(), digits.end(), prs ) );
    if( NULL != position)hFill2( "10", position->x(),position->y());
  }
  // fill multiplicity histogram
  hFill1( "1", count );

  return StatusCode::SUCCESS;
}


StatusCode CaloHypoMonitor::finalize() {
  debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
