// $Id: GiGaRunActionBase.cpp,v 1.7 2002-12-07 14:27:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/05/07 12:21:33  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// GiGa 
#include "GiGa/GiGaRunActionBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaRunActionBase
 * 
 *  @date 26/07/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

namespace GiGaRunActionBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaRunActionBase> s_Counter ;
#endif   
}

// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaRunActionBase::GiGaRunActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase( type , name , parent )
{ 
  declareInterface<IGiGaRunAction> ( this ); 
#ifdef GIGA_DEBUG
  GiGaRunActionBaseLocal::s_Counter.increment () ;
#endif 
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaRunActionBase::~GiGaRunActionBase() 
{
#ifdef GIGA_DEBUG
  GiGaRunActionBaseLocal::s_Counter.decrement () ;
#endif 
}
// ============================================================================

// ============================================================================
/** initialization method 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::initialize  ()
{ 
  StatusCode sc = GiGaBase::initialize();   
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class GiGaBase!",sc); }
  ///
  return Print("GiGaRunActionBase initialized successfully" ,
               StatusCode::SUCCESS                          , MSG::VERBOSE );
}
// ============================================================================

// ============================================================================
/** finalization method 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::finalize  ()
{
  Print("GiGaRunActionBase finalization" ,
        StatusCode::SUCCESS              , MSG::VERBOSE ) ;
  ///
  return GiGaBase::finalize(); 
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

