// ============================================================================
#ifndef LOKI_DUMPER_H 
#define LOKI_DUMPER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/Primitives.h"
#include "LoKi/Dump.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    /** @class Dump1_ 
     *  simple class to simplify the monitoring of functors 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    template <class TYPE1,class TYPE2>
    class Dump1_ : public LoKi::Functor<TYPE1,TYPE2>
    {
    private: 
      // ======================================================================
      typedef LoKi::Functor<TYPE1,TYPE2>                               Fun_ ;
      typedef typename Fun_::argument                              argument ;
      typedef typename Fun_::result_type                        result_type ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor 
       *  @param fun   the functor 
       *  @param open  the opening 
       *  @param clone the closing
       *  @param stream the stream 
       */
      Dump1_ ( const LoKi::Functor<TYPE1,TYPE2>& fun                , 
               const std::string&                open   = ""        ,
               const std::string&                close  = "\n"      ,
               const bool                        right  = true      ,
               std::ostream&                     stream = std::cout ) 
        : LoKi::AuxFunBase ( std::tie ( fun , open , close , right ) ) 
        , LoKi::Functor<TYPE1,TYPE2>() 
        , m_fun    ( fun    )
        , m_right  ( right  ) 
        , m_stream ( stream ) 
        , m_dump   ( open  , close ) 
      {}    
      /** constructor from the dumper 
       *  @param fun   the functor 
       *  @param dump  the dumper 
       *  @param stream the stream 
       */
      Dump1_ ( const LoKi::Functor<TYPE1,TYPE2>& fun                , 
               const LoKi::Dump&                 dump               ,
               const bool                        right  = true      ,
               std::ostream&                     stream = std::cout ) 
        : LoKi::AuxFunBase ( std::tie ( fun , dump , right ) ) 
        , LoKi::Functor<TYPE1,TYPE2>() 
        , m_fun    ( fun    )
        , m_right  ( right  ) 
        , m_stream ( stream ) 
        , m_dump   ( dump   ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Dump1_() {}
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  Dump1_* clone() const { return new Dump1_ ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;     
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// the default constructor is disabled 
      Dump1_ () ;                        // the default constructor is disabled 
      // ======================================================================
    private :
      // ======================================================================
      /// the functor 
      typename LoKi::Assignable<Fun_>::Type  m_fun    ;
      /// right-action ?
      bool                                   m_right  ; // right action?
      /// the stream 
      std::ostream&                          m_stream ; // the stream 
      /// the dumper 
      LoKi::Dump                             m_dump   ; // the dumper 
      // ======================================================================
    } ;
    // ========================================================================
    template <class TYPE1,class TYPE2>
    typename LoKi::Functor<TYPE1,TYPE2>::result_type 
    LoKi::Functors::Dump1_<TYPE1,TYPE2>::operator() 
      ( typename LoKi::Functor<TYPE1,TYPE2>::argument a ) const 
    {
      if ( this->m_right ) 
      {
        this -> m_stream << this -> m_dump.open  () ;
        Gaudi::Utils::toStream ( a , this->m_stream ) ;
        this -> m_stream << this -> m_dump.close () ;
        //
        return this->m_fun.fun ( a ) ;
      }
      //
      typename LoKi::Functor<TYPE1,TYPE2>::result_type res = this->m_fun.fun ( a ) ;
      //
      this -> m_stream << this -> m_dump.open  () ;
      Gaudi::Utils::toStream ( a , this->m_stream ) ;
      this -> m_stream << this -> m_dump.close () ;
      //
      return res ;
    }
    // ========================================================================
    template <class TYPE1,class TYPE2>
    std::ostream& 
    LoKi::Functors::Dump1_<TYPE1,TYPE2>::fillStream ( std::ostream& s ) const  
    {
      s << " dump1 ( " << this->m_fun ;
      if ( !this -> m_right ) { s << " ,False " ; }
      return s << " )";
    }    
    // ========================================================================
    template <>
    Dump1_<double,bool>::result_type 
    Dump1_<double,bool>::operator() 
      ( Dump1_<double,bool>::argument a ) const ;
    // ========================================================================
    template <>
    Dump1_<double,double>::result_type 
    Dump1_<double,double>::operator() 
      ( Dump1_<double,double>::argument a ) const ;
    // ========================================================================
  } //                                          end of namesapce LoKi::Functors
  // ==========================================================================
  template <class TYPE1,class TYPE2> 
  LoKi::Functors::Dump1_<TYPE1,TYPE2>
  dump1 ( const LoKi::Functor<TYPE1,TYPE2>& fun                  , 
          const bool                        right = true         ,
          const LoKi::Dump&                 dump  = LoKi::Dump() )
  { return LoKi::Functors::Dump1_<TYPE1,TYPE2>( fun , dump , right ) ; }  
  // ==========================================================================
  template <class TYPE2> 
  const LoKi::Functor<void,TYPE2>&
  dump1 ( const LoKi::Functor<void,TYPE2>&    fun      , 
          const bool                       /* right */ ,
          const LoKi::Dump&                /* dump  */ ) { return fun ; }  
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_DUMPER_H
// ============================================================================
