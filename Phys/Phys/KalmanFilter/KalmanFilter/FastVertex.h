// $Id: $
// ============================================================================
#ifndef KALMANFILTER_FASTVERTEX_H 
#define KALMANFILTER_FASTVERTEX_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Vector3DTypes.h"
// ============================================================================
// DaVinciTypes 
// ============================================================================
#include "Kernel/RecVertexHolder.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb
{
  class Track       ;  // Event/TrackEvent
  class VertexBase  ;  // Event/RecEvent
  class RecVertex   ;  // Event/RecEvent
}
// ============================================================================
/** @file 
 *
 *  Utilities for fast vertex fitting & related math 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2010-12-02
 *  
 *                    $Revision: 114335 $
 *  Last modification $Date: 2010-12-03 10:13:51 +0100 (Fri, 03 Dec 2010) $
 *                 by $Author: ibelyaev $
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace FastVertex 
  {
    // ========================================================================
    /** make 2-prong vertex from two tracks 
     *  @param track1  (INPUT) the first  track
     *  @param track2  (INPUT) the second track
     *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param doca    (OUTPUT) value of      DOCA   
     *  @param chi2    (OUTPUT) value of chi2(DOCA) 
     *  @param iterate (INPUT) iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    LHCb::RecVertexHolder makeVertex 
    ( const LHCb::Track*   track1           , 
      const LHCb::Track*   track2           ,
      const double         docamax          ,
      const double         chi2max          , 
      double&              doca             ,
      double&              chi2             , 
      const bool           iterate  = false ) ;
    // ========================================================================
    /** make 2-prong vertex from two tracks 
     *  @param track1  (INPUT) the first  track
     *  @param track2  (INPUT) the second track
     *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT) iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    LHCb::RecVertexHolder makeVertex 
    ( const LHCb::Track*   track1           , 
      const LHCb::Track*   track2           ,
      const double         docamax          ,
      const double         chi2max          , 
      const bool           iterate  = false ) ;
    // ========================================================================
    /** evaluate the distance between the track & the vertex ("impact-parameter")
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param impact  (OUTPUT) impact parameter  
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    StatusCode distance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          , 
      Gaudi::XYZVector&       impact          , 
      const bool              iterate = false ) ;
    // ========================================================================
    /** evaluate the distance between the track & the vertex ("impact-parameter")
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param impact  (OUTPUT) impact parameter  
     *  @param chi2    (OUTPUT) chi2 
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    StatusCode distance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          , 
      double&                 impact          , 
      double&                 chi2            , 
      const bool              iterate = false ) ;
    // ========================================================================
    /** evaluate the distance between the tracks ("DOCA")
     *  @param track1  (INPUT)  the first  track 
     *  @param track2  (INPUT)  the second track 
     *  @param doca    (OUTPUT) DOCA
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    StatusCode distance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          , 
      double&                 doca            , 
      const bool              iterate = false ) ;
    // =======================================================================
    /** evaluate the distance between the tracks ("DOCA")
     *  @param track1  (INPUT)  the first  track 
     *  @param track2  (INPUT)  the second track 
     *  @param doca    (OUTPUT) DOCA
     *  @param chi2    (OUTPUT) chi2(DOCA)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    StatusCode distance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          , 
      double&                 doca            , 
      double&                 chi2            , 
      const bool              iterate = false ) ;
    // =======================================================================
    /** check the distance between the track & the vertex ("impact-parameter")
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param ipmax   (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    bool checkDistance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          ,
      const double            ipmax           , 
      const double            chi2max         ,
      const bool              iterate = false ) ;
    // ========================================================================
    /** check the distance between the track & the track ("doca")
     *  @param track1  (INPUT) the first track  
     *  @param track2  (INPUT) the second track  
     *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    GAUDI_API 
    bool checkDistance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          ,
      const double            docamax         , 
      const double            chi2max         ,
      const bool              iterate = false ) ;
    // ========================================================================
  } //                                        end of namespace LoKi::FastVertex 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KALMANFILTER_FASTVERTEX_H
// ============================================================================
