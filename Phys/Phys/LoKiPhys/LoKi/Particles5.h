// $Id: Particles5.h,v 1.4 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/03/08 14:14:51  ibelyaev
//  add Particles14.h/.cpp
//
// ============================================================================
#ifndef LOKI_PARTICLES5_H 
#define LOKI_PARTICLES5_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================

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
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    /** @class InTree
     *  The trivial predicate which evaluates to true 
     *  if there is at least one particle in the decay 
     *  tree of the given particle which satisfies the 
     *  certain criteria
     *
     *  The function uses the algorithm LoKi::PhysAlgs::found 
     *
     *  @see LoKi::PhysAlgs::found 
     *  @see LoKi::Cuts::INTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class InTree 
      : public LoKi::PhysTypes::Cuts 
    {
    public:
      /** standard constructor 
       *  @param cut cut to be applied 
       */
      InTree  ( const LoKi::PhysTypes::Cuts& cut ) ;
      /// copy constructor 
      InTree  ( const InTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~InTree (){};
      /// MANDATORY: clone function ("virtual constructor")
      virtual  InTree*       clone() const { return new InTree(*this) ;}
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      InTree () ;
    private:
      LoKi::PhysTypes::Cut m_cut ;
    };
    
    /** @class NinTree
     *  The simple funtion which evaluates the number 
     *  of particle in decay tree which satisfies the certan criteria 
     * 
     *  The function uses the algorithm LoKi::PhysAlgs::count_if 
     *
     *  @see LoKi::PhysAlgs::count_if 
     *  @see LoKi::Cuts::NINTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class NinTree 
      : public LoKi::PhysTypes::Func 
    {
    public:
      /** standard constructor 
       *  @param cut cut to be applied 
       */
      NinTree  ( const LoKi::PhysTypes::Cuts& cut ) ;
      /// copy constructor 
      NinTree  ( const NinTree& right ) ;
      /// destructor 
      virtual ~NinTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  NinTree* clone() const { return new NinTree(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      NinTree () ;
    private:
      LoKi::PhysTypes::Cut m_cut ;
    };
    
    /** @class SumTree
     *
     *  The simple function which accumulated the 
     *  value of certain function over the decay 
     *  tree of the particle for the articles which 
     *  satisfy the certain selection criteria
     *  Accumulation is done throught summation
     *
     *  The function uses the algorithm LoKi::PhysAlgs::accumulate
     *
     *  @see LoKi::PhysAlgs::accumulate
     *  @see LoKi::Cuts::SUMTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class SumTree
      : public LoKi::PhysTypes::Func 
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      SumTree
      ( const LoKi::PhysTypes::Func& fun       ,
        const LoKi::PhysTypes::Cuts& cut       ,
        const double                 res = 0.0 ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      SumTree
      ( const LoKi::PhysTypes::Cuts& cut       ,
        const LoKi::PhysTypes::Func& fun       ,
        const double                 res = 0.0 ) ;
      /// copy constructor 
      SumTree ( const SumTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SumTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  SumTree* clone() const { return new SumTree(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      SumTree() ;
    private:
      LoKi::PhysTypes::Fun m_fun ;
      LoKi::PhysTypes::Cut m_cut ;
      double               m_res ;
    };

    /** @class MultTree
     *
     *  The simple function which accumulated the 
     *  value of certain function over the decay 
     *  tree of the particle for the articles which 
     *  satisfy the certain selection criteria
     *  Accumulation is done throught summation
     *
     *  The function uses the algorithm LoKi::PhysAlgs::multiplicate
     *
     *  @see LoKi::PhysAlgs::multuplicate
     *  @see LoKi::Cuts::MULTTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MultTree
      : public LoKi::PhysTypes::Func 
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MultTree
      ( const LoKi::PhysTypes::Func& fun       ,
        const LoKi::PhysTypes::Cuts& cut       ,
        const double                 res = 0.0 ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MultTree
      ( const LoKi::PhysTypes::Cuts& cut       ,
        const LoKi::PhysTypes::Func& fun       ,
        const double                 res = 0.0 ) ;
      /// copy constructor 
      MultTree ( const MultTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MultTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MultTree* clone() const { return new MultTree(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MultTree() ;
    private:
      LoKi::PhysTypes::Fun m_fun ;
      LoKi::PhysTypes::Cut m_cut ;
      double               m_res ;
    };

    /** @class MinTree
     *
     *  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  The function uses the algorithm LoKi::PhysAlgs::min_value 
     *
     *  @see LoKi::PhysAlgs::min_value  
     *  @see LoKi::Cuts::MINTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MinTree
      : public LoKi::PhysTypes::Func 
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MinTree
      ( const LoKi::PhysTypes::Func& fun  ,
        const LoKi::PhysTypes::Cuts& cut  ,
        const double                 res  = 
        LoKi::Constants::PositiveInfinity ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MinTree
      ( const LoKi::PhysTypes::Cuts& cut ,
        const LoKi::PhysTypes::Func& fun ,
        const double                 res = 
        LoKi::Constants::PositiveInfinity ) ;
      /// copy constructor 
      MinTree ( const MinTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MinTree* clone() const { return new MinTree(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MinTree() ;
    private:
      LoKi::PhysTypes::Fun m_fun ;
      LoKi::PhysTypes::Cut m_cut ;
      double               m_res ;
    };

    /** @class MinTree
     *
     *  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  The function uses the algorithm LoKi::PhysAlgs::max_value 
     *
     *  @see LoKi::PhysAlgs::max_value  
     *  @see LoKi::Cuts::MAXTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MaxTree
      : public LoKi::PhysTypes::Func 
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MaxTree
      ( const LoKi::PhysTypes::Func& fun  ,
        const LoKi::PhysTypes::Cuts& cut  ,
        const double                 res  = 
        LoKi::Constants::NegativeInfinity ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param res initial value 
       */
      MaxTree
      ( const LoKi::PhysTypes::Cuts& cut ,
        const LoKi::PhysTypes::Func& fun ,
        const double                 res = 
        LoKi::Constants::NegativeInfinity ) ;
      /// copy constructor 
      MaxTree ( const MaxTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MaxTree* clone() const { return new MaxTree(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MaxTree() ;
    private:
      LoKi::PhysTypes::Fun m_fun ;
      LoKi::PhysTypes::Cut m_cut ;
      double               m_res ;
    };


  }  // end of namespace LoKi::Particles
} // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES4_H
// ============================================================================
