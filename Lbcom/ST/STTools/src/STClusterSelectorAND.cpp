// Include files 

// local
#include "STClusterSelectorAND.h"


// ============================================================================
/** @file 
 * 
 *  Implementation file for class : STClusterSelectorAND
 *
 *  @author M Needham
 *  @date 10/2/09 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( STClusterSelectorAND )

// ============================================================================
/** Standard constructor
 *  @see GaudiTool
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @param type   tool type (?)
 *  @param name   tool name 
 *  @param parent tool parent   
 */
// ============================================================================
STClusterSelectorAND::STClusterSelectorAND
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent ) 
  , m_selectorsTypeNames ()
  , m_selectors          () 
{
  declareInterface<ISTClusterSelector> (this);
  declareProperty( "SelectorTools" , m_selectorsTypeNames );
}
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
STClusterSelectorAND::~STClusterSelectorAND(){}
// ============================================================================

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode STClusterSelectorAND::initialize () 
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class GaudiTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != it ; ++it )
  {
    ISTClusterSelector* selector = tool<ISTClusterSelector>( *it );
    m_selectors.push_back( selector );
  };     
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** "select"/"preselect" method 
 *  @see ISTClusterSelector
 *  @param  cluster pointer to ST cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STClusterSelectorAND::select     
( const LHCb::STCluster* cluster ) const { return (*this) ( cluster ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ISTClusterSelector
 *  @param  cluster pointer to ST cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STClusterSelectorAND::operator() ( const LHCb::STCluster* cluster ) const
{


  // no selectors!
  if( m_selectors.empty() ) { return false ; }
  //
  bool select = true ;
  for( Selectors::const_iterator selector = m_selectors.begin() ;
    select && m_selectors.end() != selector ; ++selector ) { 
    select = (**selector)( cluster );
    }
  ///
  return select ;
}
// ============================================================================


