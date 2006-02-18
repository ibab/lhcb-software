// $Id: HepMC2MC.h,v 1.3 2006-02-18 18:15:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/09 17:52:34  ibelyaev
//  regular update: add *.opts configuration files
//
// ============================================================================
#ifndef MCTOOLS_HEPMC2MC_H 
#define MCTOOLS_HEPMC2MC_H 1
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================


namespace LHCb
{
  namespace Relations 
  {
    /// the actual type for HepMC  -->    MC relations 
    typedef IRelation<HepMC::GenParticle*,LHCb::MCParticle>    HepMC2MC   ;
    /// the actual type for    MC  --> HepMC relations 
    typedef IRelation<LHCb::MCParticle,HepMC::GenParticle*>    MC2HepMC   ;
    /// the actual type for HepMC <-->    MC relations 
    typedef IRelation2D<HepMC::GenParticle*,LHCb::MCParticle>  HepMC2MC2D ;
  };
  
  /// the actual type for HepMC  -->    MC relations 
  typedef LHCb::Relations::HepMC2MC HepMC2MC   ;
  /// the actual type for    MC  --> HepMC relations 
  typedef LHCb::Relations::MC2HepMC     MC2HepMC   ;
  /// the actual type for HepMC <-->    MC relations 
  typedef LHCb::Relations::HepMC2MC2D   HepMC2MC2D ;
  
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
    const std::string Default = "Gen/HepMC2MC" ;
    // ========================================================================
  };
  // ==========================================================================
  
} ; // end of namespace LHCb

// ============================================================================
// The END 
// ============================================================================
#endif // MCTOOLS_HEPMC2MC_H
// ============================================================================
