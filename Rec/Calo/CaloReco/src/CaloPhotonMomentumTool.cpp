// $Id: CaloPhotonMomentumTool.cpp,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/06/13 12:32:39  ibelyaev
//  version update
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// Kernel
#include "Kernel/CaloHypotheses.h"
#include "Kernel/CaloHypoPrint.h"
// Event 
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "Event/CaloMomentum.h"
// local
#include "CaloPhotonMomentumTool.h"

// ============================================================================
/** @file 
 *  Implementation file for class CaloPhotonMomentumTool
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 19/05/2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloPhotonMomentumToolFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloPhotonMomentumTool>         s_factory ;
const        IToolFactory&CaloPhotonMomentumToolFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see CaloTool
 *  @see  AlgTool
 *  @param type tool type (?) 
 *  @param name tool name 
 *  @param parent pointer to the parent
 */
// ============================================================================
CaloPhotonMomentumTool::CaloPhotonMomentumTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  , m_evaluator ()
  , m_hypos     ()
{ 
  declareInterface<ICaloHypoTool>( this ); 
  m_hypos.push_back( CaloHypotheses::Photon               );
  m_hypos.push_back( CaloHypotheses::PhotonFromMergedPi0  );
  m_hypos.push_back( CaloHypotheses::BremmstrahlungPhoton );
};
// ============================================================================

// ============================================================================
/// destructror
// ===========================================================================+
CaloPhotonMomentumTool::~CaloPhotonMomentumTool(){};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloTool 
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPhotonMomentumTool::initialize ()
{
  // initialize the base class 
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class CaloTool!",sc); }
  // print hypotheses 
  MsgStream log( msgSvc() , name() ) ;
  log << MSG::DEBUG 
      << " List of 'active' hypotheses: " ;
  for( Hypotheses::const_iterator hypo = m_hypos.begin() ;
       m_hypos.end() != hypo ; ++hypo )
    { 
      log << " " ;      
      CaloHypoPrint( log , *hypo );
      log << " " ;
    }
  log << endreq ;
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloHypo
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloPhotonMomentumTool::process    
( CaloHypo* hypo  ) const 
{ return (*this)( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *
 *  Return codes:
 *  
 *   - 500 : CaloHypo* points to NULL
 *  
 *   - 501 : Invalid Hypothesis
 *
 *   - 502 : CaloPosition* points to NULL for hypo object
 *
 *   - 503 : CaloMomentum* is already estimated for hypo object
 *
 *   - 504 : CaloMomentum* could not be estimated for hypo object
 * 
 *  @see ICaloHypo
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloPhotonMomentumTool::operator() 
  ( CaloHypo* hypo  ) const
{
  if( 0 == hypo             ) 
    { return Error("CaloHypo* points to NULL!"           ,  500  ) ; }
  Hypotheses::const_iterator it = 
    std::find( m_hypos.begin() , m_hypos.end()     , hypo->hypothesis() );
  if( m_hypos.end() == it   ) 
    { return Error("Invalid hypothesis"                  ,  501  ) ; }
  if( 0 == hypo->position() )
    { return Error("CaloPosition* points to NULL!"       ,  502  ) ; }
  if( 0 != hypo->momentum() )
    { return Error("CaloMomentum* is already evaluated!" ,  503  ) ; }
  
  // evaluate momentum 
  CaloMomentum* momentum  = m_evaluator( *(hypo->position()) );
  if( 0 == momentum ) 
    { return Error("CaloMomentum* is not     evaluated!" ,  504  ) ; }
  
  // set momentum! 
  hypo->setMomentum( momentum );
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
