// ============================================================================
#ifndef    GIGA_GeneralPhysics_H
#define    GIGA_GeneralPhysics_H 1 
// ============================================================================
// include files 

//G4
#include "globals.hh"
#include "G4ios.hh"
#include "G4Decay.hh"
// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

// forward declarations 
template <class TYPE> class GiGaFactory;

class GeneralPhysics : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GeneralPhysics>;

public:

  GeneralPhysics  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~GeneralPhysics();

public:

  // This method will be invoked in the Construct() method. 
  // each particle type will be instantiated
  virtual void ConstructParticle();
  
  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type 
  virtual void ConstructProcess();

protected:
  G4bool wasActivated;
private:
  G4Decay fDecayProcess;
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GeneralPhysics_H
// ============================================================================











