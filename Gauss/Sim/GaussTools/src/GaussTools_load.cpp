// $Id: GaussTools_load.cpp,v 1.2 2002-09-26 18:10:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define GAUSSTOOLS_GAUSSTOOLS_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky Witold.Pokorsky@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

void GaussTools_load() 
{ 
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetTracker      );
  DECLARE_GiGaFactory     (    GiGaSensDetVelo         );
  DECLARE_GiGaFactory     (    GiGaSensDetMuon         );

  /// Algorithms
  DECLARE_ALGORITHM       (    GetGiGaHitsAlg           );
  DECLARE_ALGORITHM       (    GiGaFlushAlgorithm      );
  
  /// Magnetic Field  
  DECLARE_GiGaFactory     (    GiGaMagFieldGlobal      );
  DECLARE_GiGaFactory     (    GiGaMagFieldUniform     );
  
  /// Streams 
  DECLARE_ALGORITHM       (    GiGaInputStream         );
  DECLARE_ALGORITHM       (    GiGaOutputStream        );

  /// Physics Lists 
  DECLARE_GiGaFactory     (    GiGaPhysListModular     );
  DECLARE_GiGaFactory     (    GiGaPhysListGeantino    );
  DECLARE_GiGaFactory     (    GiGaPhysListEm          );
  DECLARE_GiGaFactory     (    GiGaPhysListFull        );
  
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetPrint        );
  DECLARE_GiGaFactory     (    GiGaSensDetBudget       );

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
extern "C" void GaussTools_loadRef() { GaussTools_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
