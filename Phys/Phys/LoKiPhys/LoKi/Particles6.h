// $Id: Particles6.h,v 1.8 2009-07-09 13:39:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES6_H 
#define LOKI_PARTICLES6_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
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
 *  @date 2006-02-21
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class ChildFunction
     *  Simple adapter function which 
     *  apply the function to a daughter  particle
     * 
     *  @code
     *
     *  const LHCb::Particle* B = ... ;
     *
     *  // the transverse momentum of the first daughter particle:
     *  Fun fun = CHILD( PT , 1 ) ;
     *  const double pt1 = fun( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::CHILD 
     *  @see LoKi::Cuts::CHILDFUN
     *  @see LoKi::Child::child 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-21
     */
    class ChildFunction
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildFunction 
      ( const LoKi::PhysTypes::Func& fun   , 
        const size_t                 index ) ;
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildFunction 
      ( const LoKi::PhysTypes::Func& fun    , 
        const size_t                 index1 , 
        const size_t                 index2 ) ;
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildFunction 
      ( const LoKi::PhysTypes::Func& fun    , 
        const size_t                 index1 , 
        const size_t                 index2 ,
        const size_t                 index3 ) ;
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildFunction 
      ( const LoKi::PhysTypes::Func& fun    , 
        const size_t                 index1 , 
        const size_t                 index2 ,
        const size_t                 index3 ,
        const size_t                 index4 ) ;
      /** constructor from the function and daughter index 
       *  @param index  the index of daughter particle
       *  @param fun    the function to be used 
       */
      ChildFunction
      ( const size_t                 index ,
        const LoKi::PhysTypes::Func& fun   ) ;
      /// copy 
      ChildFunction ( const ChildFunction& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildFunction(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildFunction*  clone() const 
      { return new ChildFunction(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the function itself 
      LoKi::PhysTypes::Fun m_fun ;                       // the function itself 
      /// indices of daughter particle
      std::vector<size_t> m_index ;             // indices of daughter particle
      // ======================================================================
    };
    // ========================================================================    
    /** @class ChildPredicate
     *  Simple adapter predicate  which 
     *  apply the predicate to a daughter  particle
     * 
     *  @code
     *
     *  const LHCb::Particle* B = ... ;
     *
     *  // ask if the first daughter is pi+
     *  Cut cut = CHILDCUT( "pi+" == ID  , 1 ) ;
     *  const bool pion = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::CHILDCUT
     *  @see LoKi::Child::child 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-21
     */
    class ChildPredicate 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /** constructor from the function and daughter index 
       *  @param cut    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildPredicate 
      ( const LoKi::PhysTypes::Cuts& cut   , 
        const size_t                 index ) ;
      /** constructor from the function and daughter index 
       *  @param cut    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildPredicate 
      ( const LoKi::PhysTypes::Cuts& cut    , 
        const size_t                 index1 ,
        const size_t                 index2 ) ;
      /** constructor from the function and daughter index 
       *  @param cut    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildPredicate 
      ( const LoKi::PhysTypes::Cuts& cut    , 
        const size_t                 index1 ,
        const size_t                 index2 ,
        const size_t                 index3 ) ;
      /** constructor from the function and daughter index 
       *  @param cut    the function to be used 
       *  @param index  the index of daughter particle
       */
      ChildPredicate 
      ( const LoKi::PhysTypes::Cuts& cut    , 
        const size_t                 index1 ,
        const size_t                 index2 ,
        const size_t                 index3 ,
        const size_t                 index4 ) ;
      /** constructor from the function and daughter index 
       *  @param index  the index of daughter particle
       *  @param cut    the function to be used 
       */
      ChildPredicate
      ( const size_t                 index ,
        const LoKi::PhysTypes::Cuts& cut   ) ;
      /// copy 
      ChildPredicate ( const ChildPredicate& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildPredicate(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildPredicate*  clone() const 
      { return new ChildPredicate(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the function itself 
      LoKi::PhysTypes::Cut m_cut ;                       // the function itself 
      /// index of daughter particle 
      std::vector<size_t> m_index ;               // index of daughter particle 
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES6_H
// ============================================================================
