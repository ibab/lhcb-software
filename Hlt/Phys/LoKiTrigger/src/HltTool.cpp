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
( std::string  tool , 
  bool         flag ) 
  : m_tool   ( std::move(tool) ) 
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
( std::string  tool , 
  bool         flag ) 
  : LoKi::Hlt1::Tool ( std::move(tool) , flag ) {}
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
( std::string tool , 
  bool        flag ) 
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
( std::string tool , 
  bool        flag ) 
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
( std::string tool , 
  bool        flag ) 
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
// The END 
// =============================================================================
