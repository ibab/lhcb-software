// $Id: CaloClusterMonitor.cpp,v 1.5 2007-07-25 19:49:12 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
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
    hBook1( "1", "# of Clusters    " + inputData(),  0, 200, 200 );
    hBook1( "2", "Cluster Size     " + inputData(),  0,  50,  50 );
    hBook1( "3", "Cluster Energy   " + inputData(),  0, m_energyMax );
    hBook1( "4", "Cluster Et       " + inputData(),  0, m_etMax );
    hBook1( "5", "Cluster x " + inputData(), -m_xband, m_xband );
    hBook1( "6", "Cluster y " + inputData(), -m_yband, m_yband );
    hBook2( "7", "Cluster x vs y   " + inputData(), -m_xband, m_xband, 100 , -m_yband, m_yband,100 );
    
    return StatusCode::SUCCESS;
  }
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_energyMax    ( 250. * Gaudi::Units::GeV )
    , m_etMax        (  15. * Gaudi::Units::GeV )
    , m_yband        (  4 * Gaudi::Units::meter )
    , m_xband        (  4 * Gaudi::Units::meter )
  { declareProperty( "EnergyMax", m_energyMax );
    declareProperty( "EtMax",     m_etMax );
    declareProperty( "xMax",     m_xband );
    declareProperty( "yMax",     m_yband );
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
  double m_energyMax;
  double m_etMax;
  double m_yband;
  double m_xband;
};

DECLARE_ALGORITHM_FACTORY( CaloClusterMonitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloClusterMonitor::execute()
{ typedef const LHCb::CaloCluster::Container Clusters;

// produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

// get input data
  Clusters* clusters = get<Clusters> ( inputData() );
  if ( 0 == clusters ) return StatusCode::FAILURE;

// fill multiplicity histogram
  hFill1( "1", clusters->size() );

  for( Clusters::const_iterator cluster = clusters->begin();
       clusters->end() != cluster ; ++cluster )
  { if ( 0 == *cluster ) continue;

    hFill1( "2", (*cluster)->entries().size() );
    
    const double e = (*cluster)->position().e();
    const double x = (*cluster)->position().x();
    const double y = (*cluster)->position().y();
    const double z = (*cluster)->position().z();

    hFill1( "3", e );
    hFill1( "4", e * sqrt( x*x + y*y ) / sqrt( x*x+y*y+z*z) );
    if ( fabs( y ) <= m_yband ) hFill1( "5", x );
    if ( fabs( x ) <= m_xband ) hFill1( "6", y );
    if ( fabs( y ) <= m_yband &&  fabs( x ) <= m_xband)hFill2( "7", x,y );
  }
  return StatusCode::SUCCESS;
}
