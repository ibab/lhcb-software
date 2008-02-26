// $Id: CopyRelatedMCParticles.h,v 1.9 2008-02-26 15:41:01 jpalac Exp $
#ifndef COPYRELATEDMCPARTICLES_H 
#define COPYRELATEDMCPARTICLES_H 1

// Include files
// from MicroDST
#include <MicroDST/MicroDSTAlgorithm.h>
#include <MicroDST/ICloneMCParticle.h>
// from Gaudi
#include "Event/Particle.h"
// from LHCb
#include "Relations/Relation2D.h"
#include "Kernel/Particle2MCLinker.h"
class MCParticle;
class MCVertex;


/** @class CopyRelatedMCParticles CopyRelatedMCParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-02
 */
class CopyRelatedMCParticles : public MicroDSTAlgorithm {
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
                                   Particle2MCLinker::ToRange&);

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

private:

  typedef LHCb::MCParticle::Container MCParticles;
  typedef MicroDST::BasicItemCloner<LHCb::MCParticle> MCParticleItemCloner;

  typedef LHCb::MCVertex::Container MCVertices;
  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> MCVertexItemCloner;

  typedef LinkerWithKey<LHCb::MCParticle, LHCb::Particle> MCLinker;

  typedef LHCb::Relation2D<LHCb::Particle, LHCb::MCParticle> MCRelations;

private:

  Particle2MCLinker* m_compositeLinker; ///< Linker for composite particles
 
  Particle2MCLinker* m_linksLinker; ///< Linker for basic particles

  ICloneMCParticle* m_cloner;
  
  MCRelations* m_relations;
  
  
};
#endif // COPYRELATEDMCPARTICLES_H
