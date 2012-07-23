// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2006/01/31 10:34:15  gcorti
/// adapt to changes in event model
///
/// Revision 1.3  2002/04/24 14:50:30  ibelyaev
///  update for newer versions of Gaudi, LHCb and G4
///
/// Revision 1.2  2001/08/12 17:24:54  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_PARTICLE2DEFINITION_H 
#define GIGACNV_PARTICLE2DEFINITION_H 1
// ============================================================================
/// Include files
/// LHCbEvent 
#include "Event/MCParticle.h"
/// STD & STL 
#include <functional>
/// forward declarations 
namespace LHCb {
  class IParticlePropertySvc;
}
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
  public std::unary_function<const LHCb::MCParticle*,G4ParticleDefinition*> 
{
public:
  
  /** Standard constructor
   *  @param Svc pointer to Particle Property Service 
   */ 
  Particle2Definition( LHCb::IParticlePropertySvc* Svc ); 
  
  /// destructor 
  ~Particle2Definition(); 
  
  /** copy constructor
   *  @param right copy 
   */
  Particle2Definition
  ( const Particle2Definition& right );
  
  /** get the Geant4 particle definition from MCParticle object
   *  @param particle pointer to MCParticle
   *  @return pointer to Geant4 particel definition 
   */
  G4ParticleDefinition* operator()
    ( const LHCb::MCParticle* ) const;
  
protected:
  
  /** accessor to particle proeprty service 
   *  @return pointer to particle property service 
   */
  inline LHCb::IParticlePropertySvc* ppSvc() const { return m_ppSvc; }

private:

  LHCb::IParticlePropertySvc* m_ppSvc; ///< particle property service 

};

// ============================================================================
#endif ///< GIGACNV_PARTICLE2DEFINITION_H
// ============================================================================
