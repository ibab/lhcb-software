// $Id: GiGa_load.cpp,v 1.13 2002-05-07 12:21:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2002/05/04 20:20:12  ibelyaev
//  see $GIGAROOT/doc/release.notes (4 May 2002)
//
// Revision 1.11  2002/04/25 13:02:05  ibelyaev
//  small update
//
// ============================================================================
#define GIGA_GIGA_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"
// ============================================================================
/** @file GiGa_load.cpp
 *  mandatry file for declaration of component library entries 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

void GiGa_load() 
{ 
  /// GiGa Service itself
  DECLARE_SERVICE         (    GiGaSvc                 ); 
  /// Streams 
  DECLARE_ALGORITHM       (    GiGaInputStream         );
  DECLARE_ALGORITHM       (    GiGaOutputStream        );
  DECLARE_ALGORITHM       (    GiGaFlushAlgorithm      );
  /// Run manager  (GiGa)
  DECLARE_GiGaFactory     (    GiGaRunManager          );
  /// Physics List (GiGa)
  DECLARE_GiGaFactory     (    GiGaPhysListGeantino    );
  DECLARE_GiGaFactory     (    GiGaPhysListEm          );
  DECLARE_GiGaFactory     (    GiGaPhysListFull        );
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetPrint        );
  DECLARE_GiGaFactory     (    GiGaSensDetBudget       );
  /// Magnetic Field  
  DECLARE_GiGaFactory     (    GiGaMagFieldGlobal      );
  DECLARE_GiGaFactory     (    GiGaMagFieldUniform     );
  /// stacking action 
  DECLARE_GiGaFactory     (    GiGaStackActionEmpty    );
  /// tracking action 
  DECLARE_GiGaFactory     (    GiGaTrackActionEmpty    );
  DECLARE_GiGaFactory     (    GiGaTrackActionSimple   );
  DECLARE_GiGaFactory     (    GiGaTrackActionSequence );
  /// stepping  action 
  DECLARE_GiGaFactory     (    GiGaStepActionEmpty     );
  DECLARE_GiGaFactory     (    GiGaStepActionDraw      );
  DECLARE_GiGaFactory     (    GiGaStepActionSequence  );
  /// run action
  DECLARE_GiGaFactory     (    GiGaRunActionSequence   );
  DECLARE_GiGaFactory     (    GiGaRunActionCommand    );
  /// event action 
  DECLARE_GiGaFactory     (    GiGaEventActionEmpty    );
  DECLARE_GiGaFactory     (    GiGaEventActionDraw     );
  DECLARE_GiGaFactory     (    GiGaEventActionSequence );
  DECLARE_GiGaFactory     (    GiGaEventActionCommand  );
};

// ============================================================================
// ============================================================================
extern "C" void GiGa_loadRef() { GiGa_load(); } ; 

// ============================================================================
// The End 
// ============================================================================








