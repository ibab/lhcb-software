// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorOp_H
#define    GIGA_GiGaPhysConstructorOp_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorOp : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorOp>;

public:

  GiGaPhysConstructorOp  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorOp();


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
  int MaxPhotonsPerRichCherenkovStepInRich1Agel() 
  {    return m_MaxPhotonsPerRichCherenkovStepInRich1Agel;}
  void setMaxPhotonsPerRichCherenkovStepInRich1Agel(int aAgelMaxPhot) 
  {m_MaxPhotonsPerRichCherenkovStepInRich1Agel=aAgelMaxPhot;}    
  int MaxPhotonsPerRichCherenkovStepInRich1Gas() 
  {    return m_MaxPhotonsPerRichCherenkovStepInRich1Gas;}
  void setMaxPhotonsPerRichCherenkovStepInRich1Gas(int aRich1GasMaxPhot) 
  {m_MaxPhotonsPerRichCherenkovStepInRich1Gas=aRich1GasMaxPhot;}    
  int MaxPhotonsPerRichCherenkovStepInRich2Gas()
  {    return  m_MaxPhotonsPerRichCherenkovStepInRich2Gas;}
  void setMaxPhotonsPerRichCherenkovStepInRich2Gas(int aRich2MaxPhot)
  {m_MaxPhotonsPerRichCherenkovStepInRich2Gas=aRich2MaxPhot;}
  
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
  
    
private:
  ///
  GiGaPhysConstructorOp           ( const GiGaPhysConstructorOp& );
  GiGaPhysConstructorOp& operator=( const GiGaPhysConstructorOp& );
  ///
  bool m_RichActivateVerboseProcessInfoTag;
  int m_MaxPhotonsPerRichCherenkovStep;
  bool m_ApplyMaxPhotCkvLimitPerRadiator;
  int m_MaxPhotonsPerRichCherenkovStepInRich1Agel;
  int m_MaxPhotonsPerRichCherenkovStepInRich1Gas;
  int m_MaxPhotonsPerRichCherenkovStepInRich2Gas;
  std::vector<G4String> m_RichRadiatorMaterialName;
  std::vector<G4int> m_RichRadiatorMaterialIndex;
  
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorOp_H
// ============================================================================











