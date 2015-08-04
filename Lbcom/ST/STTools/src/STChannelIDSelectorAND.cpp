// Include files 

// local
#include "STChannelIDSelectorAND.h"

// kernel
#include "Kernel/STChannelID.h"


// ============================================================================
/** @file 
 * 
 *  Implementation file for class : STChannelIDSelectorAND
 *
 *  @author M Needham
 *  @date 10/3/2009 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( STChannelIDSelectorAND )

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
STChannelIDSelectorAND::STChannelIDSelectorAND
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
/// destructor (virtual and protected)
// ============================================================================
STChannelIDSelectorAND::~STChannelIDSelectorAND(){}
// ============================================================================

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode STChannelIDSelectorAND::initialize () 
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

// ============================================================================
/** "select"/"preselect" method 
 *  @see ISTChannelIDSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STChannelIDSelectorAND::select     
( const LHCb::STChannelID& id ) const { return (*this) ( id ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ISTChannelIDSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool STChannelIDSelectorAND::operator() ( const LHCb::STChannelID& id ) const
{
  // no selectors!
  if( m_selectors.empty() ) { return false ; }
  //
  bool select = true ;
  for( Selectors::const_iterator selector = m_selectors.begin() ;
       select && m_selectors.end() != selector ; ++selector )
    { select = (**selector)( id ); }
  ///
  return select ;
}
// ============================================================================


