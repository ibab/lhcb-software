///
/// initial coding by W.Pokorski Feb 2002, based on Particle2Particle by I.Belyaev
/// 
// ============================================================================
#ifndef GIGACNV_GENPART2GENPART_H 
#define GIGACNV_GENPART2GENPART_H 1
// ============================================================================
/// STD & STL 
#include <functional>
/// GiGaCnv 
#include "GenPart2Definition.h"
/// forward declarations 
class IParticlePropertySvc;
class GenParticle;
class G4PrimaryParticle;


class GenPart2GenPart: 
  public std::unary_function<const HepMC::GenParticle*,G4PrimaryParticle*> 
{
public:
  
  /** standard constructor
   *  @param Svc pointer to particle property service 
   */
  GenPart2GenPart( IParticlePropertySvc* Svc );
  /// Destructor
  ~GenPart2GenPart( ); 
  
  /** convert GenParticle object into G4Primary particle object
   *  @param particle pointer to GenParticle object
   *  @return pointer to new G4Primary particle
   */
  G4PrimaryParticle* operator() ( const HepMC::GenParticle* particle ) const ;
  
protected:
  
  /** get the particle definition for given particle 
   *  @param particle pointer to GenParticle object
   *  @return Geant4 particle definition 
   */
  inline G4ParticleDefinition* 
  definition( const HepMC::GenParticle* particle ) const 
  { return m_p2d( particle ); }
  
private:
  
  /// particle to particle definition converter
  GenPart2Definition  m_p2d;
  
};

// ============================================================================
#endif /// < GIGACNV_GENPART2GENPART_H
// ============================================================================
