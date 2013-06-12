// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event
// ============================================================================
#include  "Event/CaloCluster.h"
// ============================================================================
// local
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class CaloClusterMonitor CaloClusterMonitor.cpp
 *
 *  The algorithm for trivial monitoring of "CaloCluster" containers.
 *  The algorithm produces 8 histograms:
 *
 *  <ol>
 *  <li> @p CaloCluster multiplicity                    </li>
 *  <li> @p CaloCluster size (number of cells)          </li>
 *  <li> @p CaloCluster energy distribution             </li>
 *  <li> @p CaloCluster transverse energy distribution  </li>
 *  <li> @p CaloCluster x-distribution                  </li>
 *  <li> @p CaloCluster y-distribution                  </li>
 *  <li> @p CaloCluster x vs y-distribution             </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where
 *  @ "Name" is the name of the algorithm
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMonitor>;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize()
  { StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm

    hBook1( "1", "# of Clusters    " + inputData(),  m_multMin, m_multMax , m_multBin );
    hBook1( "2", "Cluster digit multiplicity     " + inputData(),  m_sizeMin, m_sizeMax , m_sizeBin );
    hBook1( "3", "Cluster Energy   " + inputData(),  m_energyMin, m_energyMax ,m_energyBin);
    hBook1( "4", "Cluster Et       " + inputData(),  m_etMin, m_etMax , m_etBin);
    hBook1( "5", "Cluster x "        + inputData(), m_xMin, m_xMax , m_xBin);
    hBook1( "6", "Cluster y "        + inputData(), m_yMin, m_yMax , m_yBin );
    hBook2( "7", "Cluster barycenter position x vs y   " + inputData(), m_xMin, m_xMax, m_xBin , m_yMin, m_yMax, m_yBin );
    hBook2( "8", "Energy-weighted cluster barycenter position x vs y " + inputData(),m_xMin,m_xMax,m_xBin,m_yMin,m_yMax, m_yBin );
    hBook1( "9", "Cluster digit used for Energy multiplicity     " + inputData(),  m_sizeMin, m_sizeMax , m_sizeBin );

    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){

    m_multMax = 200;
    m_multBin = 100;
    setInputData( LHCb::CaloAlgUtils::CaloClusterLocation( name, context() ) );

  }
  /// destructor (virtual and protected)
  virtual ~CaloClusterMonitor() {}
private:
  /// default  construstor  is  private
  CaloClusterMonitor();
  /// copy     construstor  is  private
  CaloClusterMonitor( const CaloClusterMonitor& );
  /// assignement operator  is  private
  CaloClusterMonitor &operator=( const CaloClusterMonitor& );
private:
};

DECLARE_ALGORITHM_FACTORY( CaloClusterMonitor )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloClusterMonitor::execute(){
  typedef const LHCb::CaloCluster::Container Clusters;


  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Producing histo " << produceHistos() << endmsg;
  // produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  
  // get input data
  if( !exist<Clusters>(inputData() )){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No cluster container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  Clusters* clusters = get<Clusters> ( inputData() );
  if ( clusters->empty() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No cluster found in " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  // fill multiplicity histogram
  initCounters();
  for( Clusters::const_iterator cluster = clusters->begin();
       clusters->end() != cluster ; ++cluster ){
    if ( 0 == *cluster ) continue;
    const double e = (*cluster)->position().e();
    const double x = (*cluster)->position().x();
    const double y = (*cluster)->position().y();
    const double z = (*cluster)->position().z();
    const double et = e * sqrt( x*x + y*y ) / sqrt( x*x+y*y+z*z);
    if( e  < m_eFilter)continue;
    if( et < m_etFilter)continue;
    LHCb::CaloCellID id = (*cluster)->seed();
    count( id );
    hFill1(id, "2", (*cluster)->entries().size() );    
    hFill1(id, "3", e  );
    hFill1(id, "4", et );
    hFill1(id, "5", x  );
    hFill1(id, "6", y  );
    hFill2(id, "7", x,y );
    hFill2(id, "8", x,y,e);
    int iuse=0;
    for(std::vector<LHCb::CaloClusterEntry>::iterator ie = (*cluster)->entries().begin();
        ie != (*cluster)->entries().end();ie++){
      if( 0 != (LHCb::CaloDigitStatus::UseForEnergy & (*ie).status()) )iuse++;
    }
    hFill1(id,"9", iuse );
    if(doHisto("10"))fillCalo2D("10",  (*cluster)->seed() , 1.,  "Cluster position 2Dview " + inputData() );
    if(doHisto("11"))fillCalo2D("11",  (*cluster)->seed() , e ,  "Cluster Energy 2Dview " + inputData() );
    
  }
  // fill counter
  fillCounters("1");

  return StatusCode::SUCCESS;
}
