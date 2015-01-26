// $Id$
// ============================================================================
// Include files 
// =============================================================================
// STD & STL 
// =============================================================================
#include <sstream>
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/HltTool.h"
#include "LoKi/ToCpp.h"
// =============================================================================
/** @file 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-31
 */
// =============================================================================
/*  constructor from the tool type/name & "public" flag
 *  @param tool the tool type/name 
 *  @param flag public?
 */
// =============================================================================
LoKi::Hlt1::Tool::Tool 
( const std::string& tool , 
  const bool         flag ) 
  : m_tool   ( tool ) 
  , m_public ( flag ) 
{}
// =============================================================================
// conversion to string 
// =============================================================================
std::string LoKi::Hlt1::Tool::toString    () const 
{
  std::ostringstream s ;
  fillStream ( s )  ;
  return s.str() ;
}
// =============================================================================

// =============================================================================
/*  constructor from tool type/name & "public" flag
 *  @param tool the tool type/name 
 *  @param flag public?
 */
// =============================================================================
LoKi::Hlt1::TrackFunction::TrackFunction 
( const std::string& tool , 
  const bool         flag ) 
  : LoKi::Hlt1::Tool ( tool , flag ) {}
// ============================================================================
// constructor from the tool TL
// ============================================================================
LoKi::Hlt1::TrackFunction::TrackFunction 
( const LoKi::Hlt1::Tool& tool ) 
  : LoKi::Hlt1::Tool ( tool ) 
{}
// =============================================================================
// output operator 
// =============================================================================
std::ostream& LoKi::Hlt1::TrackFunction::fillStream  ( std::ostream& s ) const 
{
  return s << "LoKi.Hlt1.TrackFunction('" 
           << tool() << "'," << ( pub() ? "True" : "False" ) << ")" ;  
}
// =============================================================================

// =============================================================================
/*  constructor from tool type/name & "public" flag
 *  @param tool the tool type/name 
 *  @param flag public?
 */
// =============================================================================
LoKi::Hlt1::TrackBiFunction::TrackBiFunction 
( const std::string& tool , 
  const bool         flag ) 
  : LoKi::Hlt1::Tool ( std::move(tool) , flag ) {}
// ============================================================================
// constructor from the tool T
// ============================================================================
LoKi::Hlt1::TrackBiFunction::TrackBiFunction 
( const LoKi::Hlt1::Tool& tool ) 
  : LoKi::Hlt1::Tool ( tool ) 
{}
// =============================================================================
// output operator 
// =============================================================================
std::ostream& LoKi::Hlt1::TrackBiFunction::fillStream 
( std::ostream& s ) const 
{
  return s << "LoKi.Hlt1.TrackBiFunction('" 
           << tool() << "'," << ( pub() ? "True" : "False" ) << ")" ;  
}
// =============================================================================


// =============================================================================
/*  constructor from tool type/name & "public" flag
 *  @param tool the tool type/name 
 *  @param flag public?
 */
// =============================================================================
LoKi::Hlt1::MatchTVelo::MatchTVelo
( const std::string& tool , 
  const bool         flag ) 
  : LoKi::Hlt1::Tool ( std::move(tool) , flag ) {}
// ============================================================================
// constructor from the tool T
// ============================================================================
LoKi::Hlt1::MatchTVelo::MatchTVelo
( const LoKi::Hlt1::Tool& tool ) 
  : LoKi::Hlt1::Tool ( tool ) 
{}
// =============================================================================
// output operator 
// =============================================================================
std::ostream& LoKi::Hlt1::MatchTVelo::fillStream 
( std::ostream& s ) const 
{
  return s << "LoKi.Hlt1.MatchTVelo('" 
           << tool() << "'," << ( pub() ? "True" : "False" ) << ")" ;  
}
// =============================================================================

// =============================================================================
/*  constructor from tool type/name & "public" flag
 *  @param tool the tool type/name 
 *  @param flag public?
 */
// =============================================================================
LoKi::Hlt1::TrackVertexFunction::TrackVertexFunction
( const std::string& tool , 
  const bool         flag ) 
  : LoKi::Hlt1::Tool ( std::move(tool) , flag ) {}
// ============================================================================
// constructor from the tool T
// ============================================================================
LoKi::Hlt1::TrackVertexFunction::TrackVertexFunction
( const LoKi::Hlt1::Tool& tool ) 
  : LoKi::Hlt1::Tool ( tool ) 
{}
// =============================================================================
// output operator 
// =============================================================================
std::ostream& LoKi::Hlt1::TrackVertexFunction::fillStream 
( std::ostream& s ) const 
{
  return s << "LoKi.Hlt1.TrackVertexFunction('" 
           << tool() << "'," << ( pub() ? "True" : "False" ) << ")" ;  
}
// =============================================================================

// =============================================================================
std::string Gaudi::Utils::toCpp 
( const LoKi::Hlt1::Tool&                o ) 
{
  return " LoKi::Hlt1::Tool(" 
    + toCpp ( o.tool () ) + "," 
    + toCpp ( o.flag () ) + ") " ;
}
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::TrackFunction&       o ) 
{
  return " LoKi::Hlt1::TrackFunction(" 
    + toCpp ( o.tool () ) + "," 
    + toCpp ( o.flag () ) + ") " ;
} 
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::TrackBiFunction&     o ) 
{
  return " LoKi::Hlt1::TrackBiFunction(" 
    + toCpp ( o.tool () ) + "," 
    + toCpp ( o.flag () ) + ") " ;
}
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::MatchTVelo&          o ) 
{
  return " LoKi::Hlt1::MatchTVelo(" 
    + toCpp ( o.tool () ) + "," 
    + toCpp ( o.flag () ) + ") " ;
}
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::TrackVertexFunction& o ) 
{
  return " LoKi::Hlt1::TrackVertexFunction("
    + toCpp ( o.tool () ) + "," 
    + toCpp ( o.flag () ) + ") " ;
}
// =============================================================================
// The END 
// =============================================================================
