// $Id: GaussTools_load.cpp,v 1.3 2003-02-14 18:14:48 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/12/17 14:55:45  witoldp
// added GaussInitialisation, use new G4
//
// Revision 1.1  2002/12/12 15:19:30  witoldp
// major repackaging
//
// Revision 1.5  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
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
 *  @author Witold Pokorski Witold.Pokorski@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

void GaussTools_load() 
{ 
  /// Algorithms
  DECLARE_ALGORITHM       (    GetGiGaHitsAlg          );
  DECLARE_ALGORITHM       (    PrintEventAlg           );
  DECLARE_ALGORITHM       (    GiGaFlushAlgorithm      );
  DECLARE_ALGORITHM       (    GaussInitialisation     );

  /// Magnetic Field  
  DECLARE_GiGaFactory     (    GiGaMagFieldGlobal      );
  DECLARE_GiGaFactory     (    GiGaMagFieldUniform     );
  
  /// Streams 
  DECLARE_ALGORITHM       (    GiGaInputStream         );
  DECLARE_ALGORITHM       (    GiGaOutputStream        );

  /// Physics Lists 
  DECLARE_GiGaFactory     (    GiGaPhysListModular     );
  DECLARE_GiGaFactory     (    GiGaPhysListGeantino    );
 
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
  DECLARE_GiGaFactory     (    GaussStepAction         );

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
