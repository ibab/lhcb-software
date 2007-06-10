// $Id: GenMCParticleCuts.h,v 1.6 2007-06-10 20:04:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENMCPARTICLECUTS_H 
#define LOKI_GENMCPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiGenMC 
// ============================================================================
#include "LoKi/GenMCParticles.h"
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
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  namespace Cuts 
  {
    // ========================================================================
    /** Trivial predicate which evaluates to 'true'
     *  for (HepMC)particles which are "origins" for 
     *  given certain MCParticles 
     * 
     *  E.g. try to find all generator ancestor (HepMC) particles 
     *  which results lepton
     *  
     *   @code 
     *
     *   const LHCb::MCParticle* lepton = ... ; 
     *
     *   // get the relation table from TES 
     *   typedef LHCb::Relations::HepMC2MC Table ;
     *   const Table* table = 
     *         get<Table>( LHCb::HepMC2MCLocation::Default ) ;
     * 
     *   // locate LHCb::HepMCEvents from TES:
     *   const LHCb::HepMCEvents* events = 
     *      get<LHCb::HepMCEvents>( LHCb::HepMCEventLocation::Default ) ;
     * 
     *   // prepare an output container:
     *   typedef std::vector<const HepMC::GenParticle*> GParticles ;
     *   GParticles parts ;
     *  
     *   // create the selector functios:
     *   GCut cut = GMCMOTH( lepton , table ) ;
     *
     *   // select "origins" and save them into the output container
     *   LoKi::Extract::genParticles 
     *      ( events , std::back_inserter( parts ) , cut ) ;
     *
     *   @endcode 
     * 
     *  @see LHCb::MCParticle
     *  @see HepMC::GenParticle
     *  @see LHCb::Relations::HepMC2MC
     *  @see LoKi::GenMCParticles::IsAMotherForMC
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    typedef LoKi::GenMCParticles::IsAMotherForMC     GMCMOTH   ;
    // ========================================================================    
    /** trivial predicate which 
     *  evaluates to 'true' for MC-particles,
     *  which originates from certain HepMC particle 
     *
     *  E.g. try to find all MC particles from the Higgs: 
     *
     *  @code 
     *  
     *  // 1) get all HepMC particles:
     *  const LHCb::HepMCEvents* events = 
     *      get<LHCb::HepMCEvents>( LHCb::HepMCEventLocation::Default ) ;
     *
     *   // 2) select all Higgses:
     *   // prepare an output container:
     *   typedef std::vector<const HepMC::GenParticle*> GParticles ;
     *   GParticles higgses ;
     *   // select "higgses"
     *   LoKi::Extract::genParticles 
     *      ( events , std::back_inserter( higgses ) , GID = "H_10" ) ;
     * 
     *   // 3) locate  relation table from TES
     *   // get the relation table from TES 
     *   typedef LHCb::Relations::HepMC2MC2D Table2D ;
     *   typedef LHCb::Relations::MC2HepMC   Table   ;
     *   const Table2D* table2D = 
     *         get<Table>( LHCb::HepMC2MCLocation::Default ) ;
     *   const Table*   table = table2D->inverse() ;
     *
     *   // 4) create the selector funtor 
     *   MCCut cut = FROMGTREE ( higgses.begin () , higgses.end   () , table ) ; 
     *
     *   // 5a) select MC-particles (e.g. using LoKi::Algo):
     *  MCRange parts = mcselect( "parts" , cut ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::MCParticle
     *  @see LoKi::Relations::HepMC2MC
     *  @see LoKi::Relations::MC2HepMCMC
     *  @see LoKi::Relations::HepMC2MC2D
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    typedef LoKi::GenMCParticles::IsFromHepMC         FROMGTREE ;
    // ========================================================================
  }  // end of namespace LoKi::Cuts
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENMCPARTICLECUTS_H
// ============================================================================
