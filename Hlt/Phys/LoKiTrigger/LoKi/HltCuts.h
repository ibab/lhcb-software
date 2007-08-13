// $Id: HltCuts.h,v 1.1 2007-08-13 13:36:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTCUTS_H 
#define LOKI_HLTCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltFunctions.h"
// ============================================================================

namespace LoKi
{
  namespace Cuts
  {
    // ========================================================================
    /** @var TrDE
     *  The evaluator of "delta E" for the track
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Tracks::DeltaE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::DeltaE                                           TrDE ;
    // ========================================================================
    /** @var TTrDA
     *  The evaluator of "delta angle" between two tracks 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Tracks::DeltaAngle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::DeltaAngle                                      TTrDA ;
    // ========================================================================
    /** @var TTrDOCA
     *
     *  Evaluator of the distance of the closest approach between 2 tracks:
     *
     *  @code
     *
     *    const LHCb::Track& track1 = ... ;
     *    const LHCb::Track& track2 = ... ;
     *
     *    double doca = TrDOCA ( track1 , track2 ) ;
     *
     *  @endcode
     *
     *  @see HltUtils::closestDistanceMod 
     *  @see LoKi::Tracks::DistanceOfClosestApproach
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::DistanceOfClosestApproach                     TTrDOCA ;
    // ========================================================================
    /** @var TTrDP 
     *  The evaluator of "delta P" between two tracks 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Tracks::DeltaP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::DeltaP                                          TTrDP ;
    // ========================================================================
    /** @typedef TTrMATCH
     *  Simple evaluator of "track-matching"
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see ITrackMatch
     *  @see LoKi::Tracks::TrackMatch
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    typedef LoKi::Tracks::TrackMatch                                 TTrMATCH ;
    // ========================================================================
    /** @typedef TTrMATCHIDS
     *  Simple evaluator of "track-matching-id-fraction"
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see LoKi::Tracks::MatchIDsFraction
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    typedef LoKi::Tracks::MatchIDsFraction                        TTrMATCHIDS ;
    // ========================================================================
    /** @var TrVIP 
     *  Evaluator of the impact parameter of the track with respect to the vertex 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::impactParam
     *  @see LoKi::Tracks::TrMATCH
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::ImpactParameter                                 TrVIP ;
    // ========================================================================
    /** @var TrVRIP
     *  Evaluator of "radial" impact parameter of the track 
     *  with respect to the vertex 
     *  @attention track needs to be of Velo RZ type
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::rImpactParam
     *  @see LoKi::Tracks::TrMATCH
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::RadialIP                                       TrVRIP ;
    // ========================================================================


    // ========================================================================
    /** @var RVMAXPT
     *  The evaluator of "maxPt" for the recontructed vertex
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::maxPT
     *  @see LoKi::Tracks::MaxPt
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::MaxPt                                         RVMAXPT ;
    // ========================================================================
    /** @var RVMINPT
     *  The evaluator of "minPt" for the recontructed vertex
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::minPT
     *  @see LoKi::Tracks::MinPt
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::MinPt                                         RVMINPT ;
    // ========================================================================
    /** @var RVVDZ
     *  Evaluator of "delta-Z" distance between two vertices 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Tracks::DeltaZ
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::DeltaZ                                          RVVDZ ;
    // ========================================================================
    /** @var RVVFC 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::FC
     *  @see LoKi::Tracks::FC
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */        
    const LoKi::Tracks::FC                                              RVVFC ;
    // ========================================================================
    /** @var RVVFC2
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::FC2
     *  @see LoKi::Tracks::FC2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */        
    const LoKi::Tracks::FC2                                            RVVFC2 ;
    // ========================================================================

  } // end of namespace LoKi::Cuts 
} // end of namepspace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTCUTS_H
// ============================================================================
