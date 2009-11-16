// $Id: Particles31.h,v 1.1 2009-11-16 11:54:26 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES31_H 
#define LOKI_PARTICLES31_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class PCutA 
     *  Simple adapter (particle predicate) whcih delegates 
     *  "array-predicate" to its daughter particles:
     *
     *  @code
     *
     *        array_functor ( p-> daughters() ) 
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::PCUTA 
     *
     *  @see LoKi::Types::ACut
     *  @see LoKi::Types::ACuts
     *  @see LoKi::ATypes::ACut
     *  @see LoKi::ATypes::ACuts
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2009-11-16
     */
    class PCutA : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from "array"-predicate 
      PCutA ( const LoKi::ATypes::ACuts& cut ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~PCutA();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PCutA* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PCutA() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      // the actual predicate
      LoKi::ATypes::ACut  m_cut ;                       // the actual predicate
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PFunA 
     *  Simple adapter (particle function) which delegates 
     *  "array-predicate" to its daughter particles:
     *
     *  @code
     *
     *        array_functor ( p-> daughters() ) 
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::PFUNA 
     *
     *  @see LoKi::Types::AFun
     *  @see LoKi::Types::AFunc
     *  @see LoKi::ATypes::AFun
     *  @see LoKi::ATypes::AFunc
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2009-11-16
     */
    class PFunA : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from "array"-predicate 
      PFunA ( const LoKi::ATypes::AFunc& fun ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~PFunA();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PFunA* clone() const ;
      /// MANDATIRY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PFunA() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      // the actual functors
      LoKi::ATypes::AFun  m_fun ;                         // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PCutV
     *  Simple adapter (particle predicate) whcih delegates 
     *  "array-predicate" to its daughter particles:
     *
     *  @code
     *
     *        array_functor ( p-> daughters() ) 
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::PCUTV
     *
     *  @see LoKi::Types::CutVals
     *  @see LoKi::Types::CutVal
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2009-11-16
     */
    class PCutV : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from "array"-predicate 
      PCutV ( const LoKi::Types::CutVals& cut ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~PCutV();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PCutV* clone() const ;
      /// MANDATIRY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PCutV() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      // the actual predicate
      LoKi::Types::CutVal  m_cut ;                      // the actual predicate
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PFunV
     *  Simple adapter (particle function) which delegates 
     *  "array-predicate" to its daughter particles:
     *
     *  @code
     *
     *        array_functor ( p-> daughters() ) 
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::PFUNV 
     *
     *  @see LoKi::Types::FunVal
     *  @see LoKi::Types::FunVals
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2009-11-16
     */
    class PFunV : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from "array"-predicate 
      PFunV ( const LoKi::Types::FunVals& fun ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~PFunV();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PFunV* clone() const ;
      /// MANDATIRY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PFunV() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      // the actual functors
      LoKi::Types::FunVal  m_fun ;                        // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef PCUTA 
     *  Adapter for array-functor, which acts according to the rule:
     *  @code 
     *  cut ( p ) :  array_cut ( p->daughters ) 
     *  @endcode
     *  @see LoKi::Particles::PCutA 
     *  @see LoKi::ATypes::ACut
     *  @see LoKi::ATypes::ACuts
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::Particles::PCutA                                      PCUTA ;
    // ========================================================================
    /** @typedef PCUTV
     *  Adapter for array-functor, which acts according to the rule:
     *  @code 
     *  cut ( p ) :  vector_cut ( p->daughters ) 
     *  @endcode
     *  @see LoKi::Particles::PCutA 
     *  @see LoKi::Types::CutVals
     *  @see LoKi::Types::CutVal
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::Particles::PCutV                                      PCUTV ;
    // ========================================================================
    /** @typedef PFUNA 
     *  Adapter for array-functor, which acts according to the rule:
     *  @code 
     *  fun ( p ) :  array_fun ( p->daughters ) 
     *  @endcode
     *  @see LoKi::Particles::PFunA 
     *  @see LoKi::ATypes::AFun
     *  @see LoKi::ATypes::AFunc
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::Particles::PFunA                                      PFUNA ;
    // ========================================================================
    /** @typedef PFUNV
     *  Adapter for array-functor, which acts according to the rule:
     *  @code 
     *  fun ( p ) :  vector_fun ( p->daughters ) 
     *  @endcode
     *  @see LoKi::Particles::PFunV
     *  @see LoKi::Types::FunVals
     *  @see LoKi::Types::FunVal
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    typedef LoKi::Particles::PFunV                                      PFUNV ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES31_H
// ============================================================================

