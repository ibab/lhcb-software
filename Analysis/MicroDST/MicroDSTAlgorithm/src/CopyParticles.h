// $Id: CopyParticles.h,v 1.6 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYPARTICLES_H 
#define COPYPARTICLES_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTAlgorithm.h>
#include <MicroDST/ICloneParticle.h>
#include <Event/Particle.h>
#include <Event/Vertex.h>

/** @class CopyParticles CopyParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-16
 */
class CopyParticles : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:


  typedef LHCb::Particle::Container Particles;
  typedef MicroDST::BasicItemCloner<LHCb::Particle> ParticleItemCloner;

  typedef LHCb::Vertex::Container Vertices;
  typedef MicroDST::BasicItemCloner<LHCb::Vertex> VertexItemCloner;

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

private:

  /**
   *
   * Clone and store the decay vertex of a particle.
   * Iteratively clone and store its decay particles
   * and their end vertices and their decay particles
   * and ...
   * @param mother 
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  const LHCb::Vertex* storeVertex(const LHCb::Vertex* vertex);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  LHCb::Particle* storeParticle(const LHCb::Particle* particle);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  void storeDaughters(LHCb::Particle* particleClone,
                      const SmartRefVector<LHCb::Particle>& daughters);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  void storeOutgoingParticles(LHCb::Vertex* vertexClone,
                              const SmartRefVector<LHCb::Particle>& outParticles);

  
private:

};
#endif // COPYPRIMARYVERTICES_H
