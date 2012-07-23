// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2006/01/31 10:34:15  gcorti
/// adapt to changes in event model
///
/// Revision 1.2  2001/08/12 17:24:55  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_PARTICLE2PARTICLE_H 
#define GIGACNV_PARTICLE2PARTICLE_H 1
// ============================================================================
/// LHCbEvent 
#include "Event/MCParticle.h"
/// STD & STL 
#include <functional>
/// GiGaCnv 
#include "Particle2Definition.h"
/// forward declarations 
namespace LHCb {
  class IParticlePropertySvc;
}
class G4PrimaryParticle;

/** @class Particle2Particle Particle2Particle.h 
 *  
 *  a helper auxiallary class to performconversion between
 *  MCParticle and G4PrimaryParticle objects 
 *
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */

class Particle2Particle: 
  public std::unary_function<const LHCb::MCParticle*,G4PrimaryParticle*> 
{
public:
  
  /** standard constructor
   *  @param Svc pointer to particle property service 
   */
  Particle2Particle( LHCb::IParticlePropertySvc* Svc );
  /// Destructor
  ~Particle2Particle( ); 
  
  /** convert MCParticle object into G4Primary particle object
   *  The method is recursive!
   *  @param particle pointer to MCParticle object
   *  @return pointer to new G4Primary particle
   */
  G4PrimaryParticle* operator() ( const LHCb::MCParticle* particle ) const ;
  
protected:
  
  /** get the particle definition for given particle 
   *  @param particle pointer to MCParticle object
   *  @return Geant4 particle definition 
   */
  inline G4ParticleDefinition* 
  definition( const LHCb::MCParticle* particle ) const 
  { return m_p2d( particle ); }
  
private:
  
  /// particle to particle definition converter 
  Particle2Definition  m_p2d;
  
};

// ============================================================================
#endif /// < GIGACNV_PARTICLE2PARTICLE_H
// ============================================================================
