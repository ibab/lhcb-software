// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGa_entries.cpp,v 1.1 2001-04-23 08:52:08 ibelyaev Exp $ 
#define GIGA_GIGA_ENTRIES_CPP 1 
//
#include "GaudiKernel/DeclareFactoryEntries.h"

///
#include "GiGa/IGiGaPhysListFactory.h"
#define DECLARE_PLFACTORY(x)     extern const IGiGaPhysListFactory&    x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaSensDetFactory.h"
#define DECLARE_SDFACTORY(x)     extern const IGiGaSensDetFactory&     x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaMagFieldFactory.h"
#define DECLARE_MFFACTORY(x)     extern const IGiGaMagFieldFactory&    x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaStackActionFactory.h"
#define DECLARE_STACKFACTORY(x)  extern const IGiGaStackActionFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaTrackActionFactory.h"
#define DECLARE_TRACKFACTORY(x)  extern const IGiGaTrackActionFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaStepActionFactory.h"
#define DECLARE_STEPFACTORY(x)   extern const IGiGaStepActionFactory&  x##Factory; x##Factory.addRef();
// 
#include "GiGa/IGiGaEventActionFactory.h"
#define DECLARE_EVENTFACTORY(x)  extern const IGiGaEventActionFactory& x##Factory; x##Factory.addRef();
///

DECLARE_FACTORY_ENTRIES(  GiGa ) 
{
  /// GiGa Service itself
  DECLARE_SERVICE      (    GiGaSvc                 ); 
  /// Algorithm
  DECLARE_ALGORITHM    (    GiGaStream              );
  /// Physics List
  DECLARE_PLFACTORY    (    GiGaPhysListGeantino    );
  DECLARE_PLFACTORY    (    GiGaPhysListEm          );
  DECLARE_PLFACTORY    (    GiGaPhysListFull        );
  /// Magnetic Field  
  DECLARE_MFFACTORY    (    GiGaMagFieldGlobal      );
  DECLARE_MFFACTORY    (    GiGaMagFieldUniform     );
  /// stacking action 
  DECLARE_STACKFACTORY (    GiGaStackActionEmpty    );
  /// tracking action 
  DECLARE_TRACKFACTORY (    GiGaTrackActionEmpty    );
  DECLARE_TRACKFACTORY (    GiGaTrackActionSimple   );
  /// stepping  action 
  DECLARE_STEPFACTORY  (    GiGaStepActionEmpty     );
  DECLARE_STEPFACTORY  (    GiGaStepActionDraw      );
  /// event action 
  DECLARE_EVENTFACTORY (    GiGaEventActionEmpty    );
  DECLARE_EVENTFACTORY (    GiGaEventActionDraw     );
  ///
}; 






