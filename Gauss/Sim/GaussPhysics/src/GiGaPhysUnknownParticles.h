// ============================================================================
#ifndef    GAUSSPHYS_GIGAPHYSUNKNOWNPARTICLES_H
#define    GAUSSPHYS_GIGAPHYSUNKNOWNPARTICLES_H 1 
// ============================================================================
// include files 
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaPhysConstructorBase.h"
#include "G4Decay.hh"
#include "G4UnknownDecay.hh"
// ============================================================================
// forward declarations 
template <class TYPE> class GiGaFactory;

class GiGaPhysUnknownParticles : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysUnknownParticles>;

public:

  GiGaPhysUnknownParticles  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GiGaPhysUnknownParticles();


public:
  void ConstructParticle ();
  void ConstructProcess  ();
  
private:
  
  G4Decay m_decayProcess ;
  G4UnknownDecay m_unknownDecay ;
  
};
// ============================================================================


// ============================================================================
#endif   ///< GAUSSPHYS_GIGAPHYSUNKNOWNPARTICLES_H
// ============================================================================











