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

private:
  ///
  GiGaPhysConstructorOp           ( const GiGaPhysConstructorOp& );
  GiGaPhysConstructorOp& operator=( const GiGaPhysConstructorOp& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysConstructorOp_H
// ============================================================================











