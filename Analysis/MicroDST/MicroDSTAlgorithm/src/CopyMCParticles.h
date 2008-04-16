// $Id: CopyMCParticles.h,v 1.2 2008-04-16 10:28:04 jpalac Exp $
#ifndef COPYMCPARTICLES_H 
#define COPYMCPARTICLES_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTAlgorithm.h>

class ICloneMCParticle;

/** @class CopyMCParticles CopyMCParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::MCParticles and related objects 
 * from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::MCParticles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::MCParticles is performed by the 
 * ICloneMCParticle, the implementation of which is set by the property 
 * ICloneMCParticle (default MCParticleCloner)
 * @see ICloneMCParticle
 * @see MCParticleCloner
 *
 * <b>Example</b>: Clone MCParticles from "/Event/MC/Particles" to 
 * "/Event/MyLocation/MC/Particles" using an MCParticleCloner
 *  @code
 *
 *  // Add a CopyMCParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyMCParticles"};
 *  CopyMCParticles.OutputPrefix = "MyLocation";
 *  CopyMCParticles.InputLocation = "MC/Particles";
 *  CopyMCParticles.ICloneMCParticle = "MCParticleCloner"
 *  @endcode
 * 
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-08
 */
class CopyMCParticles : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyMCParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyMCParticles( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  ICloneMCParticle* m_particleCloner;
  std::string m_particleClonerName;

};
#endif // COPYMCPARTICLES_H
