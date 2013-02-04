// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <climits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Time.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Counters.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/ODIN.h"
#include "Event/RecHeader.h"
#include "Event/RecSummary.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TupleDicts.h"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Dicts::TupleArray 
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
 *  @date 2008-06-11 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/*  put one variable size array into n-Tuple 
 *  @param tuple the reference to the -tuple 
 *  @param name the column name 
 *  @param fun  the functor 
 *  @param range the data source 
 *  @param len the name of "length" column
 *  @param maxlen the maximal length 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayp 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name   , 
  const LoKi::Types::Func&  fun    , 
  const LoKi::Types::Range& range  , 
  const std::string&        len    , 
  const size_t              maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name           , fun          , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/* put one variable size array into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayp 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name1  , 
  const LoKi::Types::Func&  fun1   , 
  const std::string&        name2  , 
  const LoKi::Types::Func&  fun2   , 
  const LoKi::Types::Range& range  , 
  const std::string&        len    , 
  const size_t              maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/*  put three variable size arrays into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayp 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name1  , 
  const LoKi::Types::Func&  fun1   , 
  const std::string&        name2  , 
  const LoKi::Types::Func&  fun2   , 
  const std::string&        name3  , 
  const LoKi::Types::Func&  fun3   , 
  const LoKi::Types::Range& range  , 
  const std::string&        len    , 
  const size_t              maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      name3          , fun3         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/*  put four variable size arrays into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayp 
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
  const size_t              maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      name3          , fun3         , 
      name4          , fun4         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/*  put one variable size array into n-Tuple 
 *  @param tuple the reference to the -tuple 
 *  @param name the column name 
 *  @param fun  the functor 
 *  @param range the data source 
 *  @param len the name of "length" column
 *  @param maxlen the maximal length 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayv 
( const Tuples::Tuple&       tuple  , 
  const std::string&         name   , 
  const LoKi::Types::VFunc&  fun    , 
  const LoKi::Types::VRange& range  , 
  const std::string&         len    , 
  const size_t               maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name           , fun          , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/* put one variable size array into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayv 
( const Tuples::Tuple&       tuple  , 
  const std::string&         name1  , 
  const LoKi::Types::VFunc&  fun1   , 
  const std::string&         name2  , 
  const LoKi::Types::VFunc&  fun2   , 
  const LoKi::Types::VRange& range  , 
  const std::string&         len    , 
  const size_t               maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/*  put three variable size arrays into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayv 
( const Tuples::Tuple&       tuple  , 
  const std::string&         name1  , 
  const LoKi::Types::VFunc&  fun1   , 
  const std::string&         name2  , 
  const LoKi::Types::VFunc&  fun2   , 
  const std::string&         name3  , 
  const LoKi::Types::VFunc&  fun3   , 
  const LoKi::Types::VRange& range  , 
  const std::string&         len    , 
  const size_t               maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      name3          , fun3         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
/*  put four variable size arrays into n-Tuple 
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
// ============================================================================
StatusCode LoKi::Dicts::TupleArray::farrayv 
( const Tuples::Tuple&       tuple  , 
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
  const size_t               maxlen ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> farray
    ( name1          , fun1         , 
      name2          , fun2         , 
      name3          , fun3         , 
      name4          , fun4         , 
      range.begin () , range.end () , 
      len            , maxlen       ) ; 
}
// ============================================================================
// LoKi::Dicts::TupleAux 
// ============================================================================
/*  add "time" into n-tuple 
 *  @param tuple the reference to the -tuple 
 *  @param name the column name 
 *  @param time the time 
 *  @return status code 
 *  @see Gaudi::Time 
 */
// ============================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name   , 
  const Gaudi::Time&        time   ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  // 
  StatusCode sc  = StatusCode::SUCCESS ;
  //
  sc = tuple->column ( name + "year"    , time.year    ( true )     , 1970 , 2070 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "month"   , time.month   ( true ) + 1 , 1    ,   13 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "day"     , time.day     ( true )     , 0    ,   32 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "hour"    , time.hour    ( true )     , 0    ,   25 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "minute"  , time.minute  ( true )     , 0    ,   61 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "second"  , time.second  ( true )     , 0    ,   61 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple->column ( name + "nsecond" , time.nsecond ()     ) ;
  //
  return sc ;
}
// ============================================================================
/*  add "time" into n-tuple 
 *  @param tuple the reference to the -tuple 
 *  @param time the time 
 *  @return status code 
 *  @see Gaudi::Time 
 */
