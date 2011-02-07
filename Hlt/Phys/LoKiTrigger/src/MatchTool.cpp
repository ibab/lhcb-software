// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
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
  , m_qualityID  ( 0       ) 
  , m_qualityID2 ( 0       )
  , m_match      (         )
  , m_recoID     ( 0       )
// 
  , m_invert     ( false   )
{  
  // retrive the tool 
  m_match = alg()->tool<ITrackMatch> ( mTool() ) ;
  //
  /// get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
  { // recoID 
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , mTool()  );
    Assert( info , " request for unknown Info ID : " + mTool() );
    //
    m_recoID = info->second ;
  }
  //
  m_qualityID = 0 ;
  if ( !quality().empty() ) 
  {
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , quality() );
    Assert( info , " request for unknown Info ID : " + quality() );
    //
    m_qualityID  = info->second ; 
  }
  //
  m_qualityID2 = 0 ;
  if ( !quality2().empty() ) 
  {
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , quality2() );
    Assert( info , " request for unknown Info ID : " + quality2() );
    //
    m_qualityID2  = info->second ; 
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
  if ( sc.isFailure() ) { return 0 ; }                              // CONTINUE 
  
  // good matching ?
  if ( q1 < maxQualityCut() && 
       q2 < maxQuality2Cut() ) 
  {
    if ( 0 != qualityID  () ) { track3 -> addInfo ( qualityID  () , q1 ) ; }
    if ( 0 != qualityID2 () ) { track3 -> addInfo ( qualityID2 () , q2 ) ; }
    // move info ? 
    if ( moveInfo () ) 
    {
      Hlt::MergeInfo ( *tr1 , *track3 ) ;
      Hlt::MergeInfo ( *tr2 , *track3 ) ;          
    }        
    ///
    LHCb::Track* track = track3.release() ;
    storedTracks ( address() )->insert ( track );
    //
    return track ;
  }
  // ======================================================================
  return 0 ;
}
// ============================================================================
// The END 
// ============================================================================
