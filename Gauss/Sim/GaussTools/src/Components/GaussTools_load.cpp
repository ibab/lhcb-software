// ============================================================================
#define GAUSSTOOLS_GAUSSTOOLS_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
//#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorski Witold.Pokorski@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

DECLARE_FACTORY_ENTRIES(Test) {

  /// Streams
  DECLARE_ALGORITHM( GiGaInputStream );
  DECLARE_ALGORITHM( GiGaOutputStream );

  /// Magnetic Field  
  DECLARE_TOOL( GiGaMagFieldGlobal );
  DECLARE_TOOL( GiGaMagFieldUniform );
  
  /// Physics Lists 
  DECLARE_TOOL( GiGaPhysListModular );
  DECLARE_TOOL( GiGaPhysListGeantino );
  
  /// Sensitive Detector (Material budget evaluation) 
  DECLARE_TOOL( GiGaSensDetBudget );
  
  /// tracking action 
  DECLARE_TOOL( CommandTrackAction );
  DECLARE_TOOL( GaussPreTrackAction );
  DECLARE_TOOL( GaussPostTrackAction );
  DECLARE_TOOL( GiGaTrackActionSequence );
  DECLARE_TOOL( GaussTrackActionHepMC );
  //
  DECLARE_TOOL( GaussTrackActionStart );
  DECLARE_TOOL( GaussTrackActionGeneral );
  DECLARE_TOOL( GaussTrackActionByEnergy );
  DECLARE_TOOL( GaussTrackActionByType );
  DECLARE_TOOL( GaussTrackActionByEnergyType );
  DECLARE_TOOL( GaussTrackActionByProcess );
  DECLARE_TOOL( GaussTrackActionByEnergyProcess );
  DECLARE_TOOL( GaussTrackActionFinal );

  /// stepping  action 
  DECLARE_TOOL( GiGaStepActionDraw );
  DECLARE_TOOL( GiGaStepActionSequence );
  DECLARE_TOOL( GaussStepAction );
  DECLARE_TOOL( CutsStepAction );

  /// run action
  DECLARE_TOOL( GiGaRunActionSequence );
  DECLARE_TOOL( GiGaRunActionCommand );
  DECLARE_TOOL(TrCutsRunAction );

  /// event action 
  DECLARE_TOOL( GaussEventActionHepMC );
  DECLARE_TOOL( GiGaEventActionDraw );
  DECLARE_TOOL( GiGaEventActionSequence );
  DECLARE_TOOL( GiGaEventActionCommand );

  /// field manager 
  DECLARE_TOOL( GiGaFieldMgr );
  DECLARE_TOOL( ZeroFieldMgr );
  DECLARE_TOOL( GiGaSetSimAttributes );

  // Regions 
  DECLARE_TOOL( GiGaRegionTool );
  DECLARE_TOOL( GiGaRegionsTool );

}

