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


void
LoKi::Hlt1::Match::addStage( Hlt::Candidate &candidate
                           , const Hlt::Candidate& cand1 ) const
{
        /// get new stage 
        Hlt::Stage*     stage   = newStage() ;
        candidate.addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock { stage , match() } ;
        //
        lock.addToHistory ( cand1.workers() ) ;
        // lock.addToHistory ( myName ()  ) ;
        stage -> set ( cand1.currentStage() ) ;
}

void 
LoKi::Hlt1::Match::addStage( Hlt::Candidate &candidate
                           , const LHCb::Track& track ) const
{
        /// get new stage 
        Hlt::Stage*     stage   = newStage() ;
        candidate.addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock { stage , match() } ;
        // lock.addToHistory ( myName () ) ;
        stage -> set ( &track ) ;
}

Hlt::Candidate* 
LoKi::Hlt1::Match::createCandidate(const Hlt::Candidate& cand1, const LHCb::Track& track) const
{
      /// create new candidate:
      Hlt::Candidate* candidate = newCandidate () ;
      candidate -> addToWorkers ( alg() ) ;
      //  1. "initiator" stage - copy the first candidate:
      addStage( *candidate, cand1 );
      //  2. "regular" stage : track
      addStage( *candidate, track );

      return candidate;
}

// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match::Match 
( std::string                  output  ,   //   output selection name/key 
  const Source&                tracks2 ,   //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )   //          tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source2    { tracks2 } 
  , m_sink       { std::move(output)  }
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match::Match 
( std::string                  output  ,   //   output selection name/key 
  std::string                  tracks2 ,   //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )   //          tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source2    { LoKi::Hlt1::Selection{ std::move(tracks2) } }
  , m_sink       { std::move( output  )  }
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::Match::~Match(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Match* LoKi::Hlt1::Match::clone() const 
{ return new LoKi::Hlt1::Match { *this } ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::Match::result_type 
LoKi::Hlt1::Match::operator() 
  ( LoKi::Hlt1::Match::argument a ) const 
{
  Assert ( alg() , "Invalid setup" ) ;
  //
  // the output selection 
  result_type       output ;
  //
  if ( a.empty() ) { return output ; }                              // RETURN
  
  // get the tracks from the second source 
  const result_type cand_  = source()() ;
  
  // input selections:
  auto* arg1 = &a     ;
  auto* arg2 = &cand_ ;
  
  /// swap the arguments (if needed) 
  if  ( invert() ) { std::swap ( arg1 , arg2 ) ; }
  
  auto createAndAppend = [&]( const Hlt::Candidate& c1
                            , const LHCb::Track*    t2 )
  { if (t2) output.push_back( createCandidate(c1,*t2) ) ; };

#if 0
  struct appendIf { // back_inserter, but only if valid...
     result_type& output_;
     appendIf( output ) : output_(output) {}
     appendIf& operator++() { return *this; }
     appendIf& operator=(Hlt::Candidate* rhs) { if (rhs) output_.push_back(rhs); return *this; }
  };

  std::tranform( std::begin(*arg2), std::end(*arg2)
               , appendIf(output);
               , [&]( const Hlt::Candidate *c2)
                 { auto*  t2 = ( c2 ? match(trk1,c2)   : nullptr );
                   return t2 ? createCandidate(*cand1,*t2) : nullptr; 
                 } );

#endif

  std::for_each( std::begin(*arg1), std::end(*arg2)
               , [&](const Hlt::Candidate *c1)
                 { auto* t1 = c1 ? c1->get<LHCb::Track> () : nullptr ;
                   if ( !t1 ) return ;
                   std::for_each( std::begin(*arg2), std::end(*arg2)
                                , [&](const Hlt::Candidate *c2)
                                  { createAndAppend( *c1
                                                   , c2 ? match(t1,c2)
                                                        : nullptr ) ;
                                  } );
                 } );
  // ==========================================================================
  // register the selection in Hlt Data Service 
  return !m_sink ? output : m_sink ( output ) ;                       // RETURN 
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::Match::fillStream ( std::ostream& s ) const 
{
  return s << "TC_MATCH("    
           << "'" << output () << "',"
           <<        source () << "," 
           <<        config () << ")" ;  
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match2::Match2 
( std::string                output  ,         //   output selection name/key 
  const Source&              tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::Match ( std::move(output) , tracks2 , config ) 
{
  setInverted ( true ) ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::Match2::Match2 
( std::string                  output  ,         //   output selection name/key 
  std::string                  tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::Match ( std::move(output) , std::move(tracks2) , config ) 
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
{ return new LoKi::Hlt1::Match2 { *this } ; }
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
// constructor 
// ============================================================================
LoKi::Hlt1::FilterMatch::FilterMatch
( const LoKi::Hlt1::FilterMatch::Source& candidates , //   candidates for matching 
  const LoKi::Hlt1::MatchConf&           config     ) //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source     ( candidates  ) 
{
  Assert ( !(!match2() ) , "Invalid setup!" ) ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::FilterMatch::FilterMatch
( std::string                  candidates ,   //   candidates for matching 
  const LoKi::Hlt1::MatchConf& config     )   //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::MatchTool ( config )
  , m_source     {  LoKi::Hlt1::Selection{ std::move(candidates)  } } 
{
  Assert ( !(!match2() ) , "Invalid setup!" ) ;
}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::FilterMatch::~FilterMatch(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::FilterMatch*
LoKi::Hlt1::FilterMatch::clone() const 
{ return new LoKi::Hlt1::FilterMatch { *this } ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::FilterMatch::result_type 
LoKi::Hlt1::FilterMatch::operator() 
  ( LoKi::Hlt1::Match::argument a ) const 
{
  // the output selection 
  result_type       output ;
  //
  if ( a.empty() ) { return output ; }                              // RETURN

  // get the tracks from the second source 
  const result_type cand_  = source()() ;

  // input selections:
  auto* arg1 = &a     ;
  auto* arg2 = &cand_;
  
  /// swap the arguments (if needed) 
  if  ( invert() ) { std::swap ( arg1 , arg2 ) ; }
  //
  std::copy_if( std::begin(*arg1), std::end(*arg1)
              , std::back_inserter(output)
              , [&](const Hlt::Candidate *c1) 
                { auto* t1 = c1 ? c1->get<LHCb::Track>() : nullptr ;
                  return t1 && std::any_of( std::begin(*arg2), std::end(*arg2)
                                          , [&](const Hlt::Candidate* c2) 
                                            { return c2 && matched(t1,c2); });
                } ) ;
  //
  return output  ;                                                    // RETURN 
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::FilterMatch::fillStream ( std::ostream& s ) const 
{
  return 
    s <<  "TC_MATCHFLTR(" 
      <<        source () << "," 
      <<        config () << ")" ;  
}

// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::FilterMatch2::FilterMatch2 
( const Source&              tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::FilterMatch ( tracks2 , config ) 
{
  setInverted ( true ) ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::FilterMatch2::FilterMatch2 
( std::string                  tracks2 ,         //   tracks to be matched with 
  const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
  : LoKi::Hlt1::FilterMatch ( std::move(tracks2) , config ) 
{
  setInverted ( true ) ;
}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::FilterMatch2::~FilterMatch2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::FilterMatch2* LoKi::Hlt1::FilterMatch2::clone() const 
{ return new LoKi::Hlt1::FilterMatch2 { *this } ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::FilterMatch2::fillStream ( std::ostream& s ) const 
{
  return 
    s <<  "TC_MATCHFLTR2(" 
      <<        source () << "," 
      <<        config () << ")" ;  
}
 
 
// ============================================================================
// The END 
// ============================================================================
