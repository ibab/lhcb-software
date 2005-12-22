// ============================================================================
#define GAUSSRICH_GAUSSRICH_LOAD_CPP 1 
// ============================================================================

// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file GaussRICH_load.cpp
 *  The mandatory file for declaration of component library entries 
 *  @author Sajan Easo Sajan.Easo@cern.ch
 *  @date 2002-09-26
 */

void GaussRICH_load() 
{ 

  /// Sensitive Detector 
  DECLARE_GiGaFactory     (  RichSensDet             );

  /// run action
  DECLARE_GiGaFactory     (  RichG4RunAction         );

  /// event action 
  DECLARE_GiGaFactory     (  RichG4EventAction       );
  
  /// physics constructors
  DECLARE_GiGaFactory     (  GiGaPhysConstructorOp   );

  /// MCRichHit algorithm
  DECLARE_ALGORITHM       (  GetMCRichHitsAlg           );

  /// MCRichOpticalPhoton algorithm
  DECLARE_ALGORITHM       (  GetMCRichOpticalPhotonsAlg );

  /// MCRichSegment alorithm
  DECLARE_ALGORITHM       (  GetMCRichSegmentsAlg       );

 /// MCRichTrack alorithm
  DECLARE_ALGORITHM       (  GetMCRichTracksAlg         );

};
// ============================================================================


// ============================================================================
extern "C" void GaussRICH_loadRef() { GaussRICH_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
