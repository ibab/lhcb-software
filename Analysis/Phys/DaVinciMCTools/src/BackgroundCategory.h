// $Id: BackgroundCategory.h,v 1.28 2009-05-10 20:35:53 gligorov Exp $
#ifndef BACKGROUNDCATEGORY_H 
#define BACKGROUNDCATEGORY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Particle.h"
#include "Kernel/IBackgroundCategory.h"          // Interface
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Kernel/IPrintDecay.h"

typedef std::vector<const LHCb::MCParticle*> MCParticleVector;
typedef std::vector<const LHCb::Particle*> ParticleVector;
typedef std::pair<const LHCb::Particle*, const LHCb::MCParticle*> DaughterAndPartnerPair;
typedef std::vector<DaughterAndPartnerPair> DaughterAndPartnerVector;

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
 *				     PID is chosen unless its weight is lower than the
 *                                   cut. The default is 0.5, since the "weight" is the
 *                                   probability of the particle with the "correct" PID to
 *                                   be the "correct" associated MCParticle, given that at least one
 *                                   MCParticle exists which is associated to the ProtoParticle.
 *
 *  InclusiveDecay - is this an inclusive decay? If you want to reconstruct an exclusive
 *			semi-leptonic decay chain, set this to 0 and the SemileptonicDecay property to 1.
 *
 *  SemileptonicDecay - is this a smei-leptonic decay? If so, all neutrinos will be ignored when deciding if
 *			the decay is correctly reconstructed or not. 
 *
 *  NumNeutrinos - The number of neutrinos expected in our decay chain.
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

  IBackgroundCategory::categories category(const LHCb::Particle*);

  const LHCb::MCParticle* origin( const LHCb::Particle* );
  const DaughterAndPartnerVector getDaughtersAndPartners( const LHCb::Particle* ); 

  StatusCode initialize(); 
  StatusCode finalize();

  virtual ~BackgroundCategory( ); ///< Destructor

protected:

private:

  MCParticleVector associate_particles_in_decay(ParticleVector, const LHCb::Particle*);
  MCParticleVector get_mc_mothers(MCParticleVector);
  MCParticleVector create_finalstatedaughterarray_for_mcmother(const LHCb::MCParticle*);
  const LHCb::MCParticle* get_top_mother_of_MCParticle(const LHCb::MCParticle*);
  const LHCb::MCParticle* get_lowest_common_mother(MCParticleVector,ParticleVector);
  const LHCb::MCParticle* get_lowest_common_mother(MCParticleVector);

  int topologycheck(const LHCb::MCParticle*);
  int topologycheck(const LHCb::Particle*);

  bool isStable(int);

  bool hierarchyProblem(MCParticleVector);
  bool foundClones(MCParticleVector);
  bool doAllFinalStateParticlesHaveACommonMother(MCParticleVector, MCParticleVector, ParticleVector);
  bool isTheDecayFullyReconstructed(MCParticleVector);
  bool areAllFinalStateParticlesCorrectlyIdentified(ParticleVector, MCParticleVector);
  bool isTheMotherCorrectlyIdentified(const LHCb::Particle*);
  bool wereAnyResonancesMissed(const LHCb::Particle*);
  bool checkLowMassBackground(const LHCb::Particle*);
  bool areAnyFinalStateParticlesGhosts(MCParticleVector,ParticleVector);
  bool isThisAPileup(MCParticleVector,ParticleVector);
  bool isThisBBarBackground(MCParticleVector);
  bool isThisCCbarBackground(MCParticleVector);
  int areAnyFinalStateParticlesFromAPrimaryVertex(MCParticleVector);

private:
  IParticlePropertySvc* m_ppSvc;
  IParticleDescendants* m_particleDescendants;
  IParticle2MCWeightedAssociator* m_smartAssociator;
  IPrintDecay* m_printDecay ;

  const LHCb::MCParticle* m_commonMother;
  DaughterAndPartnerVector m_daughtersAndPartners;

  
  int m_inclusiveDecay; //are we studying an inclusive decay?
  int m_semileptonicDecay; //are we studying a semileptnoic decay?
  int m_numNeutrinos; //How many neutrinos expected in our decay chain?
  int m_useSoftPhotonCut ; //whether to use the ignore soft photons cut
  double m_softPhotonCut ; //ignore soft photons cut
  double m_lowMassCut ; /// cut applied to low-mass background
  double m_override; //see .cpp file for explanation
  double m_minWeight; //dummy sorting variable
  double m_rescut; //A cut on the minimum lifetime for a mother not to be
			//considered a short-lived resonance

};
#endif // BACKGROUNDCATEGORY_H
