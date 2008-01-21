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
  
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorHpd_H
// ============================================================================











