#ifndef RichHpdSiEnergyLoss_h
#define RichHpdSiEnergyLoss_h 1

#include "globals.hh"
#include <vector>
#include "G4VEnergyLoss.hh"
#include "G4VParticleChange.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"

class RichHpdSiEnergyLoss :  public G4VEnergyLoss {

public:  

  RichHpdSiEnergyLoss(const G4String& processName);

  virtual ~RichHpdSiEnergyLoss();

  G4bool IsApplicable(const G4ParticleDefinition&);
  // true for all charged particles

  G4double GetContinuousStepLimit(const G4Track& track,
                                  G4double previousStepSize,
                                  G4double currentMinimumStep,
                                  G4double& currentSafety);
  // for all materials other than the HpdSilicon this limit
  // is a large number so that the process is not invoked.
  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack,
                                   const G4Step& aStep) ;

  G4double GetMeanFreePath(const G4Track& track,
                           G4double previousStepSize,
                           G4ForceCondition* condition);
  // the meanfree path
  // is a large number so that the process is not invoked.
  // at all as a post step process.
  //
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step& aStep);
  G4double RichHpdSiEnergyDeposit(G4double ElossInput);
  G4double RichHpdSiBackScatter(G4double EnergyInput);

private:

  // hide assignment and copy operators.
  RichHpdSiEnergyLoss ( RichHpdSiEnergyLoss &);
  RichHpdSiEnergyLoss & operator=(const  RichHpdSiEnergyLoss &right);

  //now the data members.
  G4int fMatIndexHpdSiEloss;
  G4int fMatIndexHpdEnvelopeKovar;
  G4String HpdSiElossMaterialName;
  G4String HpdEnvelopeMaterialName;
  G4double MinKineticEnergy ;
  G4double MipEnergyHpdSiEloss;
  G4double finalRangeforSiDetStep;
  G4double PhElectronMaxEnergy;
  G4double SiHitDetGlobalEff;
  G4double PeBackScaProb;

};

inline G4double RichHpdSiEnergyLoss::RichHpdSiBackScatter (G4double EnergyInput)
{
  return EnergyInput;
}

#endif
