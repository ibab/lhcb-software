// $Id: ParticleProperties.h,v 1.4 2006-11-25 19:12:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//  $Log: not supported by cvs2svn $
//  Revision 1.3  2006/05/02 14:29:10  ibelyaev
//   censored
//
// ============================================================================
#ifndef LOKI_PARTICLEPROPERTIES_H 
#define LOKI_PARTICLEPROPERTIES_H 1
// ============================================================================
// Include files
// ============================================================================
// Kernel 
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// forward declarations
// ============================================================================
class ParticleProperty ;
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
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /** @namespace LoKi::Particles LoKi/ParticleProperties.h
   *  Simple namespace with functio related to particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  Particles
  {
    /** @var s_InvalidPIDName 
     *  representation of "invalid" particle ID 
     */
    const std::string s_InvalidPIDName = "<Unknown>" ;
    
    /** @fn pidFromName 
     *  retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle ID 
     */
    LHCb::ParticleID pidFromName ( const std::string& name ) ;
    
    /** @fn nameFromPID 
     *  retrieve particle name for given PID 
     *  @param pid particle PID 
     *  @return particle name 
     */
    std::string  nameFromPID( const LHCb::ParticleID& pid ) ;
    
    /** @fn _ppFromName 
     *  retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle property 
     */
    const ParticleProperty* _ppFromName ( const std::string& name ) ;

    /** @fn ppFromName 
     *  retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle property 
     */
    const ParticleProperty* ppFromName ( const std::string& name ) ;
    
    /** @fn _ppFromPID 
     *  retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @param particle property 
     */
    const ParticleProperty* _ppFromPID  ( const LHCb::ParticleID& pid   ) ;
    
    /** @fn ppFromPID 
     *  retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @param particle property 
     */
    const ParticleProperty* ppFromPID  ( const LHCb::ParticleID& pid   ) ;
    
    /** @fn massFromPID 
     *  retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @param particle mass 
     */
    const double massFromPID  ( const LHCb::ParticleID& pid   ) ;
    
    /** @fn massFromName 
     *  retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @param particle property 
     */
    const double massFromName ( const std::string&      name ) ;
    
    /** @fn lifeTime 
     *  retrieve the lifetime (c*tau) for the particle form the name 
     *  @param name particle name 
     */
    const double lifeTime     ( const std::string&      name ) ;
    
    /** @fn lifeTime 
     *  retrieve the lifetime (c*tau) for the particle form the name 
     *  @param name particle name 
     */
    const double lifeTime     ( const LHCb::ParticleID& pid ) ;
    
    /** @fn antiParticle 
     *  get name of 'antiparticle'
     *  @param name particlre name 
     *  @return anme of antiParticle 
     */
    std::string             antiParticle( const std::string&      name ) ;
    
    /** @fn antiParticle 
     *  get ParticleProperty of 'antiparticle'
     *  @param  pp ParticleProperty object for the particle 
     *  @return ParticleProperty obejct for antiparticle 
     */
    const ParticleProperty* antiParticle( const ParticleProperty* pp   ) ;
    
    /** @fn antiParticle 
     *  get PArticle ID for antiparticle 
     *  @param  pid particle 
     *  @return pid fro antiparticle 
     */
    LHCb::ParticleID     antiParticle( const LHCb::ParticleID&       pid  ) ;

  }
}

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLEPROPERTIES_H
// ============================================================================
