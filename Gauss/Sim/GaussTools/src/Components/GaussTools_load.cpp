// $Id: GaussTools_load.cpp,v 1.7 2003-04-11 17:55:36 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2003/04/09 12:05:58  witoldp
// change of name of GiGaTrackAction
//
// Revision 1.5  2003/04/06 19:07:33  ibelyaev
//  update foe newer GiGa, add new tools
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
  DECLARE_ALGORITHM       (    GetGiGaHitsAlg          ) ;
  DECLARE_ALGORITHM       (    PrintEventAlg           ) ;
  DECLARE_ALGORITHM       (    GiGaFlushAlgorithm      ) ;
  DECLARE_ALGORITHM       (    GaussInitialisation     ) ;
  
  /// Magnetic Field  
  DECLARE_GiGaFactory     (    GiGaMagFieldGlobal      ) ;
  DECLARE_GiGaFactory     (    GiGaMagFieldUniform     ) ;
  
  /// Streams 
  DECLARE_ALGORITHM       (    GiGaInputStream         ) ;
  DECLARE_ALGORITHM       (    GiGaOutputStream        ) ;

  /// Physics Lists 
  DECLARE_GiGaFactory     (    GiGaPhysListModular     ) ;
  DECLARE_GiGaFactory     (    GiGaPhysListGeantino    ) ;
 
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetPrint        ) ;
  DECLARE_GiGaFactory     (    GiGaSensDetBudget       ) ;

  /// stacking action 
  DECLARE_GiGaFactory     (    GiGaStackActionEmpty    ) ;

  /// tracking action 
  DECLARE_GiGaFactory     (    GiGaTrackActionEmpty    );
  DECLARE_GiGaFactory     (    GaussTrackAction        );
  DECLARE_GiGaFactory     (    GiGaTrackActionSequence );

  /// stepping  action 
  DECLARE_GiGaFactory     (    GiGaStepActionEmpty     ) ;
  DECLARE_GiGaFactory     (    GiGaStepActionDraw      ) ;
  DECLARE_GiGaFactory     (    GiGaStepActionSequence  ) ;
  DECLARE_GiGaFactory     (    GaussStepAction         ) ;
  DECLARE_GiGaFactory     (    CutsStepAction          ) ;

  /// run action
  DECLARE_GiGaFactory     (    GiGaRunActionSequence   ) ;
  DECLARE_GiGaFactory     (    GiGaRunActionCommand    ) ;
  DECLARE_GiGaFactory     (    TrCutsRunAction         ) ;

  /// event action 
  DECLARE_GiGaFactory     (    GiGaEventActionEmpty    ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionDraw     ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionSequence ) ;
  DECLARE_GiGaFactory     (    GiGaEventActionCommand  ) ;

  /// field manager 
  DECLARE_GiGaFactory     (    GiGaFieldMgr            ) ;
  DECLARE_GiGaFactory     (    GiGaEraseFieldMgr       ) ;
  DECLARE_GiGaFactory     (    GiGaSetSimAttributes    ) ;
  
};
// ============================================================================


// ============================================================================
extern "C" void GaussTools_loadRef() { GaussTools_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
