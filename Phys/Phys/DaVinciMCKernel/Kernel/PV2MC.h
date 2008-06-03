// $Id: PV2MC.h,v 1.4 2008-06-03 15:24:14 cattanem Exp $
// ============================================================================
#ifndef KERNEL_PV2MC_H 
#define KERNEL_PV2MC_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/GenCollision.h"
#include "Event/MCVertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// Forward Declarations 
// ============================================================================
template <class FROM,class TO,class WEIGHT> class IRelationWeighted ;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted2D ;
// ============================================================================
/** @file
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
 *  @date 2006-03-18 
 */
// ============================================================================
namespace LHCb
{
  namespace Relations_
  {
    /// the actual type of "weight" (pair: number of tracks + chi2)
    typedef std::pair<unsigned int,float>                    PV2MCWeight ;
    /// the actual type of PV   -> MC relations 
    typedef IRelationWeighted<LHCb::RecVertex,LHCb::MCVertex,PV2MCWeight> 
    PV2MC ;
    /// the actual type of MC   -> PV relations 
    typedef IRelationWeighted<LHCb::MCVertex,LHCb::RecVertex,PV2MCWeight> 
    MC2PV ;
    /// the actual type of PV <--> MC relations 
    typedef IRelationWeighted2D<LHCb::RecVertex,LHCb::MCVertex,PV2MCWeight> 
    PV2MC2D ;
    /// the actual type of PV   -> Collision relations 
    typedef IRelationWeighted<LHCb::RecVertex,LHCb::GenCollision,PV2MCWeight> 
    PV2Collision ;
    /// the actual type of Collision -> PV relations 
    typedef IRelationWeighted<LHCb::GenCollision,LHCb::RecVertex,PV2MCWeight> 
    Collision2PV ;
    /// the actual type of PV <--> Collision relations 
    typedef IRelationWeighted2D<LHCb::RecVertex,LHCb::GenCollision,PV2MCWeight> 
    PV2Collision2D ;    
    
  } ;
  /// the actual type of "weight" for PV   -> MC relations 
  typedef LHCb::Relations_::PV2MCWeight                            PV2MCWeight ;  
  /// the actual type of PV   -> MC relations 
  typedef LHCb::Relations_::PV2MC                                  PV2MC       ;  
  /// the actual type of MC   -> PV relations 
  typedef LHCb::Relations_::MC2PV                                  MC2PV       ;  
  /// the actual type of PV <--> MC relations 
  typedef LHCb::Relations_::PV2MC2D                                PV2MC2D     ;
  /// the actual type of PV   -> Collision relations 
  typedef LHCb::Relations_::PV2Collision                           PV2Collision ;  
  /// the actual type of Collision -> PV relations 
  typedef LHCb::Relations_::Collision2PV                           Collision2PV ;  
  /// the actual type of PV <--> Collision  relations 
  typedef LHCb::Relations_::PV2Collision2D                         PV2Collision2D ;
  
  // ==========================================================================
  /** @namespace PV2MCLocation
   *  namespace with the defintion of "default" TES location for 
   *  LHCb::PrimVertex  <---> LHCb::MCVertex relation table 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  // ==========================================================================
  namespace PV2MCLocation
  {
    // ========================================================================
    /** @var Default 
     *  The definition of the "default" TES location 
     *  for LHCb::RecVertex  <---> LHCb::MCVertex relation table 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    // ========================================================================
    const std::string Default = "Relations/" + LHCb::RecVertexLocation::Primary ;
    // ========================================================================
  };
  // ==========================================================================
  /** @namespace PV2CollisionLocation
   *  namespace with the defintion of "default" TES location for 
   *  LHCb::RecVertex  <---> LHCb::GenCollision relation table 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  // ==========================================================================
  namespace PV2CollisionLocation
  {
    // ========================================================================
    /** @var Default 
     *  The definition of the "default" TES location 
     *  for LHCb::RecVertex  <---> LHCb::GenCollision relation table 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    // ========================================================================
    const std::string Default = 
    "Relations/" + LHCb::RecVertexLocation::Primary + "2Collision";
    // ========================================================================
  }; // end of namespace LHCb::PV2CollisionLocation  
  // ==========================================================================
  
} ; // end of namespace LHCb
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_PV2MC_H
// ============================================================================
