// $Id: GiGaToolBase.cpp,v 1.1 2003-04-06 18:49:48 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa
#include "GiGa/GiGaToolBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file  
 * 
 *  Implementation file for class : GiGaToolBase
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================

// ============================================================================
namespace GiGaToolBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaToolBase> s_Counter ;
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
GiGaToolBase::GiGaToolBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : GiGaBase( type , name , parent ) 
{
  declareInterface<IGiGaTool> ( this ) ;
  //
#ifdef GIGA_DEBUG
  //
  GiGaToolBaseLocal::s_Counter.increment () ;
  //
#endif
  //  
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaToolBase::~GiGaToolBase()
{
  //
#ifdef GIGA_DEBUG
  //
  GiGaToolBaseLocal::s_Counter.decrement () ;
  //
#endif
//
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
