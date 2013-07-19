// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorOpCkv_H
#define    GIGA_GiGaPhysConstructorOpCkv_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorOpCkv : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorOpCkv>;

public:

  GiGaPhysConstructorOpCkv  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorOpCkv();


public:
  
  void ConstructParticle ();
  void ConstructProcess  ();
  //  void SetCuts ();
  void ConstructPeProcess ();
  // Now adding processes for optical photons SE 30-1-2002
  void ConstructOp       ();
  ///

  bool RichActivateVerboseProcessInfoTag() 
  {    return m_RichActivateVerboseProcessInfoTag;}
  void setRichActivateVerboseProcessInfoTag(bool aVTag)
  {m_RichActivateVerboseProcessInfoTag= aVTag;}
  int MaxPhotonsPerRichCherenkovStep()
  {    return  m_MaxPhotonsPerRichCherenkovStep;}
  void setMaxPhotonsPerRichCherenkovStep(int aMaxValue)
  {m_MaxPhotonsPerRichCherenkovStep=aMaxValue;}
  bool ApplyMaxPhotCkvLimitPerRadiator() 
  {    return  m_ApplyMaxPhotCkvLimitPerRadiator;}
  void setApplyMaxPhotCkvLimitPerRadiator(bool aLimFlag) 
  { m_ApplyMaxPhotCkvLimitPerRadiator=aLimFlag;}    
  // int MaxPhotonsPerRichCherenkovStepInRich1Agel() 
  // {    return m_MaxPhotonsPerRichCherenkovStepInRich1Agel;}
  // void setMaxPhotonsPerRichCherenkovStepInRich1Agel(int aAgelMaxPhot) 
  // {m_MaxPhotonsPerRichCherenkovStepInRich1Agel=aAgelMaxPhot;}    
  int MaxPhotonsPerRichCherenkovStepInRich1Gas() 
  {    return m_MaxPhotonsPerRichCherenkovStepInRich1Gas;}
  void setMaxPhotonsPerRichCherenkovStepInRich1Gas(int aRich1GasMaxPhot) 
  {m_MaxPhotonsPerRichCherenkovStepInRich1Gas=aRich1GasMaxPhot;}    
  int MaxPhotonsPerRichCherenkovStepInRich2Gas()
  {    return  m_MaxPhotonsPerRichCherenkovStepInRich2Gas;}
  void setMaxPhotonsPerRichCherenkovStepInRich2Gas(int aRich2MaxPhot)
  {m_MaxPhotonsPerRichCherenkovStepInRich2Gas=aRich2MaxPhot;}
  int MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator() 
  {    return m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator;}
  void setMaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator(int aQuartzMaxPhot)
  {m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator=aQuartzMaxPhot;}
  
    
  std::vector<G4String> RichRadiatorMaterialName() 
  {    return m_RichRadiatorMaterialName;}
  void setRichRadiatorMaterialName
  (std::vector<G4String> aRichRadiatorMaterialName)
  {m_RichRadiatorMaterialName=aRichRadiatorMaterialName;}
  std::vector<G4int> RichRadiatorMaterialIndex()
  {    return m_RichRadiatorMaterialIndex;}
  void setRichRadiatorMaterialIndex
  (std::vector<G4int> aRichRadiatorMaterialIndex)
  {m_RichRadiatorMaterialIndex=aRichRadiatorMaterialIndex;}    
  std::vector<G4int> FindRichRadiatorMaterialIndices
  ( std::vector<G4String> aRadiatorMaterialNames);
  //  int MaxAllowedPhotStepNumInRayleigh() 
  // {    return m_MaxAllowedPhotStepNumInRayleigh;}
  // void setMaxAllowedPhotStepNumInRayleigh(int aMaxRayleighLimit)
  // { m_MaxAllowedPhotStepNumInRayleigh=aMaxRayleighLimit;}
  //  int MaxNumberRayleighScatAllowed() 
  // {  return  m_MaxNumberRayleighScatAllowed;}
  // void setMaxNumberRayleighScatAllowed(int amrn) 
  // { m_MaxNumberRayleighScatAllowed=amrn;}
  
    
  
  //    bool  UseHpdMagDistortions() 
  // {
  //  return m_UseHpdMagDistortions;
  // }
  // void setUseHpdMagDistortions(bool afla) 
  // { m_UseHpdMagDistortions= afla;
  // }
  // bool IsPSFPreDc06Flag() {return m_IsPSFPreDc06Flag;}
  // void  setIsPreDc06Flag(bool apsfg) {m_IsPSFPreDc06Flag = apsfg;}
  bool PmtQEUseNominalTable(){    return m_PmtQEUseNominalTable;}
  void setPmtQEUseNominalTable(bool aQEOption){m_PmtQEUseNominalTable=aQEOption;}
  
  bool activateRICHOpticalPhysProcStatus()  {  return m_ActivateRICHOpticalPhysProc;}
  void setRICHOpticalPhysProcActivation(bool aAct) 
  {m_ActivateRICHOpticalPhysProc=aAct;} 
 
  bool activateRICHCF4Scintillation() 
  {  return m_activateRICHCF4Scintillation;}
  
  //  bool activateRICHCF4ScintillationHisto() 
  //{return m_activateRICHCF4ScintillationHisto;}

  bool activateTorchTestBeamSimulation() 
  {  return m_activateTorchTestBeamSimulation;}
  void setaActivateTorchTestBeamSimulation ( bool aActivateTorchTB) 
  { m_activateTorchTestBeamSimulation=aActivateTorchTB; }
  

      
  
    
private:
  ///
  GiGaPhysConstructorOpCkv           ( const GiGaPhysConstructorOpCkv& );
  GiGaPhysConstructorOpCkv& operator=( const GiGaPhysConstructorOpCkv& );
  ///
  bool m_RichActivateVerboseProcessInfoTag;
  int m_MaxPhotonsPerRichCherenkovStep;
  bool m_ApplyMaxPhotCkvLimitPerRadiator;
  //  int m_MaxPhotonsPerRichCherenkovStepInRich1Agel;
  int m_MaxPhotonsPerRichCherenkovStepInRich1Gas;
  int m_MaxPhotonsPerRichCherenkovStepInRich2Gas;
  int m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator;
  std::vector<G4String> m_RichRadiatorMaterialName;
  std::vector<G4int> m_RichRadiatorMaterialIndex;
  //  int m_MaxAllowedPhotStepNumInRayleigh;
  //int m_MaxNumberRayleighScatAllowed;
  // bool m_UseHpdMagDistortions;
  // bool m_IsPSFPreDc06Flag;
  bool m_PmtQEUseNominalTable;
  bool m_ActivateRICHOpticalPhysProc;
  bool m_ActivatePmtPhotoElectricPhysProc;
  bool m_activateRICHCF4Scintillation;
  // bool m_activateRICHCF4ScintillationHisto;
  bool m_RichApplyScintillationYieldScaleFactor;
  double m_RichScintillationYieldScaleFactor;
  int m_PmtQESource;
  bool m_activateTorchTestBeamSimulation;
  bool m_activatePmtModuleSupSet3;
  bool m_activatePmtModuleSupSet4;
  bool m_activatePmtModuleSupSet5;
  bool m_activatePmtModuleSupSet6;
  
  
    
  
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorOpCkv_H
// ============================================================================












