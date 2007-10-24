// $Id: CopyParticles.h,v 1.5 2007-10-24 15:38:35 jpalac Exp $
#ifndef COPYPARTICLES_H 
#define COPYPARTICLES_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

/** @class CopyParticles CopyParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-16
 */
class CopyParticles : public CopyAndStoreData {
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
  typedef MicroDST::CloneKeyedContainerItem<LHCb::Particle, ParticleItemCloner> ParticleCloner;

  typedef LHCb::Vertex::Container Vertices;
  typedef MicroDST::BasicItemCloner<LHCb::Vertex> VertexItemCloner;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::Vertex, VertexItemCloner> VertexCloner;


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
