///
/// initial coding by W.Pokorski Feb 2002, based on Particle2Definion by I.Belyaev
/// 
// ============================================================================
#ifndef GIGACNV_GENPART2DEFINITION_H 
#define GIGACNV_GENPART2DEFINITION_H 1
// ============================================================================
/// Include files
/// STD & STL 
#include <functional>
/// forward declarations 
class IParticlePropertySvc;
class GenParticle;
class G4ParticleDefinition;

class GenPart2Definition: 
  public std::unary_function<const HepMC::GenParticle*,G4ParticleDefinition*> 
{
public:
  
  /** Standard constructor
   *  @param Svc pointer to Particle Property Service 
   */ 
  GenPart2Definition( IParticlePropertySvc* Svc ); 
  
  /// destructor 
  ~GenPart2Definition(); 
  
  /** copy constructor
   *  @param right copy 
   */
  GenPart2Definition( const GenPart2Definition& right );

  /** get the Geant4 particle definition from GenParticle object
   *  @param particle pointer to GenParticle
   *  @return pointer to Geant4 particle definition 
   */
  G4ParticleDefinition* operator()( const HepMC::GenParticle* ) const;
  
protected:
  
  /** accessor to particle proeprty service 
   *  @return pointer to particle property service 
   */
  inline IParticlePropertySvc* ppSvc() const { return m_ppSvc; }

private:

  IParticlePropertySvc* m_ppSvc; ///< particle property service 

};

// ============================================================================
#endif ///< GIGACNV_GENPART2DEFINITION_H
// ============================================================================
