// $Id: CaloSelectorNOT.cpp,v 1.4 2006-05-30 09:42:05 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2004/02/17 12:08:10  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
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

// ============================================================================
/** @var CaloSelectorNOTFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSelectorNOT>         s_factory ;
const        IToolFactory&CaloSelectorNOTFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** StNOTard constructor
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
};
// ============================================================================

// ============================================================================
/// destructor (virtual NOT protected)
// ============================================================================
CaloSelectorNOT::~CaloSelectorNOT(){} ; 
// ============================================================================

// ============================================================================
/** stNOTard initialization of the tool 
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
    ICaloClusterSelector* selector = tool<ICaloClusterSelector>( *it );
    m_selectors.push_back( selector );
  };     
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** stNOTard finalization  of the tool 
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
};
// ============================================================================

// ============================================================================
/** "select"/"preselect" method 
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectorNOT::select     
( const LHCb::CaloCluster* cluster ) const { return (*this) ( cluster ) ; }
// ============================================================================

// ============================================================================
/** "select"/"preselect" method (functor interface)
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
  return !select ;
};
// ============================================================================




