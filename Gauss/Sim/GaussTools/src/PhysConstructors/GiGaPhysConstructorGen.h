// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorGen_H
#define    GIGA_GiGaPhysConstructorGen_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorGen : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorGen>;

public:

  GiGaPhysConstructorGen  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorGen();


public:
  
  void ConstructParticle ();
  void ConstructProcess  ();
  ///

private:

  void ConstructAllShortLiveds();
  void ConstructGeneral       ();
  ///

private:
  ///
  GiGaPhysConstructorGen           ( const GiGaPhysConstructorGen& );
  GiGaPhysConstructorGen& operator=( const GiGaPhysConstructorGen& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorGen_H
// ============================================================================











