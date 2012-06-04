// $Id: $
#ifndef CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPENERGYLOSS_H 
#define CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPENERGYLOSS_H 1

// Include files
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


/** @class TorchTBMcpEnergyLoss TorchTBMcpEnergyLoss.h CherenkovTestBeamPhysProcess/TorchTBMcpEnergyLoss.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-05-25
 */
class TorchTBMcpEnergyLoss :  public G4VEnergyLoss {
public: 
  /// Standard constructor
  TorchTBMcpEnergyLoss( const G4String& processName ,G4ProcessType   aType = fUserDefined ); 

  virtual ~TorchTBMcpEnergyLoss( ); ///< Destructor

  G4bool IsApplicable(const G4ParticleDefinition&);
  // true for all charged particles
  G4double GetContinuousStepLimit(const G4Track& track,
                                  G4double previousStepSize,
                                  G4double currentMinimumStep,
                                  G4double& currentSafety);
  // for all materials other than the PmtSilicon this limit
  // is a large number so that the process is not invoked.
  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack,
                                   const G4Step& aStep) ;

  G4double GetMeanFreePath(const G4Track& track,
                           G4double previousStepSize,
                           G4ForceCondition* condition);
  // the meanfree path
  // is a large number so that the process is not invoked.
  // at all as a post step process.
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step& aStep);
  G4double TorchTBMcpSiEnergyDeposit(G4double ElossInput);
  
  void InitializeMcpProcParam();

  void setMcpAnodeDetEff(double aValueA) {  m_McpAnodeEff= (G4double) aValueA;}
  void setMcpAnodePixelChipEff(double aValueB){ m_McpAnodeChipEff = (G4double) aValueB ;}     




protected:

private:
  // hide assignment and copy operators.
  TorchTBMcpEnergyLoss ( TorchTBMcpEnergyLoss &);
  TorchTBMcpEnergyLoss & operator=(const  TorchTBMcpEnergyLoss &right);

  G4int fMatIndexMcpAnodeEloss;
  G4int fMatIndexMcpEnvelopeKovar;
  G4String m_McpAnodeElossMaterialName;
  G4String m_McpEnvelopeMaterialName;
  G4double m_MinKineticEnergy ;
  G4double m_MipEnergyMcpAnodeEloss;
  G4double m_finalRangeforAnodeStep;
  G4double m_PhElectronMaxEnergy;
  G4double m_AnodeHitDetGlobalEff;

  G4double m_McpAnodeEff;
  G4double m_McpAnodeChipEff;
  
    

  G4double m_AnodeDetXSize;
  G4double m_AnodeDetYSize;
  G4double m_AnodeDetZSize;
  
  




};



#endif // CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPENERGYLOSS_H


