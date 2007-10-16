// $Id: CopyParticles.h,v 1.1 2007-10-16 14:41:45 jpalac Exp $
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
  struct VertexCloner 
  {
  public:

    LHCb::Vertex* clone(const LHCb::Vertex* vtx) const
    {
      LHCb::Vertex* item = vtx->clone();
      return item;
    }
  private:
    
  };

private:

  typedef LHCb::Vertex::Container Vertices;
  typedef LHCb::Particle::Container Particles;
  typedef CopyAndStoreData::BasicItemCloner<LHCb::Particle> ParticleCloner;
  typedef CopyAndStoreData::ContainerCloner<Vertices, VertexCloner > VContainerCloner;
  typedef CopyAndStoreData::ContainerCloner<Particles, ParticleCloner > ParticleContainerCloner;

//   inline const VContainerCloner& containerCloner() const 
//   {
//     return m_vtxCloner;
//   }

//   inline VContainerCloner& containerCloner()
//   {
//     return m_vtxCloner;
//   }

private:

  //  VContainerCloner m_vtxCloner;
  ParticleContainerCloner m_partCloner;
  
};
#endif // COPYPRIMARYVERTICES_H
