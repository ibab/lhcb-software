// $Id: CopyParticles.h,v 1.11 2008-03-20 13:39:02 jpalac Exp $
#ifndef COPYPARTICLES_H 
#define COPYPARTICLES_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTAlgorithm.h>
#include <MicroDST/ICloneParticle.h>

/** @class CopyParticles CopyParticles.h
 *  
 * MicroDSTAlgorithm to clone LHCb::Particles and related objects 
 * from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::Particles are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 * a leading "/Event" it is removed.
 * The actual cloning of individual LHCb::Particles is performed by the 
 * IParticleCloner, the implementation of which is set by the property 
 * ICloneParticle (default ParticleCloner)
 * @see ICloneParticle
 * @see ParticleCloner
 *
 * <b>Example</b>: Clone particles from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" to 
 * "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" using a ParticleCloner
 *  @code
 *
 *  // Add a CopyParticles instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyParticles"};
 *  CopyParticles.OutputPrefix = "MyLocation";
 *  CopyParticles.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles";
 *  CopyParticles.ICloneParticle = "ParticleCloner"
 *  @endcode
 * 
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
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

private:

  ICloneParticle* m_particleCloner;

  std::string m_particleClonerName;

};
#endif // COPYPRIMARYVERTICES_H
