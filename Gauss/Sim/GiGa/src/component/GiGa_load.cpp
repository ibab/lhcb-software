// $Id: GiGa_load.cpp,v 1.11 2002-04-25 13:02:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
  /// Physics List (GiGa)
  DECLARE_GiGaPhysList    (    GiGaPhysListGeantino    );
  DECLARE_GiGaPhysList    (    GiGaPhysListEm          );
  DECLARE_GiGaPhysList    (    GiGaPhysListFull        );
  /// Sensitive Detector 
  DECLARE_GiGaSensDet     (    GiGaSensDetPrint        );
  /// Magnetic Field  
  DECLARE_GiGaMagField    (    GiGaMagFieldGlobal      );
  DECLARE_GiGaMagField    (    GiGaMagFieldUniform     );
  /// stacking action 
  DECLARE_GiGaStackAction (    GiGaStackActionEmpty    );
  /// tracking action 
  DECLARE_GiGaTrackAction (    GiGaTrackActionEmpty    );
  DECLARE_GiGaTrackAction (    GiGaTrackActionSimple   );
  DECLARE_GiGaTrackAction (    GiGaTrackActionSequence );
  /// stepping  action 
  DECLARE_GiGaStepAction  (    GiGaStepActionEmpty     );
  DECLARE_GiGaStepAction  (    GiGaStepActionDraw      );
  DECLARE_GiGaStepAction  (    GiGaStepActionSequence  );
  /// run action
  DECLARE_GiGaRunAction   (    GiGaRunActionSequence   );
  DECLARE_GiGaRunAction   (    GiGaRunActionCommand    );
  /// event action 
  DECLARE_GiGaEventAction (    GiGaEventActionEmpty    );
  DECLARE_GiGaEventAction (    GiGaEventActionDraw     );
  DECLARE_GiGaEventAction (    GiGaEventActionSequence );
  DECLARE_GiGaEventAction (    GiGaEventActionCommand  );
};

// ============================================================================
// ============================================================================
extern "C" void GiGa_loadRef() { GiGa_load(); } ; 

// ============================================================================
// The End 
// ============================================================================








