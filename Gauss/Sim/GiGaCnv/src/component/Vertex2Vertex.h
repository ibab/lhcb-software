// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/08/12 17:24:55  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:57  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
// ============================================================================
#ifndef GIGACNV_VERTEX2VERTEX_H 
#define GIGACNV_VERTEX2VERTEX_H 1
// ============================================================================
/// STD & STL 
#include <functional>
// local 
#include "Particle2Particle.h"
/// forward declarations 
namespace LHCb {
  class IParticlePropertySvc;
}
class MCVertex;
class G4PrimaryVertex;
class G4PrimaryParticle;

/** @class Vertex2Vertex Vertex2Vertex.h 
 *   
 *  An auxillary helper class to perform conversion from 
 *  MCVertex objhect to G4PrimaryVertex Object 
 *
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */


class Vertex2Vertex: 
  public std::unary_function<const LHCb::MCVertex*,G4PrimaryVertex*>
{
public:
  
  /** standard constructor
   *  @param Svc pointer to Particle Property Service 
   */
  Vertex2Vertex( LHCb::IParticlePropertySvc* Svc );
  /// destructor 
  ~Vertex2Vertex();
  
  /** perform the conversion of  MCVertex object into 
   *  G4PrimaryVertex object
   *  @param  vertex pointer to MCVertex object 
   *  @return pointer to converted G4PrimaryVertex object
   */
  G4PrimaryVertex* operator() ( const LHCb::MCVertex* vertex ) const;

protected:
  
  /** convert particle object into G4PrimaryParticle object
   *  @param part pointer to MCParticle Object 
   *  @return pointer to converted G4PrimaryParticle object
   */
  inline G4PrimaryParticle* particle( const LHCb::MCParticle* part ) const
  {  return m_p2p( part ); }
  
private:
  
  /** the convertor from MCParticle to G4PrimaryParticle
   */
  Particle2Particle m_p2p;
  
};

// ============================================================================
#endif ///< GIGACNV_VERTEX2VERTEX_H
// ============================================================================
