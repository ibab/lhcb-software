// $Id: CopyParticles.h,v 1.3 2007-10-22 13:34:40 jpalac Exp $
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

  /**
   * Functor to custom-clone an LHCb::Vertex object.
   * Depending on the state of storeTracks(), 
   * either performs standard clone or does something
   * special with the SmartRefVector<LHCb::Track>
   *
   * @author Juan Palacios juanch@nikhef.nl
   * @date 16-10-2007
   */
  struct VertexItemCloner 
  {
  public:

    static LHCb::Vertex* clone(const LHCb::Vertex* vtx)
    {
      LHCb::Vertex* item = vtx->clone();
      return item;
    }
  };

private:

  typedef LHCb::Vertex::Container Vertices;
  typedef LHCb::Particle::Container Particles;
  typedef CopyAndStoreData::BasicItemCloner<LHCb::Particle> ParticleCloneFunctor;
  typedef CopyAndStoreData::CloneKeyedContainerItem<LHCb::Particle, ParticleCloneFunctor> ParticleCloner;
  typedef CopyAndStoreData::CloneKeyedContainerItem<LHCb::Vertex, VertexItemCloner> VertexCloner;


private:

  /**
   * This method clones and stores the LHCb::Particles in
   * inputTESLocation() and puts them in fullOutputTESLocation().
   * It is called for the top level LHCb::Particles, as these all 
   * have a unique TES location for input and output.
   *
   * @author Juan Palacios juancho@nikhef.nl
   */  
  StatusCode storeMothers();

  /**
   * 
   * Starting from an LHCb::Particle that has been cloned and stored on
   * the TES, trigger the storing of the full decay tree by calling
   * the storeVertex(LHCb::Particle*, LHCb::Vertex) method.
   *
   * 
   * @author Juan Palacios juancho@nikhef.nl
  */
  StatusCode storeDecay(LHCb::Particle* clonedParticle);

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
