// $Id: RC2HepMC.h,v 1.2 2008-06-03 15:24:15 cattanem Exp $
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-09-26
 */
// ============================================================================

namespace LHCb
{
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
