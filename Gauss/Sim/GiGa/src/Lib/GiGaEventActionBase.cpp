// $Id: GiGaEventActionBase.cpp,v 1.10 2002-12-07 14:27:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa 
#include "GiGa/GiGaEventActionBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  implementation fo class GiGaEventActionBase
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 6 May 2002 
 */
// ============================================================================

namespace GiGaEventActionBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaEventActionBase> s_Counter ;
#endif   
}

// ============================================================================
/** standard  constructor
 *  @see GiGaBase
 *  @see AlgTool
 *  @param type tool type (?)
 *  @param name  tool name 
 *  @param parent 
 */
// ============================================================================
GiGaEventActionBase::GiGaEventActionBase
( const std::string& type , 
  const std::string& nick , 
  const IInterface*  svc  )
  : GiGaBase          ( type , nick , svc ) 
{ 
  declareInterface<IGiGaEventAction> (this); 
#ifdef GIGA_DEBUG
  GiGaEventActionBaseLocal::s_Counter.increment () ;
#endif 
}
// ============================================================================

// ============================================================================
// virtual destructor
// ============================================================================
GiGaEventActionBase::~GiGaEventActionBase()
{
#ifdef GIGA_DEBUG
  GiGaEventActionBaseLocal::s_Counter.decrement () ;
#endif 
}
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaBase class!", sc); }
  ///
  return Print("GiGaEventActionBase initialized succesfully" ,
               StatusCode::SUCCESS                           , MSG::VERBOSE );
}
// ============================================================================

// ============================================================================
/** finalize initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionBase::finalize   ()  
{ 
  Print("GiGaEventActionBase Finalization" ,
        StatusCode::SUCCESS                , MSG::VERBOSE );
  // finalze the base class 
  return GiGaBase::finalize   (); 
}
// ============================================================================

// ============================================================================
// The End 
// ============================================================================












