// $Id: GiGaStepActionBase.cpp,v 1.9 2002-05-07 12:21:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// GiGa
#include "GiGa/GiGaStepActionBase.h"

/** @file 
 *  implementation of class GiGaStepActionBase
 *
 *  @author Vanya Belyaev 
 */

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStepActionBase::GiGaStepActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase ( type , name , parent  ) 
{ declareInterface<IGiGaStepAction> (this); };
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaStepActionBase::~GiGaStepActionBase(){};
// ============================================================================

// ============================================================================
/** initialize the step action  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class GiGaBase"); } 
  //
  return Print("GiGaStepActionBase initialized successfully" ,
               StatusCode::SUCCESS                           , MSG::VERBOSE ) ;
}; 
// ============================================================================

// ============================================================================
/** finalize the step action
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionBase::finalize() 
{ 
  Print("GiGaStepActionBase finalization" ,
        StatusCode::SUCCESS               , MSG::VERBOSE ) ;
  // finalize the base class 
  return GiGaBase::finalize();  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================

