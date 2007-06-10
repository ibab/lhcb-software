// $Id: Loop.h,v 1.4 2007-06-10 20:13:34 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOOP_H 
#define LOKI_LOOP_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/LoopObj.h"
// ============================================================================
/** @file
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
 *  @date 2006-03-22 
 */
// ============================================================================

namespace LoKi 
{  
  /// forward declaration 
  class LoopObj ;
  /** @class Loop Loop.h LoKi/Loop.h
   *  Useful handler for easy ans saf eoperation with class LoKi::LoopObj
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-22
   */
  class Loop 
    : public LoKi::Interface<LoKi::LoopObj>
  {
  public:
    /// constructor from LoKi::LoopObj
    Loop ( const LoKi::LoopObj* object ) ;
    /// copy constructor
    Loop ( const Loop&          object ) ;
    /// vitual destructor
    virtual ~Loop() ;
  public:
    /// conversion operator to LoKi::LoopObj
    operator       LoKi::LoopObj*    () const { return getObject() ; }
    /// conversion to bool value 
    operator bool                    () const { return valid    () ; }
    /// conversion operator to const LHCb::Particle*
    operator const LHCb::Particle*   () const { return particle () ; }
    /// conversion operator to const LHCb::Vertex*
    operator const LHCb::Vertex*     () const { return vertex   () ; }
    /// get child of the loop object 
    const LHCb::Particle* operator  () ( const size_t index ) const 
    { return child( index ) ; }
    /// "check-validity" operator
    bool                  operator! () const { return ! valid() ; }
  public:
    const Loop& operator++ ()      const 
    { if ( validPointer() ) { getObject()->next() ; } ; return *this ; };
    const Loop& operator++ ( int ) const { return ++(*this) ; }
  public:
    /// valid object??
    bool valid() const { return validPointer() && getObject()->valid() ; }
    /// conversion operator to const LHCb::Particle*    
    const LHCb::Particle*   particle () const ;
    /// conversion operator to const LHCb::Vertex*
    const LHCb::Vertex*     vertex   () const ;    
    /// get child of the loop object 
    const LHCb::Particle*   child ( const size_t index ) const ;
  private:
    // the default constructior is disabled
    Loop();
  };
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_LOOP_H
// ============================================================================




