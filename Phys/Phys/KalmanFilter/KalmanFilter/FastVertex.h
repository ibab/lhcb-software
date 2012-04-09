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
// LHCbMath 
// ============================================================================
#include "LHCbMath/Line.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2010-12-02
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class FastVertex
   *  Collection of utilities for "Fast"-verticing from track 
   *  @see LHCb::Track
   *  @see LHCb::RecVertex
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-01-14
   */
  class GAUDI_API FastVertex 
  {
    // ========================================================================
  public:
    // ========================================================================
    /// get the actual type of the line  
    typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>          Line ;
    // ========================================================================
  public:
    // ========================================================================
    /** make 2-prong vertex from two tracks 
     *
     *  @code 
     * 
     *  const LHCb::Track* track1 = ... ;
     *  const LHCb::Track* track2 = ... ;
     *
     *  double doca      ;
     *  double docachi2  ;
     *
     *  // fast vertex creation, no actual vertex fit, 
     *  LHCb::RecVertexHolder vx1 = LoKi::FastVertex::makeVertex (
     *                 track1 ,     // the first track
     *                 track2 ,     // the second tarck 
     *                 -1     ,     // no doca cut
     *                 -1     ,     // no chi2 cut - NO FIT!!!
     *                 doca   ,     // retrieve doca 
     *                 docachi2 ) ; // retrieve chi2(doca) 
     * 
     *  const LHCb::RecVertex* vertex1 = 0 ;
     *  if ( doca < 50 * Gaudi::Units::micrometer ) {  vertex1 = vx1.release() ; }
     *
     *  // fast vertex creation with vertex fit!
     *  LHCb::RecVertexHolder vx2 = LoKi::FastVertex::makeVertex (
     *                 track1 ,     // the first track
     *                 track2 ,     // the second tarck 
     *                 100 * Gaudi::Units::micrometer , // doca cut
     *                 16     ,     // chi2 cut : perform vertex FIT!!!
     *                 doca   ,     // retrieve doca 
     *                 docachi2 ) ; // retrieve chi2(doca) 
     * 
     *  const LHCb::RecVertex* vertex2 = vx2.release() ;
     *
     *  @endcode 
     *
     *  @attention LHCb::RecVertexHoder has destructive copy semantics! 
     *             It deletes the vertex! To keep the vertex form deletion, 
     *             one needs either register it in Transient Store, or
     *             release it explicitly 
     *
     *  @see LHCb::RecVertexHolder 
     *  @see std::auto_ptr 
     * 
     *  @attention For "no-fit" case, the vertex is defined as 
     *                  middle point of DOCA-vector, 
     *                  chi2(vertex) is set to -1000, and 
     *                  covarinance matrix is not defined 
     * 
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
    static 
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
     *  @code 
     * 
     *  const LHCb::Track* track1 = ... ;
     *  const LHCb::Track* track2 = ... ;
     *
     *  double doca      ;
     *  double docachi2  ;
     *
     *  // fast vertex creation, no actual vertex fit, 
     *  LHCb::RecVertexHolder vx1 = LoKi::FastVertex::makeVertex (
     *                 track1 ,     // the first track
     *                 track2 ,     // the second tarck 
     *                 100 * Gaudi::Units::micrometer , // doca cut
     *                 -1     )   ; // no chi2 cut - NO FIT!!!
     * 
     *  // fast vertex creation with vertex fit!
     *  LHCb::RecVertexHolder vx2 = LoKi::FastVertex::makeVertex (
     *                 track1 ,     // the first track
     *                 track2 ,     // the second tarck 
     *                 100 * Gaudi::Units::micrometer , // doca cut
     *                 16     ) ;   // chi2 cut : perform vertex FIT!!!
     * 
     *  @endcode 
     *
     *  @attention LHCb::RecVertexHoder has destructive copy semantics! 
     *             It deletes the vertex! To keep the vertex form deletion, 
     *             one needs either register it in Transient Store, or
     *             release it explicitly 
     *
     *  @see LHCb::RecVertexHolder 
     *  @see std::auto_ptr 
     * 
     *  @attention For "no-fit" case, the vertex is defined as 
     *                  middle point of DOCA-vector, 
     *                  chi2(vertex) is set to -1000, and 
     *                  covarinance matrix is not defined 
     * 
     *  @param track1  (INPUT) the first  track
     *  @param track2  (INPUT) the second track
     *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT) iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    LHCb::RecVertexHolder makeVertex 
    ( const LHCb::Track*   track1           , 
      const LHCb::Track*   track2           ,
      const double         docamax          ,
      const double         chi2max          , 
      const bool           iterate  = false ) ;
    // ========================================================================
    /** evaluate the vector distance between the track & the vertex 
     *  ("vector impact-parameter")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track   = ... ;
     *   const LHcb::VertexBase* vertex  = ... ;
     *
     *   Gaudi::XYZVector        impact ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( track  ,   // the track 
     *                       vertex ,   // the vertex 
     *                       impact ) ; // the impact parameter  
     * 
     *  @endcode 
     *
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param impact  (OUTPUT) impact parameter  
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          , 
      Gaudi::XYZVector&       impact          , 
      const bool              iterate = false ) ;
    // ========================================================================
    /** evaluate the distance between the track & the vertex ("impact-parameter")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track   = ... ;
     *   const LHcb::VertexBase* vertex  = ... ;
     *
     *   double ip     ;
     *   double chi2   ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( track  ,   // the track 
     *                       vertex ,   // the vertex 
     *                       ip     ,   // the impact parameter  
     *                       chi2   ) ; // chi2(ip)
     * 
     *  @endcode 
     *
     *  @attention For evaluation of chi2(ip) it is required to have valid 
     *             covariance marix for the vertex (and track)
     * 
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param impact  (OUTPUT) impact parameter  
     *  @param chi2    (OUTPUT) chi2 
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          , 
      double&                 impact          , 
      double&                 chi2            , 
      const bool              iterate = false ) ;
    // ========================================================================
    /** evaluate the distance between the tracks ("DOCA")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track1  = ... ;
     *   const LHCb::Track*      track2  = ... ;
     *
     *   double doca   ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( track1 ,   // the first track 
     *                       track2 ,   // the second track 
     *                       doca   ) ; // the distance 
     * 
     *  @endcode 
     *
     *  @param track1  (INPUT)  the first  track 
     *  @param track2  (INPUT)  the second track 
     *  @param doca    (OUTPUT) DOCA
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          , 
      double&                 doca            , 
      const bool              iterate = false ) ;
    // ========================================================================
    /** evaluate the distance between the track and the line 
     *
     *  @code 
     * 
     *   const LHCb::Track*      track   = ... ;
     *   const Line              line    = ... ;
     *
     *   double doca   ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( track  ,   // the track 
     *                       line   ,   // the line  
     *                       doca   ) ; // the distance 
     * 
     *  @endcode 
     *
     *  @param track1 (INPUT)  the track 
     *  @param line2  (INPUT)  the line 
     *  @param doca    (OUTPUT) DOCA
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const LHCb::Track*      track1          , 
      const Line&             line2           , 
      double&                 doca            , 
      const bool              iterate = false ) ;
    // =======================================================================
    /** evaluate the distance between the track and the line 
     *
     *  @code 
     * 
     *   const Line              line    = ... ;
     *   const LHCb::Track*      track   = ... ;
     *
     *   double doca   ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( line   ,   // the line  
     *                       track  ,   // the track 
     *                       doca   ) ; // the distance 
     * 
     *  @endcode 
     *
     *  @param line1   (INPUT)  the line 
     *  @param track2  (INPUT)  the track 
     *  @param doca    (OUTPUT) DOCA
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const Line&             line1           , 
      const LHCb::Track*      track2          , 
      double&                 doca            , 
      const bool              iterate = false ) ;
    // =======================================================================
    /** evaluate the distance between the tracks ("DOCA")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track1  = ... ;
     *   const LHCb::Track*      track2  = ... ;
     *
     *   double doca  ;
     *   double chi2  ;
     *
     *   StatusCode sc = LoKi::FastVertex::distance 
     *                     ( track1 ,   // the first track 
     *                       track2 ,   // the second track 
     *                       doca   ,   // the distance 
     *                       chi2   ) ; // chi2(doca) 
     * 
     *  @endcode 
     *
     *  @param track1  (INPUT)  the first  track 
     *  @param track2  (INPUT)  the second track 
     *  @param doca    (OUTPUT) DOCA
     *  @param chi2    (OUTPUT) chi2(DOCA)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    StatusCode distance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          , 
      double&                 doca            , 
      double&                 chi2            , 
      const bool              iterate = false ) ;
    // =======================================================================
    /** check the distance between the track & the vertex ("impact-parameter")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track   = ... ;
     *   const LHCb::VertexBase* vertex  = ... ;
     *
     *   const bool ok = LoKi::FastVertex::Checkdistance 
     *        ( track  ,   // the track 
     *          vertex ,   // the vertex 
     *          100 * Gaudi::Units::micrometer ,  // the impact parameter  
     *          100    ) ; // chi2(ip)
     * 
     *  @endcode 
     *
     *  @attention For evaluation of chi2(ip) it is required to have valid 
     *             covariance marix for the vertex (and track)
     * 
     *  @param track   (INPUT)  track 
     *  @param vertex  (INPUT)  vertex 
     *  @param ipmax   (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    bool checkDistance 
    ( const LHCb::Track*      track           , 
      const LHCb::VertexBase* vertex          ,
      const double            ipmax           , 
      const double            chi2max         ,
      const bool              iterate = false ) ;
    // ========================================================================
    /** check the distance between the track & the track ("doca")
     *
     *  @code 
     * 
     *   const LHCb::Track*      track1  = ... ;
     *   const LHCb::Track*      track2  = ... ;
     *
     *   const double ok  = LoKi::FastVertex::distance 
     *        ( track1 ,   // the first track 
     *          track2 ,   // the second track 
     *          100 * Gaudi::Units::micrometer ,  // the impact parameter  
     *          100    ) ; // chi2(ip)
     * 
     *  @endcode 
     *
     *  @param track1  (INPUT) the first track  
     *  @param track2  (INPUT) the second track  
     *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
     *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
     *  @param iterate (INPUT)  iterate?
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2010-12-03
     */
    static 
    bool checkDistance 
    ( const LHCb::Track*      track1          , 
      const LHCb::Track*      track2          ,
      const double            docamax         , 
      const double            chi2max         ,
      const bool              iterate = false ) ;
    // ========================================================================
  } ; //                                          end of class LoKi::FastVertex 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KALMANFILTER_FASTVERTEX_H
// ============================================================================
