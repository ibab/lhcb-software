// $Id: CaloClusterToolSequence.cpp,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// local
#include "CaloClusterToolSequence.h"

// ============================================================================
// Implementation file for class : CaloClusterToolSequence
//
// 27/04/2002 : Ivan Belyaev
// ============================================================================

// ============================================================================
/** @var CaloClusterToolSequenceFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloClusterToolSequence>         s_factory ;
const        IToolFactory&CaloClusterToolSequenceFactory = s_factory ; 
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
CaloClusterToolSequence::CaloClusterToolSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  , m_clusterToolsTypeNames () 
  , m_clusterTools          ()
{
  declareInterface<ICaloClusterTool> (this);
  declareProperty ( "ClusterTools" , m_clusterToolsTypeNames );
};
// ============================================================================

// ============================================================================
// desctructor (virtual and protected)
// ============================================================================
CaloClusterToolSequence::~CaloClusterToolSequence() {};
// ============================================================================

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see CaloTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterToolSequence::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_clusterToolsTypeNames.begin() ;
       m_clusterToolsTypeNames.end() != it ; ++it )
    {
      ICaloClusterTool* clusterTool = tool( *it , clusterTool );
      m_clusterTools.push_back( clusterTool );
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
StatusCode CaloClusterToolSequence::finalize   () 
{
  // release tools NOT clear containers 
  std::for_each( m_clusterTools.begin () , 
                 m_clusterTools.end   () , std::mem_fun(&IInterface::release));
  m_clusterTools          .clear() ;
  m_clusterToolsTypeNames .clear() ;
  // finalize the base class 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloClusterTool
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloClusterToolSequence::process    ( CaloCluster* cluster ) const 
{ return (*this) ( cluster ); };

// ============================================================================
/** The main processing method (functor interface) 
 *  @see ICaloClusterTool
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloClusterToolSequence::operator() ( CaloCluster* cluster ) const 
{
  StatusCode sc( StatusCode::SUCCESS );
  /// loop over all tools 
  for( ClusterTools::const_iterator clusterTool = m_clusterTools.begin() ;
       sc.isSuccess() && m_clusterTools.end() != clusterTool ; ++clusterTool )
    { sc = (**clusterTool) ( cluster ); }
  /// 
  return sc ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

