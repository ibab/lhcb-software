// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorEM_H
#define    GIGA_GiGaPhysConstructorEM_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorEM : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorEM>;

public:

  GiGaPhysConstructorEM  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorEM();


public:
  
  /// from G4VUserPhysicsList 
  void ConstructParticle ();
  void ConstructProcess  ();
  void SetCuts           ();
  ///

private:
  /// these methods Construct particles 
  void ConstructBosons   ();
  void ConstructLeptons  ();
  void ConstructMesons   ();
  void ConstructBaryons  ();
  ///
  /// these methods Construct physics processes and register them
  void ConstructEM       ();
  ///

private:
  ///
  GiGaPhysConstructorEM           ( const GiGaPhysConstructorEM& );
  GiGaPhysConstructorEM& operator=( const GiGaPhysConstructorEM& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorEM_H
// ============================================================================











