// $Id: CaloClusterMCTruthMonitor.cpp,v 1.2 2004-10-27 11:28:43 ibelyaev Exp $
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
#include   "GaudiKernel/IHistogramSvc.h"
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
#include   "Event/MCParticle.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include   "CaloUtils/dirHbookName.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloClusterMCTruthMonitor.h"
// ============================================================================

// ============================================================================
/** @file CaloClusterMCTruthMonitor.cpp
 * 
 *  Template implementation file for class : CaloClusterMCTruthMonitor
 *  @see CaloClusterMCTruthMonitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloClusterMCTruthMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterMCTruthMonitor>         s_Factory ;
const        IAlgFactory&CaloClusterMCTruthMonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloClusterMCTruthMonitor::CaloClusterMCTruthMonitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloMoniAlg      ( name , svcloc ) 
  , m_associatorType ("AssociatorWeighted<CaloCluster,MCParticle,float>")
  , m_associatorName ("Unknown")
  , m_associator     ( 0 ) 
  , m_links          ( 0 )
  , m_rels           ( 0 ) 
  , m_min            ( 0 ) 
  , m_max            ( 0 ) 
  , m_weights        ( 0 ) 
{
  declareProperty("AssociatorType" , m_associatorType );
  declareProperty("AssociatorName" , m_associatorName );
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloClusterMCTruthMonitor::~CaloClusterMCTruthMonitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterMCTruthMonitor::initialize() 
{  
  StatusCode sc = CaloMoniAlg::initialize();
  if ( sc.isFailure() ) { return sc ; }
  
  // locate the associator 
  m_associator = tool<Asct>( m_associatorType , m_associatorName ) ;
  if ( 0 == m_associator ) { return StatusCode::FAILURE ; }
  
  // book the histograms
  m_links   = book ( 1 , "Total Links      : " + inputData() + " " + name() , 
                     0 , 1000. , 500 ) ;
  
  // number of relations per hypo 
  m_rels    = book ( 2 , "Rels per Cluster : " + inputData() + " " + name() , 
                     0 ,   50. ,  50 ) ;
  // minimal weight 
  m_min     = book ( 3 , "Min weight       : " + inputData() + " " + name() , 
                     0 ,    2. , 100 ) ;
  
  // maximal weight 
  m_max     = book ( 4 , "Max weight       : " + inputData() + " " + name() , 
                     0 ,    2. , 100 ) ;
  // weights 
  m_weights = book ( 5 , "Weights          : " + inputData() + " " + name() , 
                     0 ,    2. , 100 ) ;
  
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
StatusCode CaloClusterMCTruthMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloClusters      Clusters ;
  
  typedef const Asct::DirectType    Table    ;
  typedef Table::Range              Range    ;
  typedef Table::iterator           iterator ;
  
  // get input data 
  Clusters* clusters = get<Clusters>( inputData() );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // check relations 
  const Table* table = m_associator->direct() ;
  if( 0 == table ) { return Error("'Direct Relation Table' points to NULL!"); }
  
  // total number of links 
  m_links ->fill( table->relations().size() );
  
  for( Clusters::const_iterator cluster = clusters -> begin() ; 
       clusters -> end() != cluster ; ++cluster ) 
  {
    const Range range = table->relations( *cluster );
    // number of related tracks 
    m_rels -> fill( range.size() ) ;
    if( range.empty() ) { continue ; }
    
    // cluster energy 
    const double e = (*cluster) -> e() ;
    
    // minimal weight 
    m_min -> fill (  range. front().weight() / e ) ;
    // maximal weight 
    m_max -> fill (  range.  back().weight() / e ) ;
    // all weights  
    for( iterator relation = range.begin() ; 
         range.end() != relation ; ++relation )
    {
      m_weights -> fill( relation->weight() /  e ) ;
    }    
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
