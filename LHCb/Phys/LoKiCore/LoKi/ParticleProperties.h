// ============================================================================
#ifndef LOKI_PARTICLEPROPERTIES_H 
#define LOKI_PARTICLEPROPERTIES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel 
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb { class ParticleProperty ; }
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
  // ==========================================================================
  /** @namespace LoKi::Particles LoKi/ParticleProperties.h
   *  Simple namespace with functio related to particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  Particles
  {
    // ========================================================================    
    /** retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle ID 
     */
    GAUDI_API 
    LHCb::ParticleID pidFromName ( const std::string& name ) ;
    // ========================================================================    
    /** retrieve particle name for given PID 
     *  @param pid particle PID 
     *  @return particle name 
     */
    GAUDI_API 
    std::string  nameFromPID( const LHCb::ParticleID& pid ) ;
    // ========================================================================    
    /** retrieve particle name for given PID 
     *  @param pid particle PID 
     *  @return particle name 
     */
    GAUDI_API 
    std::string  nameIdFromPID( const LHCb::ParticleID& pid ) ;
    // ========================================================================    
    /** retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle property 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* _ppFromName ( const std::string& name ) ;
    // ========================================================================
    /** retrieve particle ID from Particle name 
     *  @param name particle name 
     *  @return particle property 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* ppFromName ( const std::string& name ) ;
    // ========================================================================    
    /** retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @return particle property 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* _ppFromPID  ( const LHCb::ParticleID& pid   ) ;
    // ========================================================================    
    /** retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @return particle property 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* __ppFromPID ( const LHCb::ParticleID& pid   ) ;
    // ========================================================================    
    /** retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @return particle property 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* ppFromPID  ( const LHCb::ParticleID& pid   ) ;
    // ========================================================================    
    /** retrieve ParticleProperty from ParticleID 
     *  @param pid particle ID 
     *  @return particle mass 
     */
    GAUDI_API 
    double massFromPID  ( const LHCb::ParticleID& pid   ) ;
    // ========================================================================    
    /** retrieve ParticleProperty from ParticleID 
     *  @param name particle name 
     *  @return particle mass 
     */
    GAUDI_API 
    double massFromName ( const std::string&      name ) ;
    // ========================================================================    
    /** retrieve the lifetime (c*tau) for the particle from the name 
     *  @param name particle name 
     *  @return particle lifetime
     */
    GAUDI_API 
    double ctau             ( const std::string& name ) ;
    inline double lifeTime  ( const std::string& name ) 
    { return ctau ( name ) ; }
    // ========================================================================    
    /** retrieve the lifetime (c*tau) for the particle form the pid 
     *  @param pid particle ID 
     *  @return particle lifetime
     */
    GAUDI_API 
    double ctau            ( const LHCb::ParticleID& pid ) ;
    inline double lifeTime ( const LHCb::ParticleID& pid ) 
    { return ctau ( pid ) ; }
    // ========================================================================    
    /** get name of 'antiparticle'
     *  @param name particlre name 
     *  @return name of antiParticle 
     */
    GAUDI_API 
    std::string             antiParticle( const std::string&      name ) ;
    // ========================================================================    
    /** get ParticleProperty of 'antiparticle'
     *  @param  pp ParticleProperty object for the particle 
     *  @return ParticleProperty object for antiparticle 
     */
    GAUDI_API 
    const LHCb::ParticleProperty* antiParticle
    ( const LHCb::ParticleProperty* pp   ) ;
    // ========================================================================    
    /** get PArticle ID for antiparticle 
     *  @param  pid particle 
     *  @return pid fro antiparticle 
     */
    GAUDI_API 
    LHCb::ParticleID     antiParticle( const LHCb::ParticleID&       pid  ) ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLEPROPERTIES_H
// ============================================================================
