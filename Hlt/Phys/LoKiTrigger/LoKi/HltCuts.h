// $Id$
// ============================================================================
#ifndef LOKI_HLTCUTS_H 
#define LOKI_HLTCUTS_H 1
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltFunctions.h"
// ============================================================================
/** @file
 *  Definition of Hlt-specific functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *  $Revision$
 *  Last Modification $Date$ by $Author$ 
 */
namespace LoKi
{
  // ==========================================================================
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
    const LoKi::Tracks::DeltaE                                           TrDE{} ;
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
    const LoKi::Tracks::ImpactParameter                                 TrVIP{} ;
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
    const LoKi::Tracks::DeltaAngle                                      TTrDA{} ;
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
    const LoKi::Tracks::DistanceOfClosestApproach                     TTrDOCA{} ;
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
    const LoKi::Tracks::DeltaP                                          TTrDP{} ;
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



  } // end of namespace LoKi::Cuts 
} // end of namepspace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTCUTS_H
// ============================================================================
