// $Id: CaloClusterMatchMonitor.cpp,v 1.2 2004-10-27 11:28:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include   "Relations/IAssociatorWeighted.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include   "GaudiKernel/AlgFactory.h"
#include   "GaudiKernel/MsgStream.h" 
// ============================================================================
// AIDA 
// ============================================================================
#include   "AIDA/IHistogram1D.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include   "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event 
// ============================================================================
#include   "Event/CaloHypo.h"
#include   "Event/TrStoredTrack.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include   "CaloUtils/dirHbookName.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloClusterMatchMonitor.h"
// ============================================================================

// ============================================================================
/** @file CaloClusterMatchMonitor.cpp
 * 
 *  Template implementation file for class : CaloClusterMatchMonitor
 *  @see CaloClusterMatchMonitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloClusterMatchMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterMatchMonitor>         s_Factory ;
const        IAlgFactory&CaloClusterMatchMonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloClusterMatchMonitor::CaloClusterMatchMonitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloHistoAlg     ( name , svcloc ) 
  , m_associatorType ("AssociatorWeighted<CaloCluster,TrStoredTrack,float>")
  , m_associatorName ("Unknown")
  , m_associator     ( 0 ) 
  , m_links          ( 0 ) 
  , m_rels           ( 0 ) 
  , m_min            ( 0 ) 
  , m_max            ( 0 ) 
  , m_weights        ( 0 ) 
{
  declareProperty ( "AssociatorType" , m_associatorType ) ;
  declareProperty ( "AssociatorName" , m_associatorName ) ;
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloClusterMatchMonitor::~CaloClusterMatchMonitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterMatchMonitor::initialize() 
{  
  StatusCode sc = CaloHistoAlg::initialize();
  if ( sc.isFailure() ) { return  sc ; } 
  
  // locate the associator 
  m_associator = tool<Asct>( m_associatorType , m_associatorName ) ;
  if ( 0 == m_associator ) { return StatusCode::FAILURE ; }
  
  // book the histograms
  // link multiplicity
  m_links = book ( 1 , "Total Links '" + inputData() + "'" , 0 , 500 , 500  ) ;
  if ( 0 == m_links )
  { return Error ( "Could not book 'Link Multiplicity' histogram" ) ; }
  
  // number of relations per hypo 
  m_rels = book    ( 2 , " #Rels per hypo '" + inputData() + "'" , 0 , 20 , 40 ) ;
  if ( 0 == m_rels       )
  { return Error   ( "Could not book 'Rels/Hypo' histogram"       ) ; }
  
  // minimal weight 
  m_min = book     ( 3 , " Minimal weight '" + inputData() + "'" , 0 , 100 , 200 ) ;
  if ( 0 == m_min           )
  { return Error   ( "Could not book 'Min Weight'     histogram"       ) ; }
  
  // maximal weight 
  m_max  = book    ( 4 , " Maximal weight '" + inputData() + "'" , 0 , 1000, 200 ) ;
  if( 0 == m_max           )
  { return Error   ( "Could not book 'Max Weight'     histogram"       ) ; }
  
  // weights 
  m_weights = book ( 5 , " Weights '"+ inputData() + "'" , 0 , 1000 , 500 ) ;
  if ( 0 == m_weights           )
  { return Error   ( "Could not book 'Weights'     histogram"       ) ; }
  
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
StatusCode CaloClusterMatchMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloClusters      Clusters ;
  
  typedef const Asct::DirectType    Table    ;
  typedef Table::Range              Range    ;
  typedef Table::iterator           iterator ;
  
  // get input data 
  Clusters* clusters = get<Clusters>( inputData() );
  if ( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // check relations 
  const Table* table = m_associator->direct() ;
  if ( 0 == table ) { return Error("'Direct Relation Table' points to NULL!"); }
  
  // total number of links 
  m_links -> fill ( table->relations().size() );
  
  for ( Clusters::const_iterator cluster = clusters -> begin() ; 
        clusters -> end() != cluster ; ++cluster ) 
  {
    const Range range = table->relations( *cluster );
    // number of related tracks 
    m_rels -> fill ( range.size() ) ;
    if ( range.empty() ) { continue ; }
    
    // minimal weight 
    m_min -> fill (  range.begin()->weight()                 ) ;
    // maximal weight 
    m_max -> fill ( (range.begin()+range.size()-1)->weight() );
    // all weights  
    for ( iterator relation = range.begin() ; 
          range.end() != relation ; ++relation )
    {
      m_weights -> fill ( relation->weight() );
    }    
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
