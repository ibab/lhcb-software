// $Id: Tracks.h 199592 2016-01-12 19:10:37Z sstahl $
// ============================================================================
#ifndef LOKI_TRACKFUNCTIONS_H 
#define LOKI_TRACKFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/State.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/ValueWithError.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Tracks
  {
    // ========================================================================
    /** get the transverse momentum (and uncertainty) from the state
     *  @param state (INPUT) the  state 
     *  @return transverse momentum (and uncertainty) from the given state 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2016-03-05
     */
    GAUDI_API
    Gaudi::Math::ValueWithError state_pt       ( const LHCb::State& state ) ;
    // ========================================================================
    /** get the phi angle (and uncertainty) from the state
     *  @param state (INPUT) the  state 
     *  @return phi angle  (and uncertainty) from the given state 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2016-03-05
     */
    GAUDI_API
    Gaudi::Math::ValueWithError state_phi      ( const LHCb::State& state ) ;
    // ========================================================================
    /** get the theta angle (and uncertainty) from the state
     *  @param state (INPUT) the  state 
     *  @return theta angle (and uncertainty) from the given state 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2016-03-05
     */
    GAUDI_API
    Gaudi::Math::ValueWithError state_theta    ( const LHCb::State& state ) ;
    // ========================================================================
    /** get the pseudorapidity (and uncertainty) from the state
     *  @param state (INPUT) the  state 
     *  @return pseudorapidity (and uncertainty) from the given state 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2016-03-05
     */
    GAUDI_API
    Gaudi::Math::ValueWithError state_eta      ( const LHCb::State& state ) ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRACKFUNCTIONS_H
// ============================================================================
