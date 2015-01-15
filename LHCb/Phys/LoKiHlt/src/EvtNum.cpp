// $Id: $
// =============================================================================
// Include files 
// =============================================================================
// STD & STL
// =============================================================================
#include <iostream>
#include <set>
#include <algorithm>
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/Hash.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/ParsersFactory.h"
// =============================================================================
// local
// =============================================================================
#include "LoKi/EvtNum.h"
#include "LoKi/ToCpp.h"
// =============================================================================
/** @file 
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2012-02-13
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */ 
// =============================================================================
namespace Gaudi 
{ 
  // ===========================================================================
  namespace  Parsers 
  {
    // =========================================================================
    template< typename Iterator, typename Skipper>
    struct EvtNumGrammar 
      : qi::grammar<Iterator, LoKi::Numbers::EvtNum(), Skipper>
    {
      // ======================================================================
      typedef LoKi::Numbers::EvtNum ResultT;
      EvtNumGrammar() 
        : EvtNumGrammar::base_type( integer ) 
      { integer = qi::int_parser<unsigned long long>(); }
        // ====================================================================
        qi::rule<Iterator, LoKi::Numbers::EvtNum(), Skipper> integer;
        // 
    };
    // ========================================================================
    template< typename Iterator, typename Skipper>
    struct RunEvtGrammar 
      : public PairGrammar<Iterator,LoKi::Numbers::RunEvt,Skipper>
    {
    };
    // =========================================================================
    REGISTER_GRAMMAR(LoKi::Numbers::EvtNum, EvtNumGrammar) ;
    REGISTER_GRAMMAR(LoKi::Numbers::RunEvt, RunEvtGrammar) ;
    // =========================================================================
  }
  // ===========================================================================
}
// =============================================================================
// constructor 
// =============================================================================
LoKi::Numbers::EvtNum::EvtNum 
( LoKi::Numbers::EvtNum::evt_type e ) 
  : m_evtnum ( e ) {}
// =============================================================================
// Hash 
// =============================================================================
std::size_t LoKi::Numbers::EvtNum::hash() const 
{ return boost::hash_value ( m_evtnum ) ; }
// =============================================================================
// printout 
// =============================================================================
std::ostream& 
LoKi::Numbers::EvtNum::fillStream( std::ostream& s ) const
{ return s << m_evtnum ; }
// =============================================================================
std::string LoKi::Numbers::EvtNum::toString() const
{
  std::ostringstream s;
  fillStream ( s ) ;
  return s.str();
}
// =============================================================================
std::string LoKi::Numbers::EvtNum::toCpp() const 
{ return "LoKi::Numbers::EvtNum(" + toString() + ")" ; }  
// =============================================================================
//  the streamer 
// =============================================================================
std::ostream& Gaudi::Utils::toStream 
( const LoKi::Numbers::EvtNum& evt , 
  std::ostream&                s   ) { return evt.fillStream ( s )  ; }
// =============================================================================
//  the streamer 
// =============================================================================
std::string   Gaudi::Utils::toString
( const LoKi::Numbers::EvtNum& evt ) { return evt.toString ()  ; }
// =============================================================================
// parser 
// =============================================================================
StatusCode 
Gaudi::Parsers::parse 
( LoKi::Numbers::EvtNum& result, const std::string& input)
{ return parse_ ( result , input ) ; }
// =============================================================================
// EvtNumList 
// =============================================================================
LoKi::Numbers::EvtNumList::EvtNumList
( const LoKi::Numbers::EvtNumList::evt_list& lst ) 
  : m_list ( lst ) 
{
  std::sort ( m_list.begin() , m_list.end() ) ;
  auto i = std::unique( m_list.begin() , m_list.end() ) ;
  m_list.erase ( i ,  m_list.end() ) ;
}
// =============================================================================
// EvtNumList 
// =============================================================================
LoKi::Numbers::EvtNumList::EvtNumList
( const LoKi::Numbers::EvtNum& evt ) 
  : m_list ( 1 , evt ) 
{}
// ============================================================================
// append 
// ============================================================================
LoKi::Numbers::EvtNumList
LoKi::Numbers::EvtNumList::operator+
( const LoKi::Numbers::EvtNum& e ) const 
{
  //
  if ( empty() ) { return EvtNumList ( evt_list( 1, e ) ) ; }
  //
  evt_list lst  ( begin() , end() ) ;
  lst.push_back ( e ) ;
  //
  return EvtNumList ( lst ) ;
}
// ============================================================================
// append 
// ============================================================================
LoKi::Numbers::EvtNumList
LoKi::Numbers::EvtNumList::operator+
( const LoKi::Numbers::EvtNumList& e ) const
{
  if      ( e.empty()  ) { return *this ; }
  else if (   empty()  ) { return     e ; }
  else if ( &e == this ) { return *this ; }
  //
  evt_list lst ;
  lst.reserve ( size() + e.size() );
  lst.insert  ( lst.end() ,   begin() ,   end () ) ;
  lst.insert  ( lst.end() , e.begin() , e.end () ) ;
  //
  return EvtNumList ( lst ) ;
}
// ============================================================================
// hash 
// ============================================================================
std::size_t LoKi::Numbers::EvtNumList::hash       () const 
{
  return std::accumulate( begin(), end(), std::size_t{0},
                          [](std::size_t seed, const LoKi::Numbers::EvtNum& i ) {
                              boost::hash_combine(seed,i);
                              return seed;
                          } );
}
// ==============================================================================
std::ostream& LoKi::Numbers::EvtNumList::fillStream ( std::ostream& s ) const 
{ return Gaudi::Utils::toStream ( m_list , s ) ; }
// ==============================================================================
bool LoKi::Numbers::EvtNumList::contains 
( const LoKi::Numbers::EvtNum& e ) const 
{ return std::binary_search ( begin() , end () , e ) ; }
// ==============================================================================
std::string   LoKi::Numbers::EvtNumList::toString   () const 
{ 
  std::ostringstream s;
  fillStream ( s ) ;
  return s.str();
}
// =============================================================================
std::string LoKi::Numbers::EvtNumList::toCpp() const 
{ return "LoKi::Numbers::EvtNumList(" + Gaudi::Utils::toCpp ( m_list ) + ")" ; }  
// =============================================================================
//  the streamer 
// =============================================================================
std::ostream& Gaudi::Utils::toStream 
( const LoKi::Numbers::EvtNumList& evt , 
  std::ostream&                    s   ) { return evt.fillStream ( s )  ; }
