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
      /// get new stage 
      Hlt::Stage*     stage    = newStage     () ;
      candidate -> addToStages ( stage ) ;
      /// lock new stage:
      Hlt::Stage::Lock lock ( stage , alg() ) ;
      stage     -> set( track ) ;
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



// // ============================================================================
// // Obsolete?
// // ============================================================================

// // ============================================================================
// // constructor 
// // ============================================================================
// LoKi::Hlt1::TrMatch::TrMatch 
// ( const std::string&                   output  ,   // output selection name/key 
//   const LoKi::Hlt1::TrMatch::TrSource& tracks2 ,   // tracks to be matched with 
//   const LoKi::Hlt1::MatchConf&         config  )   //        tool configuration 
//   : LoKi::BasicFunctors<const LHCb::Track*>::Pipe ()
//   , LoKi::Hlt1::MatchTool ( config )
//   , m_tracks2    ( tracks2 ) 
//   , m_sink       ( output  ) 
// {}
// // ============================================================================
// // the only one important method 
// // ============================================================================
// LoKi::Hlt1::TrMatch::result_type 
// LoKi::Hlt1::TrMatch::operator() 
//   ( LoKi::Hlt1::TrMatch::argument a ) const 
// {
  
//   Assert ( !(!match()) && 0 != alg() ,  "Invalid setup!" ) ;
  
//   typedef LHCb::Track::Container          Tracks ;  
//   typedef std::vector<const LHCb::Track*> TRACKS ;
  

//   // get the tracks from the second source 
//   TRACKS tracks2 = m_tracks2() ;
  
//   // the output selection 
//   TRACKS output ;
  
//   const TRACKS* arg1 = &a       ;
//   const TRACKS* arg2 = &tracks2 ;
  
//   /// swap the arguments 
//   if  ( invert() ) 
//   { 
//     arg1 = &tracks2 ;
//     arg2 = &a       ;
//   }
  
//   // double loop over all 2-tracks combinations
//   for ( TRACKS::const_iterator itrk1 = arg1->begin() ; arg1->end() != itrk1 ; ++itrk1 ) 
//   {
//     const LHCb::Track* trk1 = *itrk1 ;
//     if ( 0 == trk1 ) { continue ; }                                // CONTINUE 
//     //
//     for ( TRACKS::const_iterator itrk2 = arg2->begin() ; arg2->end() != itrk2 ; ++itrk2 ) 
//     {
//       const LHCb::Track* trk2 = *itrk2 ;
//       if ( 0 == trk2 ) { continue ; }                              // CONTINUE  
//       //
//       // call the main mehtod 
//       const LHCb::Track* track3 = match ( trk1 , trk2 ) ;
//       //
//       if ( 0 != track3 ) { output.push_back ( track3 ) ; }
//       // ======================================================================
//     } //                    end of the loop over the second container of tracks 
//     // ========================================================================
//   } //                      end of the loop over the first  container of tracks
//   // ==========================================================================
//   //                                                                   final... 
//   // ==========================================================================
//   // register the selection in Hlt Data Service 
//   return m_sink ( output ) ;                                          // RETURN 
//   // ==========================================================================
// }
// // ============================================================================
// // OPTIONAL: nice printout 
// // ============================================================================
// std::ostream& LoKi::Hlt1::TrMatch::fillStream ( std::ostream& s ) const 
// {
//   return 
//     s << "TrMATCH("
//       << "'" << address ()  << "',"
//       <<        tracks2 ()  << "," 
//       <<        config  ()  << ")" ;  
// }
// // ============================================================================

// // ============================================================================
// // constructor 
// // ============================================================================
// LoKi::Hlt1::TrMatch2::TrMatch2 
// ( const std::string&           output  ,         //   output selection name/key 
//   const TrSource&              tracks2 ,         //   tracks to be matched with 
//   const LoKi::Hlt1::MatchConf& config  )         //          tool configuration 
//   : LoKi::Hlt1::TrMatch ( output , tracks2 , config ) 
// {
//   setInverted ( true ) ;
// }
// // ============================================================================
// // OPTIONAL: nice printout 
// // ============================================================================
// std::ostream& LoKi::Hlt1::TrMatch2::fillStream ( std::ostream& s ) const 
// {
//   return 
//     s <<  "TrMATCH2(" 
//       << "'" << address ()  << "',"
//       <<        tracks2 ()  << "," 
//       <<        config  ()  << ")" ;  
// }
  


// ============================================================================
// The END 
// ============================================================================


