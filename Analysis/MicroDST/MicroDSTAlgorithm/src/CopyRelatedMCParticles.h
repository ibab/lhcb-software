// $Id: CopyRelatedMCParticles.h,v 1.14 2008-03-26 13:57:46 jpalac Exp $
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
 * MicroDSTAlgorithm to clone the LHCb::MCParticles that are related to a
 * set of LHCb::Particle from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::Particles are taken from the TES location 
 * defined by InputLocation, and their associated LHCb::MCParticles are
 * searched using a Particle2MCLinker. The associated LHCb::MCParticles are
 * then cloned and placed in a TES location derived from the original one 
 * (typically "/Event/MC/Particles") with the value of OutputPrefix added
 * after the leading "/Event".
 *
 * The actual cloning of individual LHCb::MCParticles is performed by the 
 * ICloneMCParticle, the implementation of which is set by the property 
 * IMCCloneMCParticle (default MCParticleCloner). The depth and content of the 
 * cloning action depends on the implementation of the ICloneMCParticle.
 * The algorithm also stores the associations between the input set of 
 * LHCb::Particle and the cloned LHCb::MCParticles. This is stored in the form 
 * of an LHCb::RelationWeighted2D<LHCb::Particle, LHCb::MCParticle, double> in
 * a TES location defined by InputLocation + "/RelatedMCParticles".
 * @see ICloneMCParticle
 * @see MCParticleCloner
 *
 * <b>Example</b>: Clone LHCb::MCParticles associated to LHCb::Particles in 
 * "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"  using an MCParticleCloner. 
 * Copy associations to "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles/RelatedMCParticles"
 *
 *  @code
 *
 *  // Add a CopyRelatedMCParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyRelatedMCParticles/CopyMC"};
 *  CopyMC.OutputPrefix = "MyLocation";
 *  CopyMC.InputLocation = "MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles";
 *  CopyMC.ICloneMCParticle = "MCParticleCloner" // this is the default anyway.
 *  @endcode
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

  std::string m_mcParticleClonerName;
  
};
#endif // COPYRELATEDMCPARTICLES_H
