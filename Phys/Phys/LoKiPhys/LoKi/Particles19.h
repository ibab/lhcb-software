// $Id: Particles19.h,v 1.3 2008-11-02 20:13:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES19_H 
#define LOKI_PARTICLES19_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/VertexHolder.h"
// ============================================================================
/** @file
 * 
 *  Collection of LoKi-functors, dealing with "LifeTime"-fitter
 *  @see ILifetimeFitter
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2008-01-17
 */
namespace LoKi 
{
  namespace Particles
  {
    // ========================================================================
    /** @class LifeTime 
     *  The simple function which evaliuates the lifetime of the particle 
     *  using ILifetimeFitter tool
     *
     *  @see LoKi::Cuts::LTIME
     *  @see LoKi::Cuts::LIFETIME
     *  @see ILifetimeFitter
     *
     *  @attention Please mind the units! Unlike all other "time" quantities in LoKi,
     *             this functor returns "time"-units instead of "length"-units!
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTime 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder 
    {
    public:
      // ======================================================================
      /// constructor 
      LifeTime ( const LHCb::VertexBase* vertex , 
                 const ILifetimeFitter*  tool   ) ;
      /// constructor 
      LifeTime ( const ILifetimeFitter*  tool   , 
                 const LHCb::VertexBase* vertex ) ;
      /// constructor 
      LifeTime ( const LHCb::VertexBase*                  vertex , 
                 const LoKi::Interface<ILifetimeFitter>&  tool   ) ;
      /// constructor 
      LifeTime ( const LoKi::Interface<ILifetimeFitter>&  tool   , 
                 const LHCb::VertexBase*                  vertex ) ;
      /// MANDATORY: virtual destructor 
      virtual ~LifeTime() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTime* clone() const { return new LifeTime ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return lifeTime ( p )  ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "LIFETIME" ; }
      // ======================================================================
    public:
      // ======================================================================
      // set the fitter 
      void setTool ( const ILifetimeFitter* tool ) const { m_fitter = tool ; }
      // set the fitter 
      void setTool ( const LoKi::Interface<ILifetimeFitter>& tool ) const 
      { m_fitter = tool ; }      
      // get the fitter 
      const ILifetimeFitter* tool   () const { return m_fitter ; }
      // get the fitter 
      const ILifetimeFitter* fitter () const { return m_fitter ; }
      // cast to the fitter 
      operator const LoKi::Interface<ILifetimeFitter>&() const 
      { return m_fitter ; }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the lifetime 
      result_type lifeTime ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LifeTime () ; // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the lifetime fitter itself
      mutable LoKi::Interface<ILifetimeFitter> m_fitter ; // the lifetime fitter
      // ======================================================================
    };
    // ========================================================================
    /** @class LifeTimeChi2 
     *  The simple function which evaliuates the chi2 for lifetime of the particle 
     *  using ILifetimeFitter tool
     *
     *  @see LoKi::Cuts::LTCHI2
     *  @see LoKi::Cuts::LTIMECHI2
     *  @see LoKi::Cuts::LIFETIMECHI2
     *  @see ILifetimeFitter
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeChi2 : public LifeTime 
    {
    public:
      /// constructor 
      LifeTimeChi2 ( const LHCb::VertexBase* vertex , 
                     const ILifetimeFitter*  tool   ) 
        : LifeTime ( vertex , tool ) {}      
      /// constructor 
      LifeTimeChi2 ( const ILifetimeFitter*  tool   , 
                     const LHCb::VertexBase* vertex ) 
        : LifeTime ( vertex , tool ) {}      
      /// constructor 
      LifeTimeChi2 ( const LHCb::VertexBase*                  vertex , 
                     const LoKi::Interface<ILifetimeFitter>&  tool   ) 
        : LifeTime ( vertex , tool ) {}      
      /// constructor 
      LifeTimeChi2 ( const LoKi::Interface<ILifetimeFitter>&  tool   , 
                     const LHCb::VertexBase*                  vertex ) 
        : LifeTime ( vertex , tool ) {}      
      /// MANDATORY: virtual destructor 
      virtual ~LifeTimeChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeChi2* clone() const 
      { return new LifeTimeChi2 ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return lifeTimeChi2 ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "LTCHI2" ; }
      // ======================================================================
   public:
      // ======================================================================
      // evaluate the lifetime 
      result_type lifeTimeChi2 ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LifeTimeChi2 () ; // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class LifeTimeSignedChi2 
     *  The simple function which evaliuates the chi2 for lifetime of the particle 
     *  using ILifetimeFitter tool
     *
     *  @see LoKi::Cuts::LTSIGNCHI2
     *  @see LoKi::Cuts::LTIMESIGNCHI2
     *  @see LoKi::Cuts::LIFETIMESIGNCHI2
     *  @see ILifetimeFitter
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeSignedChi2 : public LoKi::Particles::LifeTimeChi2
    {
    public:
      // ======================================================================
      /// constructor 
      LifeTimeSignedChi2
      ( const LHCb::VertexBase* vertex , 
        const ILifetimeFitter*  tool   ) 
        : LoKi::Particles::LifeTimeChi2 (  vertex , tool ) {}
      /// constructor 
      LifeTimeSignedChi2
      ( const ILifetimeFitter*  tool   , 
        const LHCb::VertexBase* vertex ) 
        : LoKi::Particles::LifeTimeChi2 (  vertex , tool ) {}
      /// constructor 
      LifeTimeSignedChi2
      ( const LHCb::VertexBase*                  vertex , 
        const LoKi::Interface<ILifetimeFitter>&  tool   ) 
        : LoKi::Particles::LifeTimeChi2 (  vertex , tool ) {}
      /// constructor 
      LifeTimeSignedChi2
      ( const LoKi::Interface<ILifetimeFitter>&  tool   , 
        const LHCb::VertexBase*                  vertex ) 
        : LoKi::Particles::LifeTimeChi2 (  vertex , tool ) {}
      /// MANDATORY: virtual destructor 
      virtual ~LifeTimeSignedChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeSignedChi2* clone() const 
      { return new LifeTimeSignedChi2(*this ) ; }    
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return lifeTimeSignedChi2 ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "LTSIGNCHI2" ; }
      // ======================================================================
    public:
      // ======================================================================
      // evaluate the lifetime 
      result_type lifeTimeSignedChi2 ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LifeTimeSignedChi2 () ; // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class LifeTimeFitChi2 
     *  The simple function which evaliuates the chi2 for lifetime 
     *  fit of the particle 
     *  using ILifetimeFitter tool
     *
     *  @see LoKi::Cuts::LTFITCHI2
     *  @see LoKi::Cuts::LTIMEFITCHI2
     *  @see LoKi::Cuts::LIFETIMEFITCHI2
     *  @see ILifetimeFitter
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeFitChi2 : public LoKi::Particles::LifeTime 
    {
    public:
      // ======================================================================
      /// constructor 
      LifeTimeFitChi2
      ( const LHCb::VertexBase* vertex , 
        const ILifetimeFitter*  tool   ) 
        : LoKi::Particles::LifeTime ( vertex , tool ) {}
      /// constructor 
      LifeTimeFitChi2
      ( const ILifetimeFitter*  tool   , 
        const LHCb::VertexBase* vertex ) 
        : LoKi::Particles::LifeTime ( vertex , tool ) {}
      /// constructor 
      LifeTimeFitChi2
      ( const LHCb::VertexBase*                  vertex , 
        const LoKi::Interface<ILifetimeFitter>&  tool   ) 
        : LoKi::Particles::LifeTime ( vertex , tool ) {}
      /// constructor 
      LifeTimeFitChi2
      ( const LoKi::Interface<ILifetimeFitter>&  tool   , 
        const LHCb::VertexBase*                  vertex ) 
        : LoKi::Particles::LifeTime ( vertex , tool ) {}
      /// MANDATORY: virtual destructor 
      virtual ~LifeTimeFitChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeFitChi2* clone() const 
      { return  new LoKi::Particles::LifeTimeFitChi2 ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return lifeTimeFitChi2 ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "LTFITCHI2" ; }
      // ======================================================================
    public:
      // ======================================================================
      // evaluate the lifetime 
      result_type lifeTimeFitChi2 ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      LifeTimeFitChi2 () ; ///< the default constructor is disabled
      // ======================================================================
    };
   // ========================================================================
  } // end of namespace LoKi::Particles 
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_PARTICLES19_H
// ============================================================================
