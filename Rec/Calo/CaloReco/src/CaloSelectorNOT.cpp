// ============================================================================
// Include files 
// ============================================================================
// local
#include "CaloSelectorNOT.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class : CaloSelectorNOT
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 27 Apr 2002 
 */
// ============================================================================
DECLARE_TOOL_FACTORY( CaloSelectorNOT )
// ============================================================================
/*  StNOTard constructor
 *  @see GaudiTool
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @param type   tool type (?)
 *  @param name   tool name 
 *  @param parent tool parent   
 */
// ============================================================================
CaloSelectorNOT::CaloSelectorNOT
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent ) 
  , m_selectorsTypeNames ()
  , m_selectors          () 
{
  declareInterface<ICaloClusterSelector> (this);
  declareProperty( "SelectorTools" , m_selectorsTypeNames );
}
// ============================================================================
//  destructor (virtual NOT protected)
// ============================================================================
CaloSelectorNOT::~CaloSelectorNOT(){} 
// ============================================================================
/*  stNOTard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelectorNOT::initialize () 
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class GaudiTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != it ; ++it )
  {
    ICaloClusterSelector* selector = tool<ICaloClusterSelector>( *it , this );
    m_selectors.push_back( selector );
  };     
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  stNOTard finalization  of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelectorNOT::finalize   () 
{
  // clear containers 
  m_selectors          .clear() ;
  m_selectorsTypeNames .clear() ;
  // finalize the base class 
  return GaudiTool::finalize () ;
}
// ============================================================================
/*  "select"/"preselect" method 
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectorNOT::select     
( const LHCb::CaloCluster* cluster ) const { return (*this) ( cluster ) ; }
// ============================================================================
/* "select"/"preselect" method (functor interface)
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectorNOT::operator() ( const LHCb::CaloCluster* cluster ) const
{
  bool select = false ;  
  for( Selectors::const_iterator selector = m_selectors.begin() ;
       !select && m_selectors.end() != selector ; ++selector )
    { select = (**selector)( cluster ); }
  ///

  counter("selected clusters") += (int) !select;

  return !select ;
}




