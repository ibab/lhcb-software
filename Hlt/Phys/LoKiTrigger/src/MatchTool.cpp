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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
  , m_config     ( config  )
  , m_match      (         )
  , m_match2     (         )
  , m_recoID     ( 0       )
// 
  , m_invert     ( false   )
{  
  // retrive the tool 
  IAlgTool* t = alg()->tool<IAlgTool> ( mTool() ) ;
  //
  Assert ( 0 != t , "Can't aquire the tool!" );
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
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( Gaudi::StringKey(std::string("InfoID")) , mTool()  );
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
  if ( 0 == tr1 || 0 == tr2 ) { return 0 ; }                          // RETURN 
  // ==========================================================================
  /// get new track 
  std::auto_ptr<LHCb::Track> track3 ( new LHCb::Track() ) ;
  // the actual track matching
  double q1 = 0 ;
  double q2 = 0 ;
  StatusCode sc = match() -> match ( *tr1 , *tr2 , *track3 , q1 , q2 ) ;
  if ( sc.isFailure() ) { return 0 ; }                               // RETURN
  //
  // move info ? 
  if ( moveInfo () ) 
  {
     Hlt::MergeInfo ( *tr1 , *track3 ) ;
     Hlt::MergeInfo ( *tr2 , *track3 ) ;          
  }        
  ///
  LHCb::Track* track = track3.release() ;
  if ( 0 == track->parent() ) 
  { storedTracks ( address() )->insert ( track ); }
  //
  // do not put "bad" match into stream 
  if ( !m_config ( track )  ) { return 0 ; }                    // RETURN 
  //
  return track ;                                                // RETURN 
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
  if ( 0 == track || 0 == cand ) { return 0 ; }                       // RETURN
  //
  // it is track <--> track match! 
  if ( !m_match2 ) 
  {
    const LHCb::Track* track2  = cand->get<LHCb::Track> () ;
    if ( 0 == track2  ) { return 0 ; }                                // RETURN
    return match ( track , track2 ) ;                                 // RETURN 
  }
  //
  /// get new track 
  LHCb::Track* track3 = new LHCb::Track();
  // the actual track matching
  double q1 = 0 ;
  double q2 = 0 ;
  StatusCode sc = match2() -> match ( *track , *cand , *track3 , q1 , q2 ) ;
  if ( sc.isFailure() ) { return 0 ; }                                // RETURN
  
  // move info ? 
  if ( moveInfo () ) 
  {
     Hlt::MergeInfo ( *track , *track3 ) ;
  }        
  ///
  if ( 0 == track3->parent() ) 
  { storedTracks ( address() )->insert ( track3 ); }
  //
  // do not put "bad" match into stream 
  if ( !m_config ( track3 )  ) { return 0 ; }                         // RETURN
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
  //
  if ( 0 == track || 0 == cand ) { return false  ; }                   // RETURN
  //
  Assert ( !(!match2()) , "Invalid setup!" ) ;
  //
  // it is track <--> track match! 
  double q1 = 0;
  double q2 = 0;
  return match2() -> match ( track, cand, q1, q2 );
  //
}

// ============================================================================
// The END 
// ============================================================================
