// $Id: CopyRelatedMCParticles.h,v 1.11 2008-03-20 13:21:58 jpalac Exp $
#ifndef COPYRELATEDMCPARTICLES_H 
#define COPYRELATEDMCPARTICLES_H 1

// Include files
// from MicroDST
#include <MicroDST/MicroDSTAlgorithm.h>
#include <MicroDST/ICloneMCParticle.h>
// from Gaudi
#include "Event/Particle.h"
// from LHCb
#include "Relations/RelationWeighted2D.h"
#include "Kernel/Particle2MCLinker.h"
class MCParticle;
class MCVertex;


/** @class CopyRelatedMCParticles CopyRelatedMCParticles.h
 *  
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
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
  StatusCode associatedMCParticles(const LHCb::Particle* particle,
                                   Particle2MCLinker::ToRange&);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class IT>
  StatusCode loopOnParticles(IT begin, IT end);

private:

  typedef LHCb::MCParticle::Container MCParticles;
  typedef MicroDST::BasicItemCloner<LHCb::MCParticle> MCParticleItemCloner;

  typedef LHCb::MCVertex::Container MCVertices;
  typedef MicroDST::BasicItemCloner<LHCb::MCVertex> MCVertexItemCloner;

  typedef LinkerWithKey<LHCb::MCParticle, LHCb::Particle> MCLinker;

  typedef LHCb::RelationWeighted2D<LHCb::Particle, LHCb::MCParticle, double> MCRelations;

private:

  Particle2MCLinker* m_compositeLinker; ///< Linker for composite particles
 
  Particle2MCLinker* m_linksLinker; ///< Linker for basic particles

  ICloneMCParticle* m_cloner;
  
  MCRelations* m_relations;
  
  
};
#endif // COPYRELATEDMCPARTICLES_H
