// $Id: RC2HepMC.h,v 1.1.1.1 2006-10-11 13:35:09 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/10/04 12:07:45  ibelyaev
//  add LHCb::Particle -> HepMC::GenParticle relations and maker
//
// ============================================================================
#ifndef LOKIPHYSMC_RC2HEPMC_H 
#define LOKIPHYSMC_RC2HEPMC_H 1
// ============================================================================
// Include files
// ============================================================================ 
// Relations 
// ============================================================================
template <class FROM,class TO> class IRelation   ;
template <class FROM,class TO> class IRelation2D ;
// ============================================================================
// Event & HepMC 
// ============================================================================
namespace HepMC { class GenParticle ; }
namespace LHCb  { class    Particle ; }
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
 *  @date 2006-09-26
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
    /// the actual type for RC  -->   HepMC relations
    typedef IRelation<LHCb::Particle,HepMC::GenParticle*>      RC2HepMC   ;
    /// the actual type for RC  -->   HepMC relations
    typedef IRelation<HepMC::GenParticle*,LHCb::Particle>      HepMC2RC   ;
    /// the actual type for RC  <-->  HepMC relations
    typedef IRelation2D<LHCb::Particle,HepMC::GenParticle*>    RC2HepMC2D ;
  }
  /// the actual type for HepMC  -->    MC relations
  typedef LHCb::Relations_::HepMC2RC     HepMC2RC   ;
  /// the actual type for    RC  --> HepMC relations
  typedef LHCb::Relations_::RC2HepMC     RC2HepMC   ;
  /// the actual type for    RC <--> HepMC relations
  typedef LHCb::Relations_::RC2HepMC2D   RC2HepMC2D ;
}

  


// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_RC2HEPMC_H
// ============================================================================
