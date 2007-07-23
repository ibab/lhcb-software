// $Id: FuncCast.h,v 1.5 2007-07-23 17:07:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FUNCCAST_H 
#define LOKI_FUNCCAST_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
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
 *  @date 2006-03-07 
 */
// ============================================================================
namespace LoKi 
{
  namespace Adapters
  {    
    // ========================================================================
    /** @class FuncCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2>
    class FuncCast : public LoKi::Function<TYPE1>
    {
    public: 
      /// Standard constructor
      FuncCast ( const LoKi::Function<TYPE2>& fun ) 
        : LoKi::Function<TYPE1>()
        , m_fun ( fun ) 
      {} ;
      /// copy constructor 
      FuncCast ( const FuncCast& right ) 
        : LoKi::AuxFunBase      ( right )
        , LoKi::Function<TYPE1> ( right ) 
        , m_fun ( right.m_fun ) 
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~FuncCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FuncCast* clone() const { return new FuncCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE1>::result_type 
      operator() ( typename LoKi::Function<TYPE1>::argument a ) const
      {        
        typedef typename LoKi::Function<TYPE2>::argument_type _arg2 ;
        return m_fun( dynamic_cast<_arg2>( a )  ) ;
      } ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Function<TYPE1>::result_type 
      operator() ( typename LoKi::Function<TYPE2>::argument a ) const
      { return m_fun ( a ) ; } ;      
    private:
      /// default constructor is disabled 
      FuncCast();
    private:
      LoKi::FunctionFromFunction<TYPE2> m_fun ;
    } ;
    // ========================================================================    
    /** @class PredCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2>
    class PredCast : public LoKi::Predicate<TYPE1>
    {
    public: 
      /// Standard constructor
      PredCast ( const LoKi::Predicate<TYPE2>& fun ) 
        : LoKi::Predicate<TYPE1>()
        , m_fun ( fun ) 
      {} ;
      /// copy constructor 
      PredCast ( const PredCast& right ) 
        : LoKi::AuxFunBase      ( right )
        , LoKi::Predicate<TYPE1> ( right ) 
        , m_fun ( right.m_fun ) 
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~PredCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PredCast* clone() const { return new PredCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Predicate<TYPE1>::result_type 
      operator() ( typename LoKi::Predicate<TYPE1>::argument a ) const
      {
        typedef typename LoKi::Predicate<TYPE2>::argument_type _arg2 ;
        return m_fun( dynamic_cast<_arg2>( a ) ) ;
      } ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Predicate<TYPE1>::result_type 
      operator() ( typename LoKi::Predicate<TYPE2>::argument a ) const
      { return m_fun ( a ) ; } ;      
    private:
      /// default constructor is disabled 
      PredCast();
    private:
      LoKi::PredicateFromPredicate<TYPE2> m_fun ;
    } ;
    // ========================================================================
    /** @class FuncStaticCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2>
    class FuncStaticCast : public LoKi::Function<TYPE1>
    {
    public: 
      /// Standard constructor
      FuncStaticCast ( const LoKi::Function<TYPE2>& fun ) 
        : LoKi::Function<TYPE1>()
        , m_fun ( fun ) 
      {} ;
      /// copy constructor 
      FuncStaticCast ( const FuncStaticCast& right ) 
        : LoKi::AuxFunBase      ( right )
        , LoKi::Function<TYPE1> ( right ) 
        , m_fun ( right.m_fun ) 
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~FuncStaticCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FuncStaticCast* clone() const 
      { return new FuncStaticCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Function<TYPE1>::result_type 
      operator() ( typename LoKi::Function<TYPE1>::argument a ) const
      {
        typedef typename LoKi::Function<TYPE2>::argument_type _arg2 ;
        return m_fun( static_cast<_arg2>( a ) ) ;
      } ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Function<TYPE1>::result_type 
      operator() ( typename LoKi::Function<TYPE2>::argument a ) const
      { return m_fun ( a ) ; } ;      
    private:
      /// default constructor is disabled 
      FuncStaticCast();
    private:
      LoKi::FunctionFromFunction<TYPE2> m_fun ;
    } ;
    // ========================================================================    
    /** @class PredStaticCast FuncCast.h LoKi/FuncCast.h
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    template <class TYPE1,class TYPE2>
    class PredStaticCast : 
      public LoKi::Predicate<TYPE1>
    {
    public: 
      /// Standard constructor
      PredStaticCast ( const LoKi::Predicate<TYPE2>& fun ) 
        : LoKi::Predicate<TYPE1>()
        , m_fun ( fun ) 
      {} ;
      /// copy constructor 
      PredStaticCast ( const PredStaticCast& right ) 
        : LoKi::AuxFunBase       ( right )
        , LoKi::Predicate<TYPE1> ( right ) 
        , m_fun ( right.m_fun ) 
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~PredStaticCast() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PredStaticCast* clone() const 
      { return new PredStaticCast(*this); }
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Predicate<TYPE1>::result_type 
      operator() ( typename LoKi::Predicate<TYPE1>::argument a ) const
      {
        typedef typename LoKi::Predicate<TYPE2>::argument _arg2 ;
        return m_fun( static_cast<_arg2>( a ) ) ;
      } ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& stream ) const 
      { return stream << m_fun ; }
      /// OPTIONAL: helper method 
      typename LoKi::Predicate<TYPE1>::result_type 
      operator() ( typename LoKi::Predicate<TYPE2>::argument a ) const
      { return m_fun ( a ) ; } ;      
    private:
      /// default constructor is disabled 
      PredStaticCast();
    private:
      LoKi::PredicateFromPredicate<TYPE2> m_fun ;
    } ;
    // ========================================================================
  } // end of the namespace LoKi::Adapters
} // end of the namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCCAST_H
// ============================================================================
