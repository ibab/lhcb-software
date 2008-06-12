// $Id: TupleDicts.cpp,v 1.1 2008-06-12 08:28:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TupleDicts.h"
// ============================================================================
/** @file
 *  Implementation file for class : TupleDicts
 *  @date 2008-06-11 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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

 
// ============================================================================
// The END 
// ============================================================================
