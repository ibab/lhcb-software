// $Id: CaloHypoToolSequence.cpp,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
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
#include "CaloHypoToolSequence.h"

// ============================================================================
// Implementation file for class : CaloHypoToolSequence
//
// 27/04/2002 : Ivan Belyaev
// ============================================================================

// ============================================================================
/** @var CaloHypoToolSequenceFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloHypoToolSequence>         s_factory ;
const        IToolFactory&CaloHypoToolSequenceFactory = s_factory ; 
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
CaloHypoToolSequence::CaloHypoToolSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  , m_hypoToolsTypeNames () 
  , m_hypoTools          ()
{
  declareInterface<ICaloHypoTool> (this);
  declareProperty ( "HypoTools" , m_hypoToolsTypeNames );
};
// ============================================================================

// ============================================================================
// desctructor (virtual and protected)
// ============================================================================
CaloHypoToolSequence::~CaloHypoToolSequence() {};
// ============================================================================

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see CaloTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoToolSequence::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloTool",sc);}
  // locate selectors 
  for( Names::const_iterator it = m_hypoToolsTypeNames.begin() ;
       m_hypoToolsTypeNames.end() != it ; ++it )
    {
      ICaloHypoTool* hypoTool = tool( *it , hypoTool );
      m_hypoTools.push_back( hypoTool );
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
StatusCode CaloHypoToolSequence::finalize   () 
{
  // release tools NOT clear containers 
  std::for_each( m_hypoTools.begin () , 
                 m_hypoTools.end   () , std::mem_fun(&IInterface::release));
  m_hypoTools          .clear() ;
  m_hypoToolsTypeNames .clear() ;
  // finalize the base class 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloHypoTool
 *  @param hypo pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloHypoToolSequence::process    ( CaloHypo* hypo ) const 
{ return (*this) ( hypo ); };

// ============================================================================
/** The main processing method (functor interface) 
 *  @see ICaloHypoTool
 *  @param hypo pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloHypoToolSequence::operator() ( CaloHypo* hypo ) const 
{
  StatusCode sc( StatusCode::SUCCESS );
  /// loop over all tools 
  for( HypoTools::const_iterator hypoTool = m_hypoTools.begin() ;
       sc.isSuccess() && m_hypoTools.end() != hypoTool ; ++hypoTool )
    { sc = (**hypoTool) ( hypo ); }
  /// 
  return sc ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

