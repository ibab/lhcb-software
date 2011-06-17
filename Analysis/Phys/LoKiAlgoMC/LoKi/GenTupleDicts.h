// $Id$
// ============================================================================
#ifndef LOKI_GENTUPLEDICTS_H 
#define LOKI_GENTUPLEDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
#ifdef __INTEL_COMPILER
// non-pointer conversion from "double" to "float" may lose significant bits
#pragma warning(disable:2259) 
#endif
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/Tuple.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  { 
    // ========================================================================
    /** @class GenTupleArray 
     *  helper class to decorate the "farray" method for the standard Tuple
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-02-13
     */
    class GenTupleArray
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
      ( const Tuples::Tuple&        tuple  , 
        const std::string&          name   , 
        const LoKi::Types::GFunc&   fun    , 
        const LoKi::Types::GRange&  range  , 
        const std::string&          len    , 
        const size_t                maxlen ) ;
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
      ( const Tuples::Tuple&        tuple  , 
        const std::string&          name1  , 
        const LoKi::Types::GFunc&   fun1   , 
        const std::string&          name2  , 
        const LoKi::Types::GFunc&   fun2   , 
        const LoKi::Types::GRange&  range  , 
        const std::string&          len    , 
        const size_t                maxlen ) ;
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
      ( const Tuples::Tuple&        tuple  , 
        const std::string&          name1  , 
        const LoKi::Types::GFunc&   fun1   , 
        const std::string&          name2  , 
        const LoKi::Types::GFunc&   fun2   , 
        const std::string&          name3  , 
        const LoKi::Types::GFunc&   fun3   , 
        const LoKi::Types::GRange&  range  , 
        const std::string&          len    , 
        const size_t                maxlen ) ;
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
      ( const Tuples::Tuple&        tuple  , 
        const std::string&          name1  , 
        const LoKi::Types::GFunc&   fun1   , 
        const std::string&          name2  , 
        const LoKi::Types::GFunc&   fun2   , 
        const std::string&          name3  , 
        const LoKi::Types::GFunc&   fun3   , 
        const std::string&          name4  , 
        const LoKi::Types::GFunc&   fun4   , 
        const LoKi::Types::GRange&  range  , 
        const std::string&          len    , 
        const size_t                maxlen ) ;
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi:Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GENTUPLEDICTS_H
// ============================================================================
