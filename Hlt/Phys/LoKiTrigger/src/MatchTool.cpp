// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IANNSvc.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
#include "HltBase/HltTrackMerge.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MatchTool.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::MatchTool 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-10-28
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 */
// ============================================================================

// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::MatchTool::MatchTool 
( const LoKi::Hlt1::MatchConf&         config  )   //        tool configuration 
  : LoKi::Hlt1::HelperTool ( 1 ) 
  , m_config     { config  }
  , m_match      {         }
  , m_match2     {         }
  , m_recoID     { 0       }
// 
  , m_invert     { false   }
{  
  // retrive the tool 
  IAlgTool* t = alg()->tool<IAlgTool> ( mTool() ) ;
  //
  Assert ( t , "Can't aquire the tool!" );
  //
  SmartIF<ITrackMatch>                i_1 ( t ) ;
  if ( i_1 ) { m_match  = i_1.get () ; }
  //
  SmartIF<Hlt::ITrack2CandidateMatch> i_2 ( t ) ;
  if ( i_2 ) { m_match2 = i_2.get () ; }
  //
  Assert ( !(!m_match)  || !(!m_match2) , "Can't aquire right interface!" ) ; 
  //
  // get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
  { // recoID 
    auto info = ann->value( Gaudi::StringKey(std::string("InfoID")) , mTool()  );
    Assert( info , " request for unknown Info ID : " + mTool() );
    //
    m_recoID = info->second ;
  }
  // 
}
// ============================================================================
/* perform the track matching 
 *  @param tr1 the first  track 
 *  @param tr2 the second track 
 *  @return matched track (if any) 
 */
// ============================================================================
const LHCb::Track* LoKi::Hlt1::MatchTool::match 
( const LHCb::Track* tr1 , 
  const LHCb::Track* tr2 ) const 
{
  // ==========================================================================
  if ( !tr1 || !tr2 ) { return nullptr ; }                            // RETURN 
  // ==========================================================================
  /// get new track 
  std::unique_ptr<LHCb::Track> track3 ( new LHCb::Track() ) ;
  // the actual track matching
  double q1 = 0 ;
  double q2 = 0 ;
  StatusCode sc = match() -> match ( *tr1 , *tr2 , *track3 , q1 , q2 ) ;
  if ( sc.isFailure() ) { return nullptr ; }                          // RETURN
  //
  // move info ? 
  if ( moveInfo () ) 
  {
     Hlt::MergeInfo ( *tr1 , *track3 ) ;
     Hlt::MergeInfo ( *tr2 , *track3 ) ;          
  }        
  ///
  LHCb::Track* track = track3.release() ;
  if ( !track->parent() ) 
  { storedTracks ( address() )->insert ( track ); }
  //
  // do not put "bad" match into stream 
  if ( !m_config ( track )  ) { return nullptr ; }                    // RETURN 
  //
  return track ;                                                      // RETURN 
}
// ============================================================================
/*  perform the track matching 
 *  @param tr1 the first  track 
 *  @param tr2 the second track 
 *  @return matched track (if any) 
 */
// ============================================================================
const LHCb::Track* LoKi::Hlt1::MatchTool::match 
( const LHCb::Track*    track , 
  const Hlt::Candidate* cand  ) const 
{
  //
  if ( !track || !cand ) { return nullptr ; }                         // RETURN
  //
  // it is track <--> track match! 
  if ( !m_match2 ) 
  {
    const LHCb::Track* track2  = cand->get<LHCb::Track> () ;
    if ( !track2  ) { return nullptr ; }                              // RETURN
    return match ( track , track2 ) ;                                 // RETURN 
  }
  //
  /// get new track 
  LHCb::Track* track3 { new LHCb::Track()  }; // where does the ownership go???
  // the actual track matching
  double q1 = 0 ;
  double q2 = 0 ;
  StatusCode sc = match2() -> match ( *track , *cand , *track3 , q1 , q2 ) ;
  if ( sc.isFailure() ) { return nullptr ; }                           // RETURN
  
  // move info ? 
  if ( moveInfo () ) 
  {
     Hlt::MergeInfo ( *track , *track3 ) ;
  }        
  ///
  if ( !track3->parent() )   //TODO/FIXME where does ownership go if this fails???
  { storedTracks ( address() )->insert ( track3 ); }
  //
  // do not put "bad" match into stream 
  if ( !m_config ( track3 )  ) { return nullptr ; }                   // RETURN
  //
  return track3;                                                      // RETURN
  //
}
// ============================================================================
/*  perform the track matching 
 *  @param tr1 the first  track 
 *  @param tr2 the second track 
 *  @return matched track (if any) 
 */
// ============================================================================
bool LoKi::Hlt1::MatchTool::matched  
( const LHCb::Track*    track , 
  const Hlt::Candidate* cand  ) const 
{
  if ( !track || !cand ) { return false  ; }                   // RETURN
  //
  Assert ( !(!match2()) , "Invalid setup!" ) ;
  //
  // it is track <--> track match! 
  double q1 = 0;
  double q2 = 0;
  return match2() -> match ( track, cand, q1, q2 );
}

// ============================================================================
// The END 
// ============================================================================
