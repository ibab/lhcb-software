// $Id: PhysMCParticleCuts.h,v 1.3 2007-01-20 14:47:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/11/27 11:56:15  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.1.1.1  2006/03/14 19:12:21  ibelyaev
// New package : RC <---> MC links for LoKi 
// 
// ============================================================================
#ifndef LOKI_PHYSMCPARTICLECUTS_H 
#define LOKI_PHYSMCPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/PhysMCParticles.h"
// ============================================================================

namespace LoKi
{
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MCTRUTH
     *
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some LHCb::MCParticle
     *
     *  @code 
     * 
     *  // some sequence of mc-particle
     *  SEQUENCE mcps = ...  ;
     *  // get mc-matcher 
     *  MCMatch   mc =  ... ;
     *  // create the function 
     *  Cut cut = MCTRUTH( mc , mcps.begin() , mcps.end() ) ;
     *  
     *
     *  const LHCb::Particle* B = ... ;
     *  // use the predicate!
     * 
     *  const bool good = cut( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-12
     */
    typedef LoKi::Particles::MCTruth                              MCTRUTH ;
    // ========================================================================

    // ========================================================================
    /** @typedef GMCTRUTH
     *
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some HepMC::GenParticle
     *
     *  @code 
     * 
     *  // some sequence of HepMC-particles
     *  SEQUENCE hepmcps = ...  ;
     *  // get RC<-->MC-matcher 
     *  MCMatch   mc =  ... ;
     *  // get HepMC<-->MC matcher
     *  const LHCb::HepMC2MC* table = ... ;
     *  // create the function 
     *  Cut cut = GMCTRUTH( table , mc , hepmcps.begin() , hepmcps.end() ) ;
     *  
     *
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the predicate!
     * 
     *  const bool good = cut( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::HepMC2MC 
     *  @see LHCb::Particle
     *  @see HepMC::GenParticle
     *  @see LoKi::Particles::GenMCTruth
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-12
     */
    typedef LoKi::Particles::GenMCTruth                          GMCTRUTH ;
    // ========================================================================
    
    
    // ========================================================================
    /** @typedef RCTRUTH
     *
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some LHCb::MCParticle
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
     *  @see LoKi::MCParticles::RCTruth
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-12
     */
    typedef LoKi::MCParticles::RCTruth                           RCTRUTH ;
    // ========================================================================

  }  // end of the namespace LoKi::Cuts 
}  // end of the namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCPARTICLECUTS_H
// ============================================================================
