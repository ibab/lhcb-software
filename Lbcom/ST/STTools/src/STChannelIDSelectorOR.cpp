// ============================================================================
// Include files 

// local
#include "STChannelIDSelectorOR.h"


#include "Kernel/STChannelID.h"

// ============================================================================
/** @file 
 * 
 *  Implementation file for class : STChannelIDSelectorOR
 * 
 *  @author M Needham
 *  @date 03/02/2009 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( STChannelIDSelectorOR )

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
STChannelIDSelectorOR::STChannelIDSelectorOR
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent ) 
  , m_selectorsTypeNames ()
  , m_selectors          () 
{
  declareInterface<ISTChannelIDSelector> (this);
  declareProperty( "SelectorTools" , m_selectorsTypeNames );
}
// ============================================================================

// ============================================================================
/// destructor (virtual OR protected)
// ============================================================================
STChannelIDSelectorOR::~STChannelIDSelectorOR(){}
// ============================================================================

// ============================================================================
/** stORard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode STChannelIDSelectorOR::initialize () 
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class GaudiTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != it ; ++it )
  {
    ISTChannelIDSelector* selector = tool<ISTChannelIDSelector>( *it );
    m_selectors.push_back( selector );
  };     
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================

/** "select"/"preselect" method 
 *  @see ISTChannelIDSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STChannelIDSelectorOR::select     
( const LHCb::STChannelID& id ) const { return (*this) ( id ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ISTChannelIDSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STChannelIDSelectorOR::operator() ( const LHCb::STChannelID& id ) const
{
  ///
  bool select = false ;
  for( Selectors::const_iterator selector = m_selectors.begin() ;
       !select && m_selectors.end() != selector ; ++selector )
    { select = (**selector)( id ); }
  ///
  return select ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
