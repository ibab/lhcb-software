// $Id: CaloHypoMatchMonitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/03/17 16:35:19  ibelyaev
//  regular update: MCCaloMonitor and CaloPIDsMonitor
//
// ============================================================================
// Include files
// Relations 
#include   "Relations/IAssociatorWeighted.h"
// from Gaudi
#include   "GaudiKernel/AlgFactory.h"
#include   "GaudiKernel/MsgStream.h" 
#include   "GaudiKernel/IHistogramSvc.h"
// AIDA 
#include   "AIDA/IHistogram1D.h"
// CaloDet 
#include   "CaloDet/DeCalorimeter.h"
// Event 
#include   "Event/CaloHypo.h"
// Event 
#include   "Event/TrStoredTrack.h"
// CaloUtils 
#include   "CaloUtils/dirHbookName.h"
// local
#include   "CaloHypoMatchMonitor.h"

// ============================================================================
/** @file CaloHypoMatchMonitor.cpp
 * 
 *  Implementation file for class : CaloHypoMatchMonitor
 *  @see CaloHypoMatchMonitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloHypoMatchMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoMatchMonitor>         s_Factory ;
const        IAlgFactory&CaloHypoMatchMonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypoMatchMonitor::CaloHypoMatchMonitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloMoniAlg      ( name , svcloc ) 
  , m_associatorType ("AssociatorWeighted<CaloHypo,TrStoredTrack,float>")
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
CaloHypoMatchMonitor::~CaloHypoMatchMonitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoMatchMonitor::initialize() 
{  
  StatusCode sc = CaloMoniAlg::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class CaloMoniAlg",sc);}
  
  // locate the associator 
  m_associator = tool<Asct>( m_associatorType , m_associatorName ) ;
  if( 0 == m_associator ) { return StatusCode::FAILURE ; }
  
  
   // book the histograms
  m_links   = book ( 1 , "Total Links      : " + inputData() + " " + name() , 
                     0 ,  500. , 500 ) ;
  
  // number of relations per hypo 
  m_rels    = book ( 2 , "Rels per Hypo    : " + inputData() + " " + name() , 
                     0 ,   25. ,  50 ) ;
  // minimal weight 
  m_min     = book ( 3 , "Min weight       : " + inputData() + " " + name() , 
                     0 ,  100. , 200 ) ;
  
  // maximal weight 
  m_max     = book ( 4 , "Max weight       : " + inputData() + " " + name() , 
                     0 , 1000. , 200 ) ;
  // weights 
  m_weights = book ( 5 , "Weights          : " + inputData() + " " + name() , 
                     0 , 1000. , 500 ) ;
 
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
StatusCode CaloHypoMatchMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloHypos         Hypos    ;
  
  typedef const Asct::DirectType    Table    ;
  typedef Table::Range              Range    ;
  typedef Table::iterator           iterator ;
  
  // get input data 
  const Hypos* hypos = get<Hypos>( inputData() );
  if( 0 == hypos ) { return StatusCode::FAILURE ; }
  
  // check relations 
  const Table* table = m_associator->direct() ;
  if( 0 == table ) { return Error("'Direct Relation Table' points to NULL!"); }
  
  // logarithm of ( total number of links + 1 ) 
  m_links ->fill( table->relations().size() );
  
  for( Hypos::const_iterator hypo = hypos -> begin() ; 
       hypos -> end() != hypo ; ++hypo ) 
  {
    const Range range = table->relations( *hypo );
    // number of related tracks 
    m_rels -> fill( range.size() ) ;
    if( range.empty() ) { continue ; }
    
    // minimal weight 
    m_min -> fill ( range.front ().weight() ) ;
    // maximal weight 
    m_max -> fill ( range.back  ().weight() ) ;
    // all weights  
    for( iterator relation = range.begin() ; 
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
