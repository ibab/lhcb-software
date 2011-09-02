#ifndef RichHpdSiEnergyLoss_h
#define RichHpdSiEnergyLoss_h 1

#include "globals.hh"
#include <vector>
#include "G4VEnergyLoss.hh"
#include "Randomize.hh"
#include "G4VParticleChange.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ios.hh"
#include "GiGa/GiGaBase.h"

class RichHpdSiEnergyLoss :  public G4VEnergyLoss {

public:  

  RichHpdSiEnergyLoss(const G4String& processName ,G4ProcessType   aType = fUserDefined  );

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
  G4double RichHpdSiEnergyDeposit(G4double ElossInput, G4double aReadOutEff);
  G4double RichHpdSiBackScatter(G4double EnergyInput);
  void InitializeHpdProcParam();

  void setHpdSiDetEff(double aValueA) {  m_HpdSiDetEff= (G4double) aValueA;}
  void setHpdSiPixelChipEff(double aValueB){ m_SiPixelChipEff = (G4double) aValueB ;}     

  void setHpdPeBackScaProb(double aValueC) {PeBackScaProb=(G4double) aValueC ;}
 
  void setHpdReadOutEffAerogel(double aValAgelEff) 
  {m_HpdReadOutEffAerogel=(G4double) aValAgelEff;}
  void setHpdReadOutEffRich1Gas(double aValR1Eff) 
  {m_HpdReadOutEffRich1Gas=(G4double) aValR1Eff;}
  void setHpdReadOutEffRich2Gas(double aValR2Eff) 
  {m_HpdReadOutEffRich2Gas=(G4double)aValR2Eff;}
  G4double  getPhotonOriginRadDependentReadoutEfficiency(  const G4Track& aTrack );
  
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

  G4double m_HpdSiDetEff;  
  G4double m_SiPixelChipEff;
  
    
  G4double PeBackScaProb;
  G4double PeBackScaProbCorrected;

  G4double m_siliconDetXSize;
  G4double m_siliconDetYSize;
  G4double m_siliconDetZSize;
  
  G4double m_HpdReadOutEffAerogel;
  G4double m_HpdReadOutEffRich1Gas;
  G4double m_HpdReadOutEffRich2Gas;
  
  G4double m_HpdCurrentReadOutEff;
  
};

inline G4double RichHpdSiEnergyLoss::RichHpdSiBackScatter (G4double EnergyInput)
{
  return EnergyInput;
}

#endif
