// ============================================================================
// Include files 

// local
#include "STClusterSelectorOR.h"


// ============================================================================
/** @file 
 * 
 *  Implementation file for class : STClusterSelectorOR
 * 
 *  @author M Needham
 *  @date 03/02/2009 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( STClusterSelectorOR )

// ============================================================================
/** StORard constructor
 *  @see GaudiTool
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @param type   tool type (?)
 *  @param name   tool name 
 *  @param parent tool parent   
 */
// ============================================================================
STClusterSelectorOR::STClusterSelectorOR
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
/// destructor (virtual OR protected)
// ============================================================================
STClusterSelectorOR::~STClusterSelectorOR(){}
// ============================================================================

// ============================================================================
/** stORard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode STClusterSelectorOR::initialize () 
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class GaudiTool",sc);}
  // locate selectors 

  if (m_selectorsTypeNames.size() == 0)  return  Error("No selectors specified") ;

  for( Names::const_iterator it = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != it ; ++it )
  {
    ISTClusterSelector* selector = tool<ISTClusterSelector>( *it );
    m_selectors.push_back( selector );
    info() << " Adding selector named " << *it << endmsg;
  };     
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================

/** "select"/"preselect" method 
 *  @see ISTClusterSelector
 *  @param  cluster pointer to ST cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STClusterSelectorOR::select     
( const LHCb::STCluster* cluster ) const { return (*this) ( cluster ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ISTClusterSelector
 *  @param  cluster pointer to ST cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STClusterSelectorOR::operator() ( const LHCb::STCluster* cluster ) const
{
  ///
  bool select = false ;
  for( Selectors::const_iterator selector = m_selectors.begin() ;
       !select && m_selectors.end() != selector ; ++selector )
    { select = (**selector)( cluster ); }
  ///
  return select ;
}
// ============================================================================
// The END 
// ============================================================================
