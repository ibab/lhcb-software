// ============================================================================
// Include files
// CaloInterfaces 
#include "CaloInterfaces/ICaloLikelihood.h"
// local
#include "CaloSelector.h"

// ============================================================================
/** @file CaloSelector.cpp 
 *
 *  Implementation file for class : CaloSelector
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 31/03/2002 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( CaloSelector )

// ============================================================================
/** Standard constructor
 *  @param    type   tool type (?)
 *  @param    name   tool name 
 *  @param    parent tool parent 
 */
// ============================================================================
CaloSelector::CaloSelector( const std::string&  type   ,
                            const std::string&  name   ,
                            const IInterface*   parent )
  : GaudiTool ( type, name , parent ) 
  , m_lhType     (        ) 
  , m_lhName     (        ) 
  , m_likelihood ( 0      )
  , m_cut        ( 1.e+50 ) 
{  
  // interfaces  
  declareInterface<ICaloClusterSelector> (this);
  // properties 
  declareProperty( "LikelihoodType" , m_lhType ) ;
  declareProperty( "LikelihoodName" , m_lhType ) ;
  declareProperty( "LikelihoodCut"  , m_cut    ) ;
}

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelector::~CaloSelector() {}

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelector::initialize() 
{
  // initialialize the base class 
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class GaudiTool!",sc);}
  /// locate the tool 
  m_likelihood = m_lhName.empty() ?
    tool<ICaloLikelihood>( m_lhType            , this ) :
    tool<ICaloLikelihood>( m_lhType , m_lhName , this ) ;
  if( 0 == m_likelihood ) { return StatusCode::FAILURE ; }
  //
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** standard finalization  of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelector::finalize() 
{
  ///finalize the base class 
  return GaudiTool::finalize() ;
}

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelector::operator() ( const LHCb::CaloCluster* cluster ) const
{
  if( 0 == cluster ) {  return false ; }
  return  m_cut <= (*m_likelihood) (cluster) ;
}

// ============================================================================
/** "select"/"preselect" method 
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelector::select     ( const LHCb::CaloCluster* cluster ) const 
{ return (*this) ( cluster ); }

// ============================================================================
// The End 
// ============================================================================
