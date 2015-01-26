#ifndef MATCH_TRACKS_H
#define MATCH_TRACKS_H

// ============================================================================
// Include files
// ============================================================================

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"

#include "LoKi/MatchTool.h"
#include "Event/HltCandidate.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/MatchConf.h"
#include "LoKi/MatchTool.h"


// ============================================================================
//  @file LoKi/MatchTracks.h
//  Track Matching to a set of Hlt Candidates via a tool with a boolean decision
//  Thanks to Vanya for help with implementation.
//  @author Alex Shires 
//  @ see ITrackMatch
//  @ see MatchTool
//  @ see LoKi::Cuts::TC_MATCHCANDS
//  @date 2014-09-02
// ============================================================================

namespace LoKi 
{
  namespace Hlt1 
  {
    class MatchTracks 
      : public LoKi::Functor<const LHCb::Track*,bool> //unary function 
      , public LoKi::Hlt1::MatchTool  //provides the matching
    {
    private:
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      /// souce of hlt candidates to be matched 
      LoKi::Assignable<Source>::Type m_source2 ; //            source of calo
    public:
      //constructor with source
      MatchTracks ( const Source&      s, 
                    const LoKi::Hlt1::MatchConf& config ) ;
      // constructor with string
      MatchTracks ( const std::string& s, 
                    const LoKi::Hlt1::MatchConf& config ) ;
      /// result method
      result_type operator() ( argument_type t ) const ;
      /// MANDATORY: virtual destructor 
      virtual ~MatchTracks() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MatchTracks* clone() const ;
      
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
    private:
      ///disable default constructor
      MatchTracks() ;
    public:
      /// the second source 
      const Source&      source () const { return m_source2.func() ; }      
    } ;
  }
}

#endif /* MATCH TRACKS */
