// ============================================================================
#ifndef    GIGA_GiGaPhysConstructorEm_H
#define    GIGA_GiGaPhysConstructorEm_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysConstructorEm : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysConstructorEm>;

public:

  GiGaPhysConstructorEm  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysConstructorEm();


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
  void ConstructGeneral  ();
  void ConstructEM       ();
  // Now adding processes for optical photons SE 30-1-2002
  void ConstructOp       ();
  ///

private:
  ///
  GiGaPhysConstructorEm           ( const GiGaPhysConstructorEm& );
  GiGaPhysConstructorEm& operator=( const GiGaPhysConstructorEm& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorEm_H
// ============================================================================











