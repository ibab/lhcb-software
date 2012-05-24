// $Id$
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
// DaVinciTypes 
// ============================================================================
#include "Kernel/DecayTree.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Operators.h"
#include "LoKi/Algo.h"
#include "LoKi/Loop.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi 
{
  // ==========================================================================
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
      // ======================================================================
      typedef LHCb::Particle                             Type ;
      typedef LoKi::BasicFunctors<const Type*>::Function Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::DecayTree&          p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      // ======================================================================
      // __call__
      static std::vector<Fun::result_type> __call__
      ( const Fun& fun  , const Type::ConstVector&    o ) 
      { return o >> fun  ; }
      // __call__
      static std::vector<Fun::result_type> __call__
      ( const Fun& fun  , const Type::Range&          o ) 
      { return o >> fun  ; }
      // __call__
      static std::vector<Fun::result_type> __call__
      ( const Fun& fun  , const Type::Container*      o ) 
      { return o >> fun  ; }
      // __call__
      static std::vector<Fun::result_type> __call__
      ( const Fun& fun  , const Type::Selection*      o ) 
      { return o >> fun  ; }
      // __call__
      static std::vector<Fun::result_type> __call__
      ( const Fun& fun  , const SmartRefVector<Type>& o ) 
      { return o >> fun  ; }      
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const Type*                 o ) 
      { return fun ( o )   ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const LHCb::DecayTree&      o ) 
      { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&           o ) 
      { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&       o ) 
      { return fun ( o )   ; }
      // __rrshift__ 
      static std::vector<Fun::result_type> __rrshift__
      ( const Fun& fun  , const Type::ConstVector&    o ) 
      { return o >> fun  ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__
      ( const Fun& fun  , const Type::Range&          o ) 
      { return o >> fun  ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__
      ( const Fun& fun  , const Type::Container*      o ) 
      { return o >> fun  ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__
      ( const Fun& fun  , const Type::Selection*      o ) 
      { return o >> fun  ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__
      ( const Fun& fun  , const SmartRefVector<Type>& o ) 
      { return o >> fun  ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class ExtCutCalls<LHCb::Particle>
    {
    private:
      // ======================================================================
      typedef LHCb::Particle                              Type ;
      typedef LoKi::BasicFunctors<const Type*>::Predicate Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*           p ) { return fun ( p ) ; }
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::DecayTree&          p ) { return fun ( p ) ; }
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&               p ) { return fun ( p ) ; }
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Particle>& p ) { return fun ( p ) ; }
      //
      // __call__ as filter 
      //
      // __call__ 
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::ConstVector&      o ) 
      { return o >> fun ; }      
      // __call__ 
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Container*        o ) 
      { return o >> fun ; }      
      // __call__ 
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Selection*        o ) 
      { return o >> fun ; }      
      // __call__ 
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const Type::Range&            o ) 
      { return o >> fun ; }      
      // __call__ 
      static Type::ConstVector __call__ 
      ( const Fun& fun  , const SmartRefVector<Type>&   o ) 
      { return o >> fun ; }      
      //      
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::ConstVector&      o )
      { return o >> fun  ; }
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Container*        o ) 
      { return o >> fun ; }      
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Selection*        o ) 
      { return o >> fun ; }      
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const Type::Range&            o ) 
      { return o >> fun ; }      
      // __rrshift__ 
      static Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<Type>&   o )
      { return o >> fun ; }      
      // ======================================================================
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type*                       o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const LHCb::DecayTree&            o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&                 o ) { return fun ( o ) ; }
      // __rrshift__
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&             o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    template <>
    class ExtFunCalls<LHCb::VertexBase>
    {
    private:
      // ======================================================================
      typedef LHCb::VertexBase            Type ;
      typedef LoKi::BasicFunctors<const Type*>::Function Fun  ;      
      // ======================================================================
    public:
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::VertexBase*           p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LoKi::Loop&                 p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::Vertex>&     p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const SmartRef<LHCb::RecVertex>&  p ) { return fun ( p ) ; }
      // ======================================================================
      static Fun::result_type __call__ 
      ( const Fun& fun , const LHCb::Particle*  p ) 
      {
        const LHCb::VertexBase* _vertex = 0 ;
        return 0 != p ? fun ( p->endVertex() ) : fun ( _vertex ) ;
      }
      //
      // ======================================================================
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const LHCb::VertexBase::ConstVector&    o ) 
      { return o >> fun ; }
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const LHCb::Vertex::ConstVector&        o ) 
      { return o >> fun ; }
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const LHCb::RecVertex::ConstVector&     o ) 
      { return o >> fun ; }      
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const LHCb::Vertex::Container*          o ) 
      { return o >> fun ; }
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container*       o ) 
      { return o >> fun ; }
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>&     o ) 
      { return o >> fun ; }
      // __call__
      static std::vector<Fun::result_type> __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>&  o ) 
      { return o >> fun ; }      
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::VertexBase::ConstVector&    o ) 
      { return o >> fun ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::ConstVector&        o ) 
      { return o >> fun ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::ConstVector&     o ) 
      { return o >> fun ; }      
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::Container*          o ) 
      { return o >> fun ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container*       o ) 
      { return o >> fun ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>&     o ) 
      { return o >> fun ; }
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>&  o ) 
      { return o >> fun ; }      
      // ======================================================================
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const LHCb::VertexBase*           o ) { return fun ( o ) ; }
      static Fun::result_type __rrshift__ 
      ( const Fun& fun , const LoKi::Loop&                  o ) { return fun ( o ) ; }
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>&     o ) { return fun ( o ) ; }
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>&  o ) { return fun ( o ) ; }
      //
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle*  p ) 
      {
        const LHCb::VertexBase* _vertex = 0 ;
        return 0 != p ? fun ( p->endVertex() ) : fun ( _vertex ) ;
      } 
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    };
    // ========================================================================
    template <>
    class ExtCutCalls<LHCb::VertexBase>
    {
    private:
      typedef LHCb::VertexBase                            Type ;
      typedef LoKi::BasicFunctors<const Type*>::Predicate Fun  ;      
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LHCb::VertexBase*           o ) { return fun ( o ) ; }
      // __call__ 
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LoKi::Loop&                 o ) { return fun ( o ) ; }
      // __call__ 
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>&     o ) { return fun ( o ) ; }
      // __call__ 
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>&  o ) { return fun ( o ) ; }
      //
      //
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LHCb::Particle*  p ) 
      {
        const LHCb::VertexBase* _vertex = 0 ;
        return 0 != p ? fun ( p->endVertex() ) : fun ( _vertex ) ;
      }
      // call as filter
      //
      // __call__ 
      static LHCb::VertexBase::ConstVector __call__ 
      ( const Fun& fun  , const LoKi::Types::VRange&            i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::VertexBase::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::VertexBase::ConstVector&  i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::VertexBase::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Vertex::ConstVector&      i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::RecVertex::ConstVector  __call__ 
      ( const Fun& fun  , const LHCb::RecVertex::ConstVector&   i )
      { return i >> fun ; }      
      // __call__ 
      static LHCb::VertexBase::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Vertex::Container*        i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::RecVertex::ConstVector  __call__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container*     i )
      { return i >> fun ; }      
      // __call__ 
      static LHCb::VertexBase::ConstVector  __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>&   i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::RecVertex::ConstVector   __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>& i )
      { return i >> fun ; }
      // ======================================================================
      // __call__ 
      static LHCb::Particle::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Particle::ConstVector&    i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::Particle::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Particle::Container*      i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::Particle::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Particle::Selection*      i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::Particle::ConstVector __call__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Particle>& i )
      { return i >> fun ; }
      // __call__ 
      static LHCb::Particle::ConstVector __call__ 
      ( const Fun& fun  , const LHCb::Particle::Range&          i )
      { return i >> fun ; }      
      // ======================================================================
    public:
      // ======================================================================      
      //
      // rrshift as filter 
      //
      // __rrshift__ 
      static LHCb::VertexBase::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::VertexBase::ConstVector&  i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::VertexBase::ConstVector __rrshift__ 
      ( const Fun& fun  , const LoKi::Types::VRange&            i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::VertexBase::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::ConstVector&      i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::RecVertex::ConstVector  __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::ConstVector&   i )
      { return i >> fun ; }      
      // __rrshift__ 
      static LHCb::VertexBase::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Vertex::Container*        i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::RecVertex::ConstVector  __rrshift__ 
      ( const Fun& fun  , const LHCb::RecVertex::Container*     i )
      { return i >> fun ; }      
      // __rrshift__ 
      static LHCb::VertexBase::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Vertex>&   i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::RecVertex::ConstVector   __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::RecVertex>& i )
      { return i >> fun ; }
      // ======================================================================
      // some really  weird stuff
      // ======================================================================
      // __rrshift__ 
      static LHCb::Particle::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle::ConstVector&    i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::Particle::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle::Container*      i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::Particle::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle::Selection*      i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::Particle::ConstVector __rrshift__ 
      ( const Fun& fun  , const SmartRefVector<LHCb::Particle>& i )
      { return i >> fun ; }
      // __rrshift__ 
      static LHCb::Particle::ConstVector __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle::Range&          i )
      { return i >> fun ; }      
      // ======================================================================
      // __rrshift_ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const LHCb::VertexBase*           o ) { return fun ( o ) ; }
      // __rrshift_ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const LoKi::Loop&                 o ) { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>&     o ) { return fun ( o ) ; }
      // __rrshift__ 
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>&  o ) { return fun ( o ) ; }
      //
      static Fun::result_type __rrshift__ 
      ( const Fun& fun  , const LHCb::Particle*  p ) 
      {
        const LHCb::VertexBase* _vertex = 0 ;
        return 0 != p ? fun ( p->endVertex() ) : fun ( _vertex ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun& o ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================    
  } //                                             end of namespace LoKi::Dicts
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTCALLS_H
// ============================================================================
