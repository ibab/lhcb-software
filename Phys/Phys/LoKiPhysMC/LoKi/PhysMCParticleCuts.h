// $Id: PhysMCParticleCuts.h,v 1.2 2006-11-27 11:56:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
     *  Helper and useful fucnction to eb used if some 
     *  LHCb::Paricle "mathced" wit some LHCb::MCParticle
     *
     *  @code 
     * 
     *  // some sequence of mc-particle
     *  SEQUENCE mcps = ...  ;
     *  // get mc-matcher 
     *  MCMatch   mc =  ... ;
     *  // create the function 
     *  Cut cut = MCTRUTH( mc , mcb.begin() , mcps.begin() , mcps.end() ) ;
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
    /** @typedef RCTRUTH
     *  Helper and useful fucnction to eb used if some 
     *  LHCb::Paricle "mathced" wit some LHCb::MCParticle
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
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