// =============================================================================
//  the streamer 
// =============================================================================
std::string   Gaudi::Utils::toString
( const LoKi::Numbers::EvtNumList& evt ) { return evt.toString ()  ; }
// =============================================================================
// parser 
// =============================================================================
StatusCode 
Gaudi::Parsers::parse 
( LoKi::Numbers::EvtNumList& result, const std::string& input)
{
  //
  typedef std::vector<LoKi::Numbers::EvtNum> Events ;
  Events events  ;
  StatusCode sc = parse_ ( events , input ) ;
  // 
  
  result = LoKi::Numbers::EvtNumList ( events );
  //
  return sc ;
}
// =============================================================================
// RunEvt-pair 
// =============================================================================
LoKi::Numbers::RunEvt::RunEvt
( const LoKi::Numbers::RunEvt::run_type run , 
  const LoKi::Numbers::RunEvt::evt_type evt ) 
  : m_pair ( run , evt ) 
{}
// =============================================================================
// RunEvt-pair 
// =============================================================================
LoKi::Numbers::RunEvt::RunEvt
( const LoKi::Numbers::RunEvt::run_type run , 
  const LoKi::Numbers::EvtNum::evt_type evt ) 
  : m_pair ( run , LoKi::Numbers::EvtNum ( evt ) ) 
{}
// ============================================================================
// hash 
// ============================================================================
std::size_t LoKi::Numbers::RunEvt::hash       () const 
{
  std::size_t seed = 0 ;
  //
  boost::hash_combine ( seed , m_pair.first  ) ;
  boost::hash_combine ( seed , m_pair.second ) ;  
  //
  return seed ;
}
// ==============================================================================
std::ostream& LoKi::Numbers::RunEvt::fillStream ( std::ostream& s ) const 
{ return Gaudi::Utils::toStream ( m_pair , s ) ; }
// ==============================================================================
std::string   LoKi::Numbers::RunEvt::toString   () const 
{ 
  std::ostringstream s;
  fillStream ( s ) ;
  return s.str();
}
// =============================================================================
//  the streamer 
// =============================================================================
std::ostream& Gaudi::Utils::toStream 
( const LoKi::Numbers::RunEvt& evt , 
  std::ostream&                s   ) { return evt.fillStream ( s )  ; }
