// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorPhotoDetector_H
#define    GIGA_GiGaPhysConstructorPhotoDetector_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorPhotoDetector : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorPhotoDetector>;

public:

  GiGaPhysConstructorPhotoDetector  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorPhotoDetector();


public:
  
  void ConstructParticle ();
  void ConstructProcess  ();
  bool activateRICHPmtPhysProcStatus() 
  {  return m_ActivateRICHPmtPhysProc;}
  
  void setRICHPmtPhysProcActivation(bool bAct) 
  { m_ActivateRICHPmtPhysProc=bAct;  }
  
  ///

private:

  void ConstructPeGenericProcess();
  void ConstructPmtSiEnLoss  ();
  ///

private:
  ///
  GiGaPhysConstructorPhotoDetector           ( const GiGaPhysConstructorPhotoDetector& );
  GiGaPhysConstructorPhotoDetector& operator=( const GiGaPhysConstructorPhotoDetector& );
  ///

  double  m_RichPmtSiDetEfficiency;
  double m_RichPmtPixelChipEfficiency;
  double m_RichPmtPeBackScatterProb;
  bool m_ActivateRICHPmtPhysProc;
  
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorPhotoDetector_H
// ============================================================================











