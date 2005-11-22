// $Id: CaloClusterMonitor.cpp,v 1.4 2005-11-22 22:43:01 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
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
 *  The algorithm produces 3 histograms: 
 *  
 *  <ol> 
 *  <li> @p CaloCluster multiplicity                    </li>
 *  <li> @p CaloCluster size (number of cells)          </li>
 *  <li> @p CaloCluster transverse energy distribution  </li>
 *  <li> @p CaloCluster x-distribution                  </li>
 *  <li> @p CaloCluster y-distribution                  </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where 
 *  @ "Name" is the name of the algorithm
 * 
 *  @see CaloCluster 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
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
  virtual StatusCode initialize();  
  /// standard algorithm execution 
  virtual StatusCode execute   ();
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloClusterMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg    ( name , svcloc )
    , m_energyMax    ( 250. * GeV ) 
    , m_etMax        (  10. * GeV ) 
    , m_yband        (  32. * cm  ) 
    , m_xband        (  32. * cm  ) 
  {
    declareProperty ( "EnergyMax" , m_energyMax ) ;
    declareProperty ( "EtMax"     , m_etMax     ) ;
    declareProperty ( "yBand"     , m_yband     ) ;
    declareProperty ( "xBand"     , m_xband     ) ;
  };
  /// destructor (virtual and protected)
  virtual ~CaloClusterMonitor(){};
private:
  /// default  construstor  is  private 
  CaloClusterMonitor(); 
  /// copy     construstor  is  private 
  CaloClusterMonitor
  ( const CaloClusterMonitor& );
  /// assignement operator  is  private 
  CaloClusterMonitor& operator=
  ( const CaloClusterMonitor& );
private:  
  double        m_energyMax    ;  
  double        m_etMax        ;  
  double        m_yband        ;
  double        m_xband        ;
};

// ============================================================================
/** @var CaloClusterMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterMonitor>         s_Factory ;
const        IAlgFactory&CaloClusterMonitorFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** standard algorithm initialization
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloClusterMonitor::initialize()
{
  StatusCode sc = CaloMoniAlg::initialize();
  if ( sc.isFailure() ) { return sc ; }
  
  // book the histograms
  // cluster multiplicity
  book ( 1  , "# of Clusters  " + inputData()  ,
         0  , 200 , 200                        ) ; 
  // cluster size
  book ( 2  , "Cluster Size   " + inputData()  ,
         0  , 50                               ) ; 
  // cluster energy
  book ( 3  , "Cluster Energy " + inputData()  ,  
         0. , m_energyMax                      ) ; 
  // cluster transverse energy
  book ( 4  , "Cluster Et     " + inputData()  ,
         0. ,  m_etMax                         ) ;
  // cluster x-position
  book ( 5  , "Cluster x      " + inputData()  ,
         -4 * meter , 4  * meter               ) ;
  // cluster y-position
  book ( 6  , "Cluster y      " + inputData()  ,  
         -4 * meter ,  4  * meter              ) ;
  // cluster x-position
  book ( 7  ,  "Cluster x (band)" + inputData() ,
         -4 * meter , 4  * meter               ) ;
  // cluster y-position
  book ( 8  , "Cluster y  (band)" + inputData()  ,  
         -4 * meter , 4  * meter               ) ; ///< upper edge
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloClusterMonitor::execute  ()
{  
  // avoid long names 
  typedef const   CaloClusters          Clusters ;
  
  // produce histos ?
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  // get input data 
  Clusters* clusters = get<Clusters> ( inputData() ) ;
  
  // fill multiplicity histogram  
  AIDA::IHistogram1D* h1 = histo ( 1 )  ;
  hFill ( h1 , clusters->size() ) ;
  
  if ( 0 == clusters    ) { return StatusCode::FAILURE ; }
  
  AIDA::IHistogram1D* h2 = histo ( 2 )  ;
  AIDA::IHistogram1D* h3 = histo ( 3 )  ;
  AIDA::IHistogram1D* h4 = histo ( 4 )  ;
  AIDA::IHistogram1D* h5 = histo ( 5 )  ;
  AIDA::IHistogram1D* h6 = histo ( 6 )  ;
  AIDA::IHistogram1D* h7 = histo ( 7 )  ;
  AIDA::IHistogram1D* h8 = histo ( 8 )  ;
  
  for( Clusters::const_iterator cluster = clusters -> begin() ;
       clusters -> end () != cluster ; ++cluster ) 
  {
    // skip NULLs 
    if ( 0 == *cluster ) { continue ; }
    
    hFill ( h2 , (*cluster)->entries().size() ) ;
    
    const double e = (*cluster)->position().e() ;
    const double x = (*cluster)->position().x() ;
    const double y = (*cluster)->position().y() ;
    const double z = (*cluster)->position().z() ;
    
    hFill ( h3 , e                          ) ;
    hFill ( h4 , e * sqrt( x*x + y*y )  / z ) ;
    hFill ( h5 , x                          ) ;
    hFill ( h6 , y                          ) ;
    
    if ( fabs( y ) <= m_yband ) { hFill ( h7 , x ) ; }
    if ( fabs( x ) <= m_xband ) { hFill ( h8 , y ) ; }
    
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
