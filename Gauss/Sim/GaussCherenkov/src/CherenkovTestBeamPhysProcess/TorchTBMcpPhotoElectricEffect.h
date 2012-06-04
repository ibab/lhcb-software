// $Id: $
#ifndef CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPPHOTOELECTRICEFFECT_H 
#define CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPPHOTOELECTRICEFFECT_H 1

// Include files
#include <vector>
#include "globals.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4VDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Step.hh"

#include "GiGa/GiGaBase.h"

#include "TorchTBMcpProperties.h"

/** @class TorchTBMcpPhotoElectricEffect TorchTBMcpPhotoElectricEffect.h CherenkovTestBeamPhysProcess/TorchTBMcpPhotoElectricEffect.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-05-18
 */
class TorchTBMcpPhotoElectricEffect : public G4VDiscreteProcess {
public: 
  /// Standard constructor
  TorchTBMcpPhotoElectricEffect(const GiGaBase*, const G4String&, 
                          G4ProcessType   aType = fOptical); 

  virtual ~TorchTBMcpPhotoElectricEffect( ); ///< Destructor

  G4bool IsApplicable(const G4ParticleDefinition&  );
  // is applicable for optical photon only

  G4double GetMeanFreePath(const G4Track& ,
                           G4double ,
                           G4ForceCondition* condition);
   
  // returns infinity (max integer possible) . This means the process does
  // not limit the step, but sets the Forced condition for the DoIt to be
  // invoked at every step. But only at the boundary between Mcp quartz
  // window and the Mcp photocathode any action be taken.
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step&  aStep);
  //The method for implementing the McpPhotoelectric effect.

  double getCurrentMcpQE(int imcpnum,double photonenergy);
  // To interpolate the QE from the QE data for a specific pmt.

  G4double McpPhElectronKE()   {return m_McpPhElectronKE; }
  //To get the PhotoElectron energy at emission, which is from the
  // HV applied to the MCP tube.
  
  void setMcpPhElectronKE(double McpPhElecKE );

  //  double PhCathodeToAnodeDetMaxDist() {return m_PhCathodeToAnodeDetMaxDist; }
  //  void setPhCathodeToAnodeDetMaxDist(double PhCathodeToSilZ );


  G4String PrePhotoElectricLogVolName() const {return m_PrePhotoElectricLogVolName; }
  void setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName)
                   {m_PrePhotoElectricLogVolName= PrePhotoElecLogVolName;  }
  G4String PostPhotoElectricLogVolName() const {return m_PostPhotoElectricLogVolName; }
  void setPostPhotoElectricLogVolName( const G4String PostPhotoElecLogVolName)
  {m_PostPhotoElectricLogVolName=PostPhotoElecLogVolName;  }
  


  TorchTBMcpProperties* McpProperty() 
    {return  TorchTBMcpProperties::getTorchTBMcpPropertiesInstance(); }



  G4String TorchTBPhysVolNameA() const {return  m_TorchTBPhysVolNameA; }
  G4String TorchTBPhysVolNameB() const {return  m_TorchTBPhysVolNameB; }

  double MaxAnyMcpQEff() const
  { return m_MaxAnyMcpQEff;}

  int numTotMcp() 
  {  return m_numTotMcp ;}
  void setNumTotMcp (int aNumM);
  
  void   setMcpPhElecParam();
  
    
protected:

private:

  G4ThreeVector getCathodeToAnodeDirection(int imcpnum , int torchdetnum , 
                                           G4ThreeVector aLocalElnOrigin , 
                                          std::vector<double>apsfVectInXY);
 
  G4String m_PrePhotoElectricLogVolName;
  G4String m_PostPhotoElectricLogVolName;

  G4String m_TorchTBPhysVolNameA;
  G4String m_TorchTBPhysVolNameB;
  double m_MaxAnyMcpQEff;
  double m_McpPhElectronKE;
  //  double m_PhCathodeToAnodeDetMaxDist;
  int m_numTotMcp;
  G4double m_elecOrigTolerence;
  

};
inline G4bool TorchTBMcpPhotoElectricEffect::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

inline G4double TorchTBMcpPhotoElectricEffect::GetMeanFreePath(const G4Track& ,
                                            G4double ,
                                            G4ForceCondition* condition)
{
  *condition = Forced;
  return DBL_MAX;
}
inline void TorchTBMcpPhotoElectricEffect::setMcpPhElectronKE(double PmtPhElecKE) 
{  m_McpPhElectronKE=PmtPhElecKE;}

//inline void  TorchTBMcpPhotoElectricEffect::setPhCathodeToAnodeDetMaxDist
// (double PhCathodeToSilZ){  m_PhCathodeToAnodeDetMaxDist= PhCathodeToSilZ;}

inline  void TorchTBMcpPhotoElectricEffect::setNumTotMcp (int aNumM)
  {m_numTotMcp=aNumM;}



#endif // CHERENKOVTESTBEAMPHYSPROCESS_TORCHTBMCPPHOTOELECTRICEFFECT_H
