// $Id: CaloRemoveMomentum.cpp,v 1.1 2002-09-04 14:46:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event 
#include "Event/CaloHypo.h"
#include "Event/CaloMomentum.h"
// local
#include "CaloRemoveMomentum.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloRemoveMomentum
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloRemoveMomentumFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloRemoveMomentum>         s_factory ;
const        IToolFactory&CaloRemoveMomentumFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service or tool)
 */
// ============================================================================
CaloRemoveMomentum::CaloRemoveMomentum
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  )
  : CaloTool    ( type, name , parent ) 
{
  // interafce 
  declareInterface<ICaloHypoTool> (this);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloRemoveMomentum::~CaloRemoveMomentum() {}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloRemoveMomentum::finalize   () 
{
  // release calorimeter 
  setDet( (const DeCalorimeter*) 0 );
  // finalize the base class 
  return CaloTool::finalize ();
};
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloRemoveMomentum::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool" , sc ) ; }
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloRemoveMomentum::process    ( CaloHypo* hypo  ) const 
{ return (*this) ( hypo ) ; };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloRemoveMomentum::operator() ( CaloHypo* hypo  ) const
{
  // check arguments 
  if( 0 == hypo ) { return Error("CaloHypo* points to NULL");}
  
  CaloMomentum* momentum = hypo->momentum();
  if( 0 != momentum ) { delete momentum ; momentum = 0 ; }
  hypo->setMomentum( 0 );
  
  return StatusCode::SUCCESS ;
  
};  
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================

