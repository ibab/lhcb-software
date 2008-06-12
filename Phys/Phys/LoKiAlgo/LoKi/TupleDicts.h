// $Id: TupleDicts.h,v 1.1 2008-06-12 08:28:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TUPLEDICTS_H 
#define LOKI_TUPLEDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/Tuple.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  { 
    // ========================================================================
    /** @class TupleArray 
     *  helper class to decorate the "farray" method for the standard Tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-11
     */
    class TupleArray
    {
    public:
      // ======================================================================
      /** put one variable size array into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param fun  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayp 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name   , 
        const LoKi::Types::Func&  fun    , 
        const LoKi::Types::Range& range  , 
        const std::string&        len    , 
        const size_t              maxlen ) ;
      // ======================================================================
      /** put two variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayp 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name1  , 
        const LoKi::Types::Func&  fun1   , 
        const std::string&        name2  , 
        const LoKi::Types::Func&  fun2   , 
        const LoKi::Types::Range& range  , 
        const std::string&        len    , 
        const size_t              maxlen ) ;
      // ======================================================================
      /** put three variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param name3 the column name 
       *  @param fun3  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayp 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name1  , 
        const LoKi::Types::Func&  fun1   , 
        const std::string&        name2  , 
        const LoKi::Types::Func&  fun2   , 
        const std::string&        name3  , 
        const LoKi::Types::Func&  fun3   , 
        const LoKi::Types::Range& range  , 
        const std::string&        len    , 
        const size_t              maxlen ) ;
      // ======================================================================
      /** put four variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param name3 the column name 
       *  @param fun3  the functor 
       *  @param name4 the column name 
       *  @param fun4  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayp 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name1  , 
        const LoKi::Types::Func&  fun1   , 
        const std::string&        name2  , 
        const LoKi::Types::Func&  fun2   , 
        const std::string&        name3  , 
        const LoKi::Types::Func&  fun3   , 
        const std::string&        name4  , 
        const LoKi::Types::Func&  fun4   , 
        const LoKi::Types::Range& range  , 
        const std::string&        len    , 
        const size_t              maxlen ) ;
      // ======================================================================
    public:
      // ======================================================================
      /** put one variable size array into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param fun  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayv 
      ( const Tuples::Tuple&       tuple  , 
        const std::string&         name   , 
        const LoKi::Types::VFunc&  fun    , 
        const LoKi::Types::VRange& range  , 
        const std::string&         len    , 
        const size_t               maxlen ) ;
      // ======================================================================
      /** put two variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayv 
      ( const Tuples::Tuple&       tuple  , 
        const std::string&         name1  , 
        const LoKi::Types::VFunc&  fun1   , 
        const std::string&         name2  , 
        const LoKi::Types::VFunc&  fun2   , 
        const LoKi::Types::VRange& range  , 
        const std::string&         len    , 
        const size_t               maxlen ) ;
      // ======================================================================
      /** put three variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param name3 the column name 
       *  @param fun3  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayv 
      ( const Tuples::Tuple&       tuple  , 
        const std::string&         name1  , 
        const LoKi::Types::VFunc&  fun1   , 
        const std::string&         name2  , 
        const LoKi::Types::VFunc&  fun2   , 
        const std::string&         name3  , 
        const LoKi::Types::VFunc&  fun3   , 
        const LoKi::Types::VRange& range  , 
        const std::string&         len    , 
        const size_t               maxlen ) ;
      // ======================================================================
      /** put four variable size arrays into n-Tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name1 the column name 
       *  @param fun1  the functor 
       *  @param name2 the column name 
       *  @param fun2  the functor 
       *  @param name3 the column name 
       *  @param fun3  the functor 
       *  @param name4 the column name 
       *  @param fun4  the functor 
       *  @param range the data source 
       *  @param len the name of "length" column
       *  @param maxlen the maximal length 
       *  @return status code 
       */
      static StatusCode farrayv 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&         name1  , 
        const LoKi::Types::VFunc&  fun1   , 
        const std::string&         name2  , 
        const LoKi::Types::VFunc&  fun2   , 
        const std::string&         name3  , 
        const LoKi::Types::VFunc&  fun3   , 
        const std::string&         name4  , 
        const LoKi::Types::VFunc&  fun4   , 
        const LoKi::Types::VRange& range  , 
        const std::string&         len    , 
        const size_t               maxlen ) ;
      // ======================================================================
    };
    // ========================================================================
  } //end of namespace LoKi:Dicts 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TUPLEDICTS_H
// ============================================================================
