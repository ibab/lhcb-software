// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrMatch.h"
#include "LoKi/TrMatch.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::TrMatch
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-17
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match::Match 
( const std::string&           output  ,   //   output selection name/key 
  const Source&                tracks2 ,   //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )   //          tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source2    ( tracks2 ) 
  , m_sink       ( output  ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match::Match 
( const std::string&           output  ,   //   output selection name/key 
  const std::string&           tracks2 ,   //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )   //          tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source2    ( LoKi::Hlt1::Selection ( tracks2 ) ) 
  , m_sink       ( output  ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::Match::~Match(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Match* LoKi::Hlt1::Match::clone() const 
{ return new LoKi::Hlt1::Match ( *this ) ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::Match::result_type 
LoKi::Hlt1::Match::operator() 
  ( LoKi::Hlt1::Match::argument a ) const 
{
  //
  Assert ( !(!match()) && 0 != alg() ,  "Invalid setup!" ) ;
  
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;
  
  // get the tracks from the second source 
  const CANDIDATES cand_  = source()() ;
  
  // the output selection 
  CANDIDATES       output ;
  
  // input selections:
  const CANDIDATES* arg1 = &a     ;
  const CANDIDATES* arg2 = &cand_ ;
  
  /// swap the arguments (if needed) 
  if  ( invert() ) { std::swap ( arg1 , arg2 ) ; }
  
  for ( ITERATOR icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 ) 
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( 0 == cand1 ) { continue ; }                              // CONTINUE 
    const LHCb::Track*    trk1  = cand1->get<LHCb::Track> () ;
    if ( 0 == trk1  ) { continue ; }                              // CONTINUE  
    //
    for ( ITERATOR icand2 = arg2->begin() ; arg2->end() != icand2 ; ++icand2 ) 
    {
      const Hlt::Candidate* cand2 = *icand2 ;
      if ( 0 == cand2 ) { continue ; }                            // CONITNUE 
      const LHCb::Track*    trk2  = cand2->get<LHCb::Track> () ;
      if ( 0 == trk2  ) { continue ; }                            // CONTINUE 
      //
      // match track?
      const LHCb::Track* track = match ( trk1 , trk2 ) ;
      if ( 0 == track ) { continue ; }                            // CONTINUE 
      //
      /// get new candidate:
      Hlt::Candidate* candidate = newCandidate () ;
      candidate -> addToWorkers ( alg() ) ;
      //
      {
        //
        // 1. "initiator" stage - copy the first candidate:
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , match () ) ;
        //
        lock.addToHistory ( cand1->workers() ) ;
        stage -> set ( cand1->currentStage() ) ;
      }
      //
      {
        //
        //  2. "regular" stage : track
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , match () ) ;
        stage    -> set ( track ) ;
      }
      //
      // add new candidate to the output:
      output.push_back ( candidate ) ;
      //
    } //                              end of the loop over the second container
    // ========================================================================
  } //                                end of the loop over the first  container 
  // ==========================================================================
  // register the selection in Hlt Data Service 
  return m_sink ( output ) ;                                          // RETURN 
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::Match::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TC_MATCH("
      << "'" << output () << "',"
      <<        source () << "," 
      <<        config () << ")" ;  
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match2::Match2 
( const std::string&           output  ,         //   output selection name/key 
  const Source&              tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::Match ( output , tracks2 , config ) 
{
  setInverted ( true ) ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match2::Match2 
( const std::string&           output  ,         //   output selection name/key 
  const std::string&           tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::Match ( output , tracks2 , config ) 
{
  setInverted ( true ) ;
}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::Match2::~Match2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Match2* LoKi::Hlt1::Match2::clone() const 
{ return new LoKi::Hlt1::Match2 ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::Match2::fillStream ( std::ostream& s ) const 
{
  return 
    s <<  "TC_MATCH2(" 
      << "'" << output () << "',"
      <<        source () << "," 
      <<        config () << ")" ;  
}
// ============================================================================
// The END 
// ============================================================================