// ============================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const Gaudi::Time&        time   ) { return column( tuple ,"" , time ) ; }
// ============================================================================
/*  add basic ODIN  into n-tuple:
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
// ============================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name   , 
  const LHCb::ODIN*         odin   ) 
{
  //
  if ( 0 == odin || !tuple.valid() ) { return StatusCode::FAILURE ; }  
  // 
  StatusCode sc  = StatusCode::SUCCESS ;
  //
  sc = tuple -> column ( name + "run"     , odin ->   runNumber             () ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple -> column ( name + "evt"     , odin -> eventNumber             () ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple -> column ( name + "tck"     , odin -> triggerConfigurationKey () ) ;
  if ( sc.isFailure() ) { return sc ; }
  //
  sc = tuple -> column ( name + "bxtype"  , odin -> bunchCrossingType () , 0 , 4 ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple -> column ( name + "trgtype" , odin -> triggerType       () , 0 , 8 ) ;
  if ( sc.isFailure() ) { return sc ; }
  //
  return column ( tuple , name , odin->eventTime() ) ;
} 
// ============================================================================
/*  add basic ODIN  into n-tuple:
 *  - run&evt numebrs 
 *  - tck
 *  - bxtype & trigger types
 *  - event time 
 *  @param tuple the reference to the -tuple 
 *  @param odin ODIN-object
 *  @return status code 
 *  @see LHCb::ODIN
 */
// ============================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const LHCb::ODIN*         odin   ) { return column ( tuple , "" , odin ) ; }
// ===========================================================================
/*  add basic RecHeader information 
 *  - run&evt numebrs 
 *  - event time 
 *  @param tuple the reference to the -tuple 
 *  @param name the column name 
 *  @param hdr  run header 
 *  @return status code 
 *  @see LHCb::RecHeader 
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const std::string&        name   , 
  const LHCb::RecHeader*    hdr    ) 
{
  if ( 0 == hdr || !tuple.valid() ) { return StatusCode::FAILURE ; }  
  //
  StatusCode sc = StatusCode::SUCCESS ;
  //
  sc = tuple -> column ( name + "run"     , hdr -> runNumber  () ) ;
  if ( sc.isFailure() ) { return sc ; }
  sc = tuple -> column ( name + "evt"     , hdr -> evtNumber  () ) ;
  if ( sc.isFailure() ) { return sc ; }
  //
  return column ( tuple , name , Gaudi::Time ( hdr -> gpsTime ()  * 1000  ) ) ;
}
// ===========================================================================
/*  add basic RecHeader information 
 *  - run&evt numebrs 
 *  - event time 
 *  @param tuple the reference to the -tuple 
 *  @param hdr  run header 
 *  @return status code 
 *  @see LHCb::RecHeader 
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple  , 
  const LHCb::RecHeader*    hdr    ) { return column ( tuple , "" , hdr ) ; }
// ===========================================================================
/*  add basic RecSummary information 
 *  @param tuple the reference to the -tuple 
 *  @param name  the column name 
 *  @param sum   the rec-summary object  
 *  @return status code 
 *  @see LHCb::RecSummary 
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple   , 
  const std::string&        name    , 
  const LHCb::RecSummary*   sum     ) 
{
  if ( 0 == sum || !tuple.valid() ) { return StatusCode::FAILURE ; }  
  //
  return  tuple -> fmatrix ( name +  "summary"      , 
                             sum  ->  summaryData() , 
                             name + "nSummary"      , 200 ) ;
  //
}
// ===========================================================================
/*  add basic RecSummary information 
 *  @param tuple the reference to the -tuple 
 *  @param sum   the rec-summary object  
 *  @return status code 
 *  @see LHCb::RecSummary 
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple   , 
  const LHCb::RecSummary*   sum     ) { return column ( tuple , "" , sum  ) ; } 
// ===========================================================================
/*  add basic information from counters   { name : value }
 *  @param tuple the reference to the -tuple 
 *  @param name the column name 
 *  @param cnts the counters 
 *  @return status code 
 *  @see Gaudi::Numbers
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple   , 
  const std::string&        name    , 
  const Gaudi::Numbers*     cnts    ) 
{
  if ( 0 == cnts || !tuple.valid() ) { return StatusCode::FAILURE ; }  
  //
  const Gaudi::Numbers::Map& m = cnts -> numbers () ;
  StatusCode sc = tuple->column ( name + "numbers" , cnts->size() ) ;
  //
  for ( Gaudi::Numbers::Map::const_iterator i = m.begin() ; m.end() != i ; ++i ) 
  {
    StatusCode sc = tuple->column ( name + "num_" + i->first , i->second ) ;
    if ( sc.isFailure() ) { return sc ; }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  add basic information from counters   { name : value }
 *  @param tuple the reference to the -tuple 
 *  @param cnts the counters 
 *  @return status code 
 *  @see Gaudi::Numbers
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column 
( const Tuples::Tuple&      tuple   , 
  const Gaudi::Numbers*     cnts    ) { return column ( tuple , "" , cnts ) ; }
// ===========================================================================
/*  add Tis/Tos/Tobbing information 
 *  @param tuple the tuple itself 
 *  @param name the column name 
 *  @param TisTosTob-infomration
 *  @param verbose use full information
 *  @return status code 
 *  @see ITisTos::TisTosTob 
 */
