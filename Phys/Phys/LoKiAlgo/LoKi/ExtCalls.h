// $Id: ExtCalls.h,v 1.2 2007-06-04 20:22:21 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EXTCALLS_H 
#define LOKI_EXTCALLS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/Loop.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    // ========================================================================
    /** @class ExtFunCalls ExtCalls.h LoKi/ExtCalls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-30
     */
    // ========================================================================
    template <class TYPE> class ExtFunCalls ;
    // ========================================================================
    /** @class ExtCutCalls ExtCalls.h LoKi/ExtCalls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-30
     */
    // ========================================================================
    template <class TYPE> class ExtCutCalls ;
    // ========================================================================
    template <>
    class ExtFunCalls<LHCb::Particle>
    {
    private:
      typedef LHCb::Particle              Type ;
      typedef LoKi::Function<const Type*> Fun  ;      
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class ExtCutCalls<LHCb::Particle>
    {
    private:
      typedef LHCb::Particle               Type ;
      typedef LoKi::Predicate<const Type*> Fun  ;      
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class ExtFunCalls<LHCb::VertexBase>
    {
    private:
      typedef LHCb::VertexBase            Type ;
      typedef LoKi::Function<const Type*> Fun  ;      
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::VertexBase*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&                 p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::VertexBase>& p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Vertex>&     p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::RecVertex>&  p ) { return fun ( p ) ; }
      // ======================================================================
    };
    // ========================================================================
    template <>
    class ExtCutCalls<LHCb::VertexBase>
    {
    private:
      typedef LHCb::VertexBase             Type ;
      typedef LoKi::Predicate<const Type*> Fun  ;      
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::VertexBase*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&                 p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::VertexBase>& p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Vertex>&     p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::RecVertex>&  p ) { return fun ( p ) ; }
      // ======================================================================
    } ;
    // ========================================================================    
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTCALLS_H
// ============================================================================
