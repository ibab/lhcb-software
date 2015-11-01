// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "P2MCBase.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::P2MCBase
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// standard constructor
// ============================================================================
LoKi::P2MCBase::P2MCBase
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : GaudiAlgorithm ( name , pSvc ) 
  , m_inputs  (   ) 
  , m_method  ( Particle2MCMethod::No ) 
  , m_output  (   ) 
  , m_linker  ( 0 )
{
  declareProperty ( "Inputs"  , m_inputs  ) ;
  declareProperty ( "Method"  , m_method  ) ;
  declareProperty ( "Output"  , m_output  ) ;
  // declare handlers 
  m_inputs.declareUpdateHandler ( &LoKi::P2MCBase::inputsHandler , this );
  m_method.declareUpdateHandler ( &LoKi::P2MCBase::methodHandler , this );
} 
// ============================================================================
// virtual protected destructors 
// ============================================================================
LoKi::P2MCBase::~P2MCBase() {}
// ============================================================================
// initialize the algorithm 
// ============================================================================
StatusCode LoKi::P2MCBase::initialize () 
{
  // inittialize the base 
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure()   ) {  return sc ; }                         // RETURN 
  // check the inputs
  if ( m_inputs.value().empty() ) { Warning ( "Empty list of inputs!") ; }
  /// delete linker 
  if ( 0 != m_linker    ) { delete m_linker ; m_linker = 0  ; }
  return StatusCode::SUCCESS ;                                    // RETURN 
}
// ============================================================================
// finalize   the algorithm 
// ============================================================================
StatusCode LoKi::P2MCBase::finalize   () 
{
  // delete linker if needed 
  if ( 0 != m_linker ) { delete m_linker ; m_linker = 0  ; }  
  // finalize the base class 
  return GaudiAlgorithm::finalize () ; 
} 
// ============================================================================
// handler for inputs 
// ============================================================================
void LoKi::P2MCBase::inputsHandler ( Property& /* p */ ) 
{
  if ( 0 == m_linker ) { return ; }                       // NO ACTION 
  // linker exists (and probably initialized already) 
  warning() 
    << "Linker must be reinitialized for " << m_inputs << endmsg  ;
  if ( 0 != m_linker ) { delete m_linker ; m_linker = 0 ; }
} 
// ============================================================================
// handler for method 
// ============================================================================
void LoKi::P2MCBase::methodHandler ( Property& /* p */ ) 
{
  if ( 0 == m_linker ) { return ; }                       // NO ACTION 
  // linker exists (and probably initialized already) 
  warning() 
    << "Linker must be reinitialized for " << m_method << endmsg  ;
  if ( 0 != m_linker ) { delete m_linker ; m_linker = 0 ; }
} 
// ============================================================================
// The END 
// ============================================================================
