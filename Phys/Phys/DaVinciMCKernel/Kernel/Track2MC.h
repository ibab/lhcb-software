// $Id: Track2MC.h,v 1.2 2008-06-03 15:24:15 cattanem Exp $
// ============================================================================
#ifndef KERNEL_TRACK2MC_H 
#define KERNEL_TRACK2MC_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/Track.h"
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
 *  @date 2006-08-17 
 */
// ============================================================================
namespace LHCb
{
  namespace Relations_
  {
    /// the actual type of Track   -> MC relations 
    typedef IRelationWeighted<LHCb::Track,LHCb::MCParticle,double> 
    Track2MC ;
    /// the actual type of Track   -> PV relations 
    typedef IRelationWeighted<LHCb::MCParticle,LHCb::Track,double> 
    MC2Track ;
    /// the actual type of Track <--> MC relations 
    typedef IRelationWeighted2D<LHCb::Track,LHCb::MCParticle,double> 
    Track2MC2D ;
  } ;
  /// the actual type of PV   -> MC relations 
  typedef LHCb::Relations_::Track2MC                              Track2MC    ;  
  /// the actual type of MC   -> Track relations 
  typedef LHCb::Relations_::MC2Track                              MC2Track    ;  
  /// the actual type of Track <--> MC relations 
  typedef LHCb::Relations_::Track2MC2D                            Track2MC2D  ;
  // ==========================================================================
  namespace Track2MCLocation
  {  
    const std::string Default = "Relations/Rec/Track/Default"     ;
    const std::string One2One = "Relations/Rec/Track/Default1to1" ;
  } // end of namespace Track2MCLocation
  //
} // end of namespace LHCb
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_TRACK2MC_H
// ============================================================================
