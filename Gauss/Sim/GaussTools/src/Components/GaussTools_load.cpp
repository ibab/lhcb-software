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
  /// Streams 
  DECLARE_ALGORITHM       (    GiGaInputStream         ) ;
  DECLARE_ALGORITHM       (    GiGaOutputStream        ) ;

  /// Magnetic Field  
  DECLARE_GiGaFactory     (    GiGaMagFieldGlobal      ) ;
  DECLARE_GiGaFactory     (    GiGaMagFieldUniform     ) ;
  
  /// Physics Lists 
  DECLARE_GiGaFactory     (    GiGaPhysListModular     ) ;
  DECLARE_GiGaFactory     (    GiGaPhysListGeantino    ) ;
  
  /// Sensitive Detector (Material budget evaluation) 
  DECLARE_GiGaFactory     (    GiGaSensDetBudget       ) ;
  
  /// tracking action 
  DECLARE_GiGaFactory     (    CommandTrackAction      );
  DECLARE_GiGaFactory     (    GaussPreTrackAction     );
  DECLARE_GiGaFactory     (    GaussPostTrackAction    );
  DECLARE_GiGaFactory     (    GiGaTrackActionSequence );
  DECLARE_GiGaFactory     (    GaussTrackActionHepMC   );
  //
  DECLARE_GiGaFactory     (    GaussTrackActionStart        ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionGeneral      ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionByEnergy     ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionByType       ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionByEnergyType ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionByProcess    ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionByEnergyProcess ) ;
  DECLARE_GiGaFactory     (    GaussTrackActionFinal        ) ;

  /// stepping  action 
  DECLARE_GiGaFactory     (    GiGaStepActionDraw      ) ;
  DECLARE_GiGaFactory     (    GiGaStepActionSequence  ) ;
  DECLARE_GiGaFactory     (    GaussStepAction         ) ;
  DECLARE_GiGaFactory     (    CutsStepAction          ) ;

  /// run action
  DECLARE_GiGaFactory     (    GiGaRunActionSequence   ) ;
  DECLARE_GiGaFactory     (    GiGaRunActionCommand    ) ;
  DECLARE_GiGaFactory     (    TrCutsRunAction         ) ;

  /// event action 
  DECLARE_GiGaFactory     (    GaussEventActionHepMC   ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionDraw     ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionSequence ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionCommand  ) ;

  /// field manager 
  DECLARE_GiGaFactory     (    GiGaFieldMgr            ) ;
  DECLARE_GiGaFactory     (    ZeroFieldMgr            ) ;
  DECLARE_GiGaFactory     (    GiGaSetSimAttributes    ) ;

  // Regions 
  DECLARE_GiGaFactory     (    GiGaRegionTool          ) ;
  DECLARE_GiGaFactory     (    GiGaRegionsTool         ) ;

};
// ============================================================================


// ============================================================================
extern "C" void GaussTools_loadRef() { GaussTools_load(); } ; 
// ============================================================================

