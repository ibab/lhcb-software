// $Id: GiGaStepActionBase.cpp,v 1.10 2002-12-07 14:27:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
// 
// ============================================================================
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "GiGa/GiGaUtil.h"

/** @file 
 *  implementation of class GiGaStepActionBase
 *
 *  @author Vanya Belyaev 
 */

namespace GiGaStepActionBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaStepActionBase> s_Counter ;
#endif   
};

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
{ 
  declareInterface<IGiGaStepAction> (this); 
#ifdef GIGA_DEBUG
  GiGaStepActionBaseLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaStepActionBase::~GiGaStepActionBase()
{
#ifdef GIGA_DEBUG
  GiGaStepActionBaseLocal::s_Counter.decrement () ;
#endif
};
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