// =============================================================================
//  the streamer 
// =============================================================================
std::string   Gaudi::Utils::toString
( const LoKi::Numbers::RunEvt& evt ) { return evt.toString ()  ; }
// =============================================================================
std::string LoKi::Numbers::RunEvt::toCpp() const 
{ 
  return "LoKi::Numbers::RunEvt(" 
    + Gaudi::Utils::toCpp ( m_pair.first  ) + "," 
    + Gaudi::Utils::toCpp ( m_pair.second ) + "," ; 
}  
// =============================================================================
// parser 
// =============================================================================
StatusCode 
Gaudi::Parsers::parse 
( LoKi::Numbers::RunEvt& result, const std::string& input)
{ return parse_ ( result , input ) ; }
// =============================================================================
// RunEvtList
// =============================================================================
LoKi::Numbers::RunEvtList::RunEvtList
( const LoKi::Numbers::RunEvtList::runevt_list& lst )  
  : m_list ( lst )  
{
  std::sort ( m_list.begin() , m_list.end() ) ;
  runevt_list::iterator i = std::unique( m_list.begin() , m_list.end() ) ;
  m_list.erase ( i ,  m_list.end() ) ;
}
// =============================================================================
// RunEvtList
// =============================================================================
LoKi::Numbers::RunEvtList::RunEvtList
( const LoKi::Numbers::RunEvt& evt  )  
  : m_list ( 1 , evt  )  
{}
// ============================================================================
// hash 
// ============================================================================
std::size_t LoKi::Numbers::RunEvtList::hash       () const 
{
  return std::accumulate( begin(), end(), std::size_t{0},
                          [](std::size_t seed, const LoKi::Numbers::RunEvt&  i ) {
                              boost::hash_combine(seed,i);
                              return seed;
                          } );
}
// ============================================================================
std::ostream& LoKi::Numbers::RunEvtList::fillStream ( std::ostream& s ) const 
{ return Gaudi::Utils::toStream ( m_list , s ) ; }
// =============================================================================
std::string LoKi::Numbers::RunEvtList::toCpp() const 
{ return "LoKi::Numbers::RunEvtList(" + Gaudi::Utils::toCpp ( m_list ) + ")" ; }  
// ============================================================================
std::string   LoKi::Numbers::RunEvtList::toString   () const 
{ 
  std::ostringstream s;
  fillStream ( s ) ;
  return s.str();
}
// ==============================================================================
bool LoKi::Numbers::RunEvtList::contains 
( const LoKi::Numbers::RunEvt& e ) const 
{ return std::binary_search ( begin() , end () , e ) ; }
// =============================================================================
// append 
// ============================================================================
LoKi::Numbers::RunEvtList
LoKi::Numbers::RunEvtList::operator+
( const LoKi::Numbers::RunEvt& e ) const 
{
  //
  if ( empty() ) { return RunEvtList ( runevt_list( 1, e ) ) ; }
  //
  runevt_list lst  ( begin() , end() ) ;
  lst.push_back ( e ) ;
  //
  return RunEvtList ( lst ) ;
}
// ============================================================================
// append 
// ============================================================================
LoKi::Numbers::RunEvtList
LoKi::Numbers::RunEvtList::operator+
( const LoKi::Numbers::RunEvtList& e ) const 
{
  if      ( e.empty()  ) { return *this ; }
  else if (   empty()  ) { return     e ; }
  else if ( &e == this ) { return *this ; }
  //
  runevt_list lst ;
  lst.reserve ( size() + e.size() );
  lst.insert  ( lst.end() ,   begin() ,   end () ) ;
  lst.insert  ( lst.end() , e.begin() , e.end () ) ;
  //
  return RunEvtList ( lst ) ;
}
// =============================================================================
// parser 
// =============================================================================
StatusCode 
Gaudi::Parsers::parse 
( LoKi::Numbers::RunEvtList& result, const std::string& input)
{
  //
  std::vector<LoKi::Numbers::RunEvt> events  ;
  StatusCode sc = parse_ ( events , input ) ;
  // 
  result = LoKi::Numbers::RunEvtList ( events );
  //
  return sc ;
}
// ===============================================================================
// respect python 
// ============================================================================
LoKi::Numbers::EvtNumList 
LoKi::Numbers::add1 
( const LoKi::Numbers::EvtNum&     e1 , 
  const LoKi::Numbers::EvtNumList& e2 ) { return e1 + e2 ; }
//
LoKi::Numbers::EvtNumList 
LoKi::Numbers::add1 
( const LoKi::Numbers::EvtNumList& e1 , 
  const LoKi::Numbers::EvtNum&     e2 ) { return e1 + e2 ; }
//
LoKi::Numbers::EvtNumList 
LoKi::Numbers::add1 
( const LoKi::Numbers::EvtNumList& e1 , 
  const LoKi::Numbers::EvtNumList& e2 ) { return e1 + e2 ; }
//
LoKi::Numbers::EvtNumList
LoKi::Numbers::add1 
( const LoKi::Numbers::EvtNumList& e1 , 
  LoKi::Numbers::EvtNum::evt_type  e2 ) { return e1 + e2 ; }
//
LoKi::Numbers::EvtNumList 
LoKi::Numbers::add1 
( const LoKi::Numbers::EvtNum&     e1 , 
  const LoKi::Numbers::EvtNum&     e2 ) { return EvtNumList ( e1 )  + e2 ; }
// =============================================================================
LoKi::Numbers::RunEvtList 
LoKi::Numbers::add2 
( const LoKi::Numbers::RunEvt&     e1 , 
  const LoKi::Numbers::RunEvtList& e2 ) { return e1 + e2 ; }
//
LoKi::Numbers::RunEvtList 
LoKi::Numbers::add2 
( const LoKi::Numbers::RunEvtList& e1 , 
  const LoKi::Numbers::RunEvt&     e2 ) { return e1 + e2 ; }
// 
LoKi::Numbers::RunEvtList 
LoKi::Numbers::add2 
( const LoKi::Numbers::RunEvtList& e1 , 
  const LoKi::Numbers::RunEvtList& e2 ) { return e1 + e2 ; }
// 
LoKi::Numbers::RunEvtList 
LoKi::Numbers::add2 
( const LoKi::Numbers::RunEvt&     e1 , 
  const LoKi::Numbers::RunEvt&     e2 ) { return e1 + e2 ; }
// =============================================================================
// The END 
// =============================================================================
