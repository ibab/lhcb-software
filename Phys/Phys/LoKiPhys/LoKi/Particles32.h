// $Id: Particles32.h,v 1.1 2010-02-19 16:40:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES32_H 
#define LOKI_PARTICLES32_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class BestPrimaryVertexAdaptor
     *  Simple adaptor,that delegates the evaluation of "vertex"
     *  functor to "best-primary-vertex"
     *  @see LoKi::Cuts::BPV
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date 2010-02-19
     */
    class BestPrimaryVertexAdaptor 
      : public         LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      // ======================================================================
      /// constructor from vertex-function 
      BestPrimaryVertexAdaptor 
      ( const LoKi::PhysTypes::VFunc& vfun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~BestPrimaryVertexAdaptor() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BestPrimaryVertexAdaptor* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice string representation 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      BestPrimaryVertexAdaptor () ;      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual vertex-functor 
      LoKi::PhysTypes::VFun m_vfun ;               // the actual vertex-functor 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES32_H
// ============================================================================
