// $Id: CaloClusterMonitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/02/17 12:00:57  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.3  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
// AIDA 
#include  "AIDA/IHistogram1D.h"
// Event 
#include  "Event/CaloCluster.h"
// CaloUtils 
#include  "CaloUtils/dirHbookName.h"
// local
#include  "CaloClusterMonitor.h"

// ============================================================================
/** @file CaloClusterMonitor.cpp
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
 *  @ "Name" is teh name of the algorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloClusterMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterMonitor>         s_Factory ;
const        IAlgFactory&CaloClusterMonitorFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name
 *  @param   svcloc pointer to service locator
 */
// ============================================================================
CaloClusterMonitor::CaloClusterMonitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc )
  , m_multiplicity (   0        ) 
  , m_size         (   0        )
  , m_energy       (   0        )
  , m_energyMax    ( 250. * GeV ) 
  , m_et           (   0        ) 
  , m_etMax        (  10. * GeV ) 
  , m_x            (   0        )
  , m_x1           (   0        )
  , m_yband        (  32. * cm  ) 
  , m_y            (   0        )
  , m_y1           (   0        )
  , m_xband        (  32. * cm  ) 
{
  declareProperty ( "EnergyMax" , m_energyMax ) ;
  declareProperty ( "EtMax"     , m_etMax     ) ;
  declareProperty ( "yBand"     , m_yband     ) ;
  declareProperty ( "xBand"     , m_xband     ) ;
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloClusterMonitor::~CaloClusterMonitor() {};
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
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}

  // PAWC/RZ directory
  const std::string directory( dirHbookName( "Calo/" + name() ) );

  // book the histograms
  // cluster multiplicity
  m_multiplicity   =
    histoSvc () -> book ( directory                        ,  ///< directory
                          1                                ,  ///< ID
                          "# of Clusters  " + inputData()  ,  ///< title
                          200                              ,  ///< #bins
                          0                                ,  ///< low edge
                          200                              ); ///< upper edge
  if( 0 == m_multiplicity )
    { return Error("Could not book 'Cluster Multiplicity' histogram" ) ; }

  // cluster size
  m_size           =
    histoSvc () -> book ( directory                        ,  ///< directory
                          2                                ,  ///< ID
                          "Cluster Size   " + inputData()  ,  ///< title
                          50                               ,  ///< #bins
                          0                                ,  ///< low edge
                          50                              ); ///< upper edge
  if( 0 == m_size         )
    { return Error("Could not book 'Cluster Size' histogram"         ) ; }

  // cluster energy
  m_energy         =
    histoSvc () -> book ( directory                        ,  ///< directory
                          3                                ,  ///< ID
                          "Cluster Energy " + inputData()  ,  ///< title
                          500                              ,  ///< #bins
                          0.                               ,  ///< low edge
                          m_energyMax                      ); ///< upper edge
  if( 0 == m_energy       )
    { return Error("Could not book 'Cluster Energy' histogram"       ) ; }

  // cluster transverse energy
  m_et             =
    histoSvc () -> book ( directory                        ,  ///< directory
                          4                                ,  ///< ID
                          "Cluster Et     " + inputData()  ,  ///< title
                          200                              ,  ///< #bins
                          0.                               ,  ///< low edge
                          m_etMax                          ); ///< upper edge
  if( 0 == m_et           )
    { return Error("Could not book 'Cluster Et'     histogram"       ) ; }

  // cluster x-position
  m_x             =
    histoSvc () -> book ( directory                        ,  ///< directory
                          5                                ,  ///< ID
                          "Cluster x      " + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          -4 * meter                       ,  ///< low edge
                          4  * meter                       ); ///< upper edge
  if( 0 == m_x            )
    { return Error("Could not book 'Cluster x'      histogram"       ) ; }

  // cluster y-position
  m_y              =
    histoSvc () -> book ( directory                        ,  ///< directory
                          6                                ,  ///< ID
                          "Cluster y      " + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          -4 * meter                       ,  ///< low edge
                          4  * meter                       ); ///< upper edge
  if( 0 == m_y            )
    { return Error("Could not book 'Cluster y'      histogram"       ) ; }
  
  // cluster x-position
  m_x1             =
    histoSvc () -> book ( directory                         ,  ///< directory
                          7                                 ,  ///< ID
                          "Cluster x (band)" + inputData() ,  ///< title
                          400                               ,  ///< #bins
                          -4 * meter                        ,  ///< low edge
                          4  * meter                        ); ///< upper edge
  if( 0 == m_x1           )
    { return Error("Could not book 'Cluster x(band)' histogram"       ) ; }
  
  // cluster y-position
  m_y1             =
    histoSvc () -> book ( directory                        ,  ///< directory
                          8                                ,  ///< ID
                          "Cluster y      " + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          -4 * meter                       ,  ///< low edge
                          4  * meter                       ); ///< upper edge
  if( 0 == m_y1           )
    { return Error("Could not book 'Cluster y(band)'  histogram"       ) ; }
  
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
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input data 
  Clusters* clusters = get<Clusters> ( inputData() ) ;
  if( 0 == clusters   )              { return StatusCode::FAILURE ; }
  
  // fill multiplicity histogram 
  m_multiplicity -> fill ( clusters->size() );

  for( Clusters::const_iterator cluster = clusters -> begin() ;
       clusters -> end () != cluster ; ++cluster ) 
    {
      // skip NULLs 
      if( 0 == *cluster ) { continue ; }
      
      m_size   -> fill ( (*cluster)->entries().size() ) ;
      
      const double e = (*cluster)->position().e() ;
      const double x = (*cluster)->position().x() ;
      const double y = (*cluster)->position().y() ;
      const double z = (*cluster)->position().z() ;
      
      m_energy -> fill ( e                          ) ;
      m_et     -> fill ( e * sqrt( x*x + y*y )  / z ) ;
      m_x      -> fill ( x                          ) ;
      m_y      -> fill ( y                          ) ;
      
      if( fabs( y ) <= m_yband ) { m_x1 -> fill ( x ) ; }
      if( fabs( x ) <= m_xband ) { m_y1 -> fill ( y ) ; }

    }

  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
