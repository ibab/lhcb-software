// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_PARTICLE2DEFINITION_H 
#define GIGACNV_PARTICLE2DEFINITION_H 1
// ============================================================================
/// Include files
/// STD & STL 
#include <functional>
/// forward declarations 
class IParticlePropertySvc;
class MCParticle;
class G4ParticleDefinition;

/** @class Particle2Definition Particle2Definition.h 
 *  
 *  A simple auxilalry class to provide G4ParticleDefinition
 *  for MCParticle object 
 *
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */
class Particle2Definition: 
  public std::unary_function<const MCParticle*,G4ParticleDefinition*> 
{
public:
  
  /** Standard constructor
   *  @param Svc pointer to Particle Property Service 
   */ 
  Particle2Definition( IParticlePropertySvc* Svc ); 
  
  /// destructor 
  ~Particle2Definition(); 
  
  /** copy constructor
   *  @param right copy 
   */
  Particle2Definition( const Particle2Definition& right );

  /** get the Geant4 particle definition from MCParticle object
   *  @param particle pointer to MCParticle
   *  @return pointer to Geant4 particel definition 
   */
  G4ParticleDefinition* operator()( const MCParticle* ) const;
  
protected:
  
  /** accessor to particle proeprty service 
   *  @return pointer to particle property service 
   */
  inline IParticlePropertySvc* ppSvc() const { return m_ppSvc; }

private:

  IParticlePropertySvc* m_ppSvc; ///< particle property service 

};

// ============================================================================
#endif ///< GIGACNV_PARTICLE2DEFINITION_H
// ============================================================================