// ===========================================================================
StatusCode LoKi::Dicts::TupleAux::column
( const Tuples::Tuple&      tuple   , 
  const std::string&        name    , 
  const ITisTos::TisTosTob& ttt     ,
  const bool                verbose ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  //
  StatusCode sc = tuple -> column ( name , ttt.value () , 0u , 15u ) ;
  //
  if ( !verbose || sc.isFailure() ) { return sc ; }          // RETURN 
  //
  sc = tuple -> column ( name + "_tos" , ttt.tos      () ) ;
  if ( sc.isFailure() )          { return sc ; }             // RETURN 
  sc = tuple -> column ( name + "_tis" , ttt.tis      () ) ;
  if ( sc.isFailure() )          { return sc ; }             // RETURN 
  sc = tuple -> column ( name + "_tps" , ttt.tps      () ) ;
  if ( sc.isFailure() )          { return sc ; }             // RETURN 
  sc = tuple -> column ( name + "_tus" , ttt.tus      () ) ;
  if ( sc.isFailure() )          { return sc ; }             // RETURN 
  sc = tuple -> column ( name + "_dec" , ttt.decision () ) ;
  if ( sc.isFailure() )          { return sc ; }             // RETURN 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// store boolean value in n-tuple 
// ============================================================================
StatusCode LoKi::Dicts::TupleBasic::column_bool  
( const Tuples::Tuple&      tuple , 
  const std::string&        name  , 
  const bool                value )
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> column ( name , value ) ;  
}
// ============================================================================
namespace 
{
  // ==========================================================================
  const double s_float_max =  0.99 * std::numeric_limits<float>::max() ;
  const double s_float_min =  0.99 * std::numeric_limits<float>::min() ;
  // ==========================================================================
}
// ============================================================================
// store float  value in n-tuple
// ============================================================================
StatusCode LoKi::Dicts::TupleBasic::column_float  
( const Tuples::Tuple&      tuple , 
  const std::string&        name  , 
  const double              value )
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  //
  const float _value = float ( std::max ( std::min ( value , s_float_max ) , s_float_min ) ) ;
  //
  return tuple -> column ( name , _value ) ;  
}
// ============================================================================
// store double value in n-tuple
// ============================================================================
StatusCode LoKi::Dicts::TupleBasic::column_double
( const Tuples::Tuple&      tuple , 
  const std::string&        name  , 
  const double              value )
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> column ( name , value ) ;  
}
// ============================================================================
// store int value in n-tuple 
// ============================================================================
StatusCode LoKi::Dicts::TupleBasic::column_int 
( const Tuples::Tuple&      tuple , 
  const std::string&        name  , 
  const int                 value ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> column ( name , value ) ;  
}
// ============================================================================
// store long value in n-tuple 
// ============================================================================
StatusCode LoKi::Dicts::TupleBasic::column_long
( const Tuples::Tuple&      tuple , 
  const std::string&        name  , 
  const long                value ) 
{
  if ( !tuple.valid() ) { return StatusCode::FAILURE ; }  
  return tuple -> column ( name , value ) ;  
}

// ============================================================================
// The END 
// ============================================================================
