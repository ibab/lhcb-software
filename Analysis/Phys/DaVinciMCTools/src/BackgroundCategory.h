// $Id: BackgroundCategory.h,v 1.10 2006-02-01 16:44:41 gligorov Exp $
#ifndef BACKGROUNDCATEGORY_H 
#define BACKGROUNDCATEGORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Particle.h"
#include "Event/Collision.h"
#include "DaVinciMCTools/IBackgroundCategory.h"          // Interface
#include "Kernel/IParticleDescendants.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

typedef std::vector<const MCParticle*> MCParticleVector;

/** @class BackgroundCategory BackgroundCategory.h
 * 
 *  For further documentation, please see the file IBackgroundCategory.h
 *  Available properties: 
 *
 *  UseSoftPhotonCut - whether to ignore "soft" photons 
 *  
 *  SoftPhotonCut - the maximum energy of a "soft" photon, if using the cut. (default to 300MeV or less)
 *
 *  LowMassBackgroundCut - the mass cut used to classify background as Low Mass. For
 *			   an explanation of usage, see IBackgroundCategory.h. It 
 *			   defaults to 100MeV.
 *
 *  MCmatchQualityPIDoverrideLevel - At present the tool will occasionally find that
 *				     one ProtoParticle has more than one MCParticle
 *				     associated to it. The MCParticle with the "correct"
 *				     PID is chosen unless the ratio of matching weights 
 *				     (correctpid_match_weight/best_match_weight) is less
 *				     than the cut. The default is 10.
 *  
 *  ResonanceCut - The maximum lifetime at which a particle is considered a short lived resonance.
 *  		   Defaults to 10^-6 nanoseconds.
 *
 *  @author Vladimir Gligorov
 *  @date   2005-11-23
 */
class BackgroundCategory : public GaudiTool, virtual public IBackgroundCategory {
public: 
  /// Standard constructor
  BackgroundCategory( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  IBackgroundCategory::categories category(const Particle*);

  const MCParticle* origin( const Particle* ); 

  StatusCode initialize(); 
  StatusCode finalize();

  virtual ~BackgroundCategory( ); ///< Destructor

protected:

private:

  MCParticleVector associate_particles_in_decay(ParticleVector);
  MCParticleVector get_mc_mothers(MCParticleVector, const Particle*);
  MCParticleVector create_finalstatedaughterarray_for_mcmother(const MCParticle*);

  bool isStable(int);

  bool condition_A(MCParticleVector, ParticleVector);
  bool condition_B(MCParticleVector);
  bool condition_C(ParticleVector, MCParticleVector);
  //  bool condition_D(MCParticleVector, const Particle*);
  bool condition_D(const Particle*);
  bool condition_E();
  bool condition_F(const Particle*);
  bool condition_G(MCParticleVector,ParticleVector);
  bool condition_H(MCParticleVector,ParticleVector);
  bool condition_I(MCParticleVector);
  bool condition_J(MCParticleVector);
  int condition_PV(MCParticleVector,MCParticleVector);

private:
  //typedef std::vector<MCParticle*> MCParticleVector;
  IParticlePropertySvc* m_ppSvc;
  IParticleDescendants* m_particleDescendants;
  ProtoParticle2MCAsct::IAsct* m_pCPPAsct;
  Particle2MCWithChi2Asct::IAsct* m_pChi2PPAsct;
  const MCParticle* m_commonMother;

  int m_useSoftPhotonCut ; //whether to use the ignore soft photons cut
  double m_softPhotonCut ; //ignore soft photons cut
  double m_lowMassCut ; /// cut applied to low-mass background
  double m_override; //see .cpp file for explanation
  double m_maxWeight; //dummy sorting variable
  double m_rescut; //A cut on the minimum lifetime for a mother not to be
			//considered a short-lived resonance

};
#endif // BACKGROUNDCATEGORY_H
