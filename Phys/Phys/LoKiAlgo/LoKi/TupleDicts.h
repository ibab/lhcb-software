// $Id$
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
/** @file LoKi/TupleDicts.h
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
    class GAUDI_API TupleArray
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
  } //                                              end of namespace LoKi:Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  class ODIN       ;
  class RecHeader  ;
  class RecSummary ;
  // ==========================================================================
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  class Numbers ;
  // ==========================================================================
}
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ITisTos.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    /** @class TupleAux
     *  set of helper columns for n-tuple 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-16
     */
    class GAUDI_API TupleAux
    {
    public:
      // ======================================================================
      /** add Tis/Tos/Tobbing information 
       *  @param tuple the tuple itself 
       *  @param name the column name 
       *  @param TisTosTob-infomration
       *  @param verbose use full information
       *  @return status code 
       *  @see ITisTos::TisTosTob 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple           , 
        const std::string&        name            , 
        const ITisTos::TisTosTob& ttt             , 
        const bool                verbose = false ) ;
      // ======================================================================
        /** add "time" into n-tuple 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param time the time 
       *  @return status code 
       *  @see Gaudi::Time 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name   , 
        const Gaudi::Time&        time   ) ;
      // ======================================================================
      /** add "time" into n-tuple 
       *  @param tuple the reference to the -tuple 
       *  @param time the time 
       *  @return status code 
       *  @see Gaudi::Time 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const Gaudi::Time&        time   ) ;
      // ======================================================================
      /** add basic ODIN  into n-tuple:
       *  - run&evt numebrs 
       *  - tck
       *  - bxtype & trigger types
       *  - event time 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param odin ODIN-object
       *  @return status code 
       *  @see LHCb::ODIN
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name   , 
        const LHCb::ODIN*         odin   ) ;
      // ======================================================================
      /** add basic ODIN  into n-tuple:
       *  - run&evt numebrs 
       *  - tck
       *  - bxtype & trigger types
       *  - event time 
       *  @param tuple the reference to the -tuple 
       *  @param odin ODIN-object
       *  @return status code 
       *  @see LHCb::ODIN
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const LHCb::ODIN*         odin   ) ;
      // ======================================================================
      /** add basic RecHeader information 
       *  - run&evt numebrs 
       *  - event time 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param hdr  run header 
       *  @return status code 
       *  @see LHCb::RecHeader 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const std::string&        name   , 
        const LHCb::RecHeader*    hdr    ) ;
      // ======================================================================
      /** add basic RecHeader information 
       *  - run&evt numebrs 
       *  - event time 
       *  @param tuple the reference to the -tuple 
       *  @param hdr  run header 
       *  @return status code 
       *  @see LHCb::RecHeader 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple  , 
        const LHCb::RecHeader*    hdr    ) ;
      // ======================================================================
      /** add basic RecSummary information 
       *  @param tuple the reference to the -tuple 
       *  @param name    the column name 
       *  @param summary the rec-summary object  
       *  @return status code 
       *  @see LHCb::RecSummary 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple   , 
        const std::string&        name    , 
        const LHCb::RecSummary*   summary ) ;
      // ======================================================================
      /** add basic RecSummary information 
       *  @param tuple the reference to the -tuple 
       *  @param summary the rec-summary object  
       *  @return status code 
       *  @see LHCb::RecSummary 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple   , 
        const LHCb::RecSummary*   summary ) ;
      // ======================================================================
      /** add basic information from counters  { name : value } 
       *  @param tuple the reference to the -tuple 
       *  @param name the column name 
       *  @param cnts the counters 
       *  @return status code 
       *  @see Gaudi::Numbers 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple   , 
        const std::string&        name    , 
        const Gaudi::Numbers*     cnts    ) ;
      // ======================================================================
      /** add basic information from counters  { name : value } 
       *  @param tuple the reference to the -tuple 
       *  @param cnts the counters 
       *  @return status code 
       *  @see Gaudi::Numbers 
       */
      static StatusCode column 
      ( const Tuples::Tuple&      tuple   , 
        const Gaudi::Numbers*     cnts    ) ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TupleBasic 
     *  set of helper columns for n-tuple 
     *  @toto try to move it to Gaudi
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-16
     */
    class GAUDI_API TupleBasic
    {
    public:
      // ======================================================================
      /// store boolean value in n-tuple 
      static StatusCode column_bool  
      ( const Tuples::Tuple&      tuple , 
        const std::string&        name  , 
        const bool                value ) ;
      /// store float  value in n-tuple 
      static StatusCode column_float  
      ( const Tuples::Tuple&      tuple , 
        const std::string&        name  , 
        const double              value ) ;
      /// store float  value in n-tuple 
      static StatusCode column_double  
      ( const Tuples::Tuple&      tuple , 
        const std::string&        name  , 
        const double              value ) ;
      /// store int value in n-tuple 
      static StatusCode column_int   
      ( const Tuples::Tuple&      tuple , 
        const std::string&        name  , 
        const int                 value ) ;
      /// store int value in n-tuple 
      static StatusCode column_long 
      ( const Tuples::Tuple&      tuple , 
        const std::string&        name  , 
        const long                value ) ;
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi:Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================

// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TUPLEDICTS_H
// ============================================================================
