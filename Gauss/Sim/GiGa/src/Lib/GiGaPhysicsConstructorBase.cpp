// $Id: GiGaPhysicsConstructorBase.cpp,v 1.1 2003-04-06 18:49:47 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// include 
#include "GiGa/GiGaPhysicsConstructorBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  implementation of file GiGaPhysicsConstructorBase
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================

// ============================================================================
namespace GiGaPhysicsConstructorBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaPhysicsConstructorBase> s_Counter ;
#endif   
}
// ============================================================================

// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaPhysicsConstructorBase::GiGaPhysicsConstructorBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase( type , name , parent )
{
  declareInterface<IGiGaPhysicsConstructor> ( this ) ;
  //
#ifdef GIGA_DEBUG
  GiGaPhysicsConstructorBaseLocal::s_Counter.increment () ;
#endif
  //
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysicsConstructorBase::~GiGaPhysicsConstructorBase()
{
  //
#ifdef GIGA_DEBUG
  GiGaPhysicsConstructorBaseLocal::s_Counter.decrement () ;
#endif
  //
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
