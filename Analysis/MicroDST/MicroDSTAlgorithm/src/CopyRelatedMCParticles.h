// $Id: CopyRelatedMCParticles.h,v 1.5 2007-11-23 17:59:19 jpalac Exp $
#ifndef COPYRELATEDMCPARTICLES_H 
#define COPYRELATEDMCPARTICLES_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"
#include "Event/Particle.h"
// from LHCb
#include "Kernel/Particle2MCLinker.h"
class MCParticle;
class MCVertex;


/** @class CopyRelatedMCParticles CopyRelatedMCParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-02
 */
class CopyRelatedMCParticles : public CopyAndStoreData {
public: 
  /// Standard constructor
  CopyRelatedMCParticles( const std::string& name, 
                          ISvcLocator* pSvcLocator );

  virtual ~CopyRelatedMCParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /**
   * Store the MCParticles which are related to a Particle.
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  StatusCode storeAssociatedMCParticles(const LHCb::Particle* particle);



  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  LHCb::MCVertex* storeMCVertex(const LHCb::MCVertex* vertex);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  StatusCode associatedMCParticles(const LHCb::Particle* particle,
                                   LHCb::MCParticle::ConstVector&);
  

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class IT>
  StatusCode loopOnParticles(IT begin, IT end);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  LHCb::MCParticle* storeMCParticle(const LHCb::MCParticle* mcp,
                                    bool storeOriginVertex = false);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  inline bool deepCloneMCVertex() { return m_deepCloneMCVertex; }  

private:

  typedef LHCb::MCParticle::Container MCParticles;
  typedef MicroDST::BasicItemCloner<LHCb::MCParticle> MCParticleItemCloner;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::MCParticle, MCParticleItemCloner> MCParticleCloner;

  typedef LHCb::MCVertex::Container MCVertices;
  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> MCVertexItemCloner;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::MCVertex, MCVertexItemCloner> MCVertexCloner;

private:

  Particle2MCLinker* m_compositeLinker; ///< Linker for composite particles
 
  Particle2MCLinker* m_linksLinker; ///< Linker for basic particles

  bool m_deepCloneMCVertex; ///< control whether MCVertex's products should also be cloned
  
};
#endif // COPYRELATEDMCPARTICLES_H
