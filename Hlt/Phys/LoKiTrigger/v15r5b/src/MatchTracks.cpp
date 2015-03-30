// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::MatchTracks
 *  @author Alex Shires ashires@cern.ch
 *  @date 2014-09-02
 */
#include "LoKi/TrgToCpp.h"
#include "LoKi/MatchConf.h"
#include "LoKi/MatchTracks.h"
#include "LoKi/ToCpp.h"

//
LoKi::Hlt1::MatchTracks::MatchTracks
( const Source &                candidates, 
  const LoKi::Hlt1::MatchConf & config ) 
  : LoKi::AuxFunBase ( std::tie ( candidates , config ) ) 
  , LoKi::Hlt1::MatchTool ( config     )
  , m_source2             ( candidates ) 
{}
//
LoKi::Hlt1::MatchTracks::MatchTracks
( const std::string&           candidates, 
  const LoKi::Hlt1::MatchConf& config ) 
  : LoKi::AuxFunBase ( std::tie ( candidates , config ) ) 
  , LoKi::Hlt1::MatchTool ( config )
  , m_source2 ( LoKi::Hlt1::Selection( candidates ) ) 
{}
//
LoKi::Hlt1::MatchTracks::~MatchTracks(){}
//
std::ostream& LoKi::Hlt1::MatchTracks::fillStream ( std::ostream& s ) const 
{ return s << "TC_MATCHTRACKS( " << source()  << ", " << config() << " )" ; }
//
LoKi::Hlt1::MatchTracks* LoKi::Hlt1::MatchTracks::clone() const 
{ return new LoKi::Hlt1::MatchTracks { *this } ; }
//
LoKi::Hlt1::MatchTracks::result_type 
LoKi::Hlt1::MatchTracks::operator() 
  ( LoKi::Hlt1::MatchTracks::argument_type t ) const 
{
  // the output selection 
  result_type   output ;
    //
  const Source::result_type& cands = source()() ;
  //loop over inputs and match track
  bool match_track = false ;
  //avoid lambdas for the moment! unsafe?? 
  //seocnd iterator should be hlt candidates
  for ( auto itr(std::begin(cands)) , end2(std::end(cands)) ; itr != end2 ; ++itr )
  {
    //matched function comes from MatchTool class
    match_track = matched ( t , *itr ) ; 
    if ( match_track )  break ;
  }
  output = match_track ;
  return output ;
}


