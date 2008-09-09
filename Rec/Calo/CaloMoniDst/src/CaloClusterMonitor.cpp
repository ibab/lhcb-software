// $Id: CaloClusterMonitor.cpp,v 1.6 2008-09-09 15:37:24 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// Include files
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
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "# of Clusters    " + inputData(),  m_multMin, m_multMax , m_multBin );
    hBook1( "2", "Cluster Size     " + inputData(),  m_sizeMin, m_sizeMax , m_sizeBin );
    hBook1( "3", "Cluster Energy   " + inputData(),  m_energyMin, m_energyMax ,m_energyBin);
    hBook1( "4", "Cluster Et       " + inputData(),  m_etMin, m_etMax , m_etBin);
    hBook1( "5", "Cluster x "        + inputData(), m_xMin, m_xMax , m_xBin);
    hBook1( "6", "Cluster y "        + inputData(), m_yMin, m_yMax , m_yBin );
    hBook2( "7", "Cluster x vs y   " + inputData(), m_xMin, m_xMax, m_xBin , m_yMin, m_yMax, m_yBin );
    
    return StatusCode::SUCCESS;
  }
  virtual StatusCode finalize();
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){
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

DECLARE_ALGORITHM_FACTORY( CaloClusterMonitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloClusterMonitor::execute()
{ typedef const LHCb::CaloCluster::Container Clusters;


 debug() << " Producing histo " << produceHistos() << endreq;
 // produce histos ?
 if ( !produceHistos() ) return StatusCode::SUCCESS;

// get input data
 if( !exist<Clusters>(inputData() )){
   debug() << "No cluster container found " << endreq;
   return StatusCode::SUCCESS;
 }
 
 Clusters* clusters = get<Clusters> ( inputData() );
 if ( 0 == clusters || clusters->empty() ){
   debug() << "No cluster found in " << inputData() << endreq;
   return StatusCode::SUCCESS;
 }
  
// fill multiplicity histogram
  long count = 0;
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
    count++;
    hFill1( "2", (*cluster)->entries().size() );    
    hFill1( "3", e );
    hFill1( "4", et );
    hFill1( "5", x );
    hFill1( "6", y );
    hFill2( "7", x,y );
  }
  hFill1( "1", count );




  return StatusCode::SUCCESS;
}



StatusCode CaloClusterMonitor::finalize() {
  debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
