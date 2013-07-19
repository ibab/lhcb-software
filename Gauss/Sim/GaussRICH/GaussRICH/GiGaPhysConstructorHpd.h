// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorSi_H
#define    GIGA_GiGaPhysConstructorSi_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorHpd : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorHpd>;

public:

  GiGaPhysConstructorHpd  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorHpd();


public:
  
  void ConstructParticle ();
  void ConstructProcess  ();
  bool activateRICHHPDPhysProcStatus() 
  {  return m_ActivateRICHHPDPhysProc;}
  
  void setRICHHPDPhysProcActivation(bool bAct) 
  { m_ActivateRICHHPDPhysProc=bAct;  }
  
    
  ///

private:

  void ConstructPeGenericProcess();
  void ConstructHpdSiEnLoss  ();
  ///

private:
  ///
  GiGaPhysConstructorHpd           ( const GiGaPhysConstructorHpd& );
  GiGaPhysConstructorHpd& operator=( const GiGaPhysConstructorHpd& );
  ///

  double  m_RichHpdSiDetEfficiency;
  double m_RichHpdPixelChipEfficiency;
  double m_RichHpdPeBackScatterProb;
  bool m_ActivateRICHHPDPhysProc;
  double m_RichHpdReadoutEffWithAerogel;   //to account for efficiency loss seen in real data
  double m_RichHpdReadoutEffWithRich1Gas; // to account for efficiency loss seen in real data
  double m_RichHpdReadoutEffWithRich2Gas; // to account for efficiency loss seen in real data
  
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorHpd_H
// ============================================================================











