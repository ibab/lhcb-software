// $Id: CaloSelectorAND.cpp,v 1.2 2004-02-17 12:08:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:42  ibelyaev
// new package 
//
// Revision 1.1  2002/04/27 19:21:30  ibelyaev
//  several 'technical' tools are added
//
// ============================================================================
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "CaloSelectorAND.h"


// ============================================================================
/** @file 
 * 
 *  Implementation file for class : CaloSelectorAND
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 27 Apr 2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloSelectorANDFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSelectorAND>         s_factory ;
const        IToolFactory&CaloSelectorANDFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see CaloTool
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @param type   tool type (?)
 *  @param name   tool name 
 *  @param parent tool parent   
 */
// ============================================================================
CaloSelectorAND::CaloSelectorAND
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : CaloTool ( type, name , parent ) 
  , m_selectorsTypeNames ()
  , m_selectors          () 
{
  declareInterface<ICaloClusterSelector> (this);
  declareProperty( "SelectorTools" , m_selectorsTypeNames );
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectorAND::~CaloSelectorAND(){} ; 
// ============================================================================

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see CaloTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelectorAND::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_selectorsTypeNames.begin() ;
       m_selectorsTypeNames.end() != it ; ++it )
  {
    ICaloClusterSelector* selector = tool<ICaloClusterSelector>( *it );
    m_selectors.push_back( selector );
  };     
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** standard finalization  of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see CaloTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelectorAND::finalize   () 
{
  // clear containers 
  m_selectors          .clear() ;
  m_selectorsTypeNames .clear() ;
  // finalize the base class 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** "select"/"preselect" method 
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectorAND::select     
( const CaloCluster* cluster ) const { return (*this) ( cluster ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectorAND::operator() ( const CaloCluster* cluster ) const
{
  // no selectors!
  if( m_selectors.empty() ) { return false ; }
  //
  bool select = true ;
  for( Selectors::const_iterator selector = m_selectors.begin() ;
       select && m_selectors.end() != selector ; ++selector )
    { select = (**selector)( cluster ); }
  ///
  return select ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================


