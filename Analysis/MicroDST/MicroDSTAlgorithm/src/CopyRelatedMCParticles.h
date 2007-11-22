// $Id: CopyRelatedMCParticles.h,v 1.4 2007-11-22 16:19:48 jpalac Exp $
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
   * Store the MCParticle which is related to a Particle.
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  LHCb::MCParticle* storeMCParticle(const LHCb::Particle* particle);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  const LHCb::MCVertex* storeMCVertex(const LHCb::MCVertex* vertex);

  /**
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  StatusCode associatedMCParticles(const LHCb::Particle* particle,
                                   LHCb::MCParticle::ConstVector&);
  


private:

  Particle2MCLinker* m_compositeLinker; ///< Linker for composite particles
 
  Particle2MCLinker* m_linksLinker; ///< Linker for basic particles
  
};
#endif // COPYRELATEDMCPARTICLES_H
