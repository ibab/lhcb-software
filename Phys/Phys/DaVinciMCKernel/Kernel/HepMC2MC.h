// $Id: HepMC2MC.h,v 1.2 2008-06-03 15:24:14 cattanem Exp $
// ============================================================================
#ifndef MCTOOLS_HEPMC2MC_H 
#define MCTOOLS_HEPMC2MC_H 1
// ============================================================================
// Include files
// ============================================================================
// GeneEvent
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// forward declarations
// ============================================================================
// HepMC 
// ============================================================================
namespace HepMC { class GenParticle ; }
// ============================================================================
// Relations 
// ============================================================================
template <class FROM,class TO> class IRelation   ;
template <class FROM,class TO> class IRelation2D ;
// ============================================================================
// Event 
// ============================================================================
namespace LHCb { class MCParticle  ; }
// ============================================================================

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
 *  @date 2006-01-23 
 */
// ============================================================================


namespace LHCb
{
  /** @namespace LHCb::Relations_
   *  definition fo low-level classes used for relations
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-16
   */
  namespace Relations_ 
  {
    /// the actual type for HepMC  -->    MC relations 
    typedef IRelation<HepMC::GenParticle*,LHCb::MCParticle>    HepMC2MC   ;
    /// the actual type for    MC  --> HepMC relations 
    typedef IRelation<LHCb::MCParticle,HepMC::GenParticle*>    MC2HepMC   ;
    /// the actual type for HepMC <-->    MC relations 
    typedef IRelation2D<HepMC::GenParticle*,LHCb::MCParticle>  HepMC2MC2D ;
  };
  
  /// the actual type for HepMC  -->    MC relations 
  typedef LHCb::Relations_::HepMC2MC HepMC2MC   ;
  /// the actual type for    MC  --> HepMC relations 
  typedef LHCb::Relations_::MC2HepMC     MC2HepMC   ;
  /// the actual type for HepMC <-->    MC relations 
  typedef LHCb::Relations_::HepMC2MC2D   HepMC2MC2D ;
  
  // ==========================================================================
  /** @namespace HepMC2MCLocation
   *  namespace with the definiton of "default" TES 
   *  location for  HepMC::GenParticle <---> MCParticle relation table 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2005-07-14 
   */
  // ==========================================================================
  namespace HepMC2MCLocation 
  {
    // ========================================================================
    /** @var Default 
     *  The definiton of "default" TES 
     *  location for  HepMC::GenParticle <---> MCParticle relation table 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2005-07-14 
     */
    // ========================================================================
    const std::string Default = 
    "Relations/" + LHCb::HepMCEventLocation::Default ;
    // ========================================================================
  };
  // ==========================================================================
  
} ; // end of namespace LHCb

// ============================================================================
// The END 
// ============================================================================
#endif // MCTOOLS_HEPMC2MC_H
// ============================================================================
