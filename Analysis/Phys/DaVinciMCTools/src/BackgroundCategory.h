// $Id: BackgroundCategory.h,v 1.6 2006-01-13 01:29:47 gligorov Exp $
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

private:
  //typedef std::vector<MCParticle*> MCParticleVector;
  IParticlePropertySvc* m_ppSvc;
  IParticleDescendants* m_particleDescendants;
  ProtoParticle2MCAsct::IAsct* m_pCPPAsct;
  Particle2MCWithChi2Asct::IAsct* m_pChi2PPAsct;
  const MCParticle* m_commonmother;

  double m_lowMassCut ; /// cut applied to low-mass background
  double m_override; //see .cpp file for explanation
  double m_maxweight; //dummy sorting variable

};
#endif // BACKGROUNDCATEGORY_H
