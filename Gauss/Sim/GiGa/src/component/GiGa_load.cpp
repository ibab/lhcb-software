/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:54:37  ibelyaev
/// package restructurisation
///
/// ===========================================================================
#define GIGA_GIGA_LOAD_CPP 1 
/// ===========================================================================

///
#include "GaudiKernel/ISvcFactory.h"
#define DLL_DECL_SERVICE(x)   extern const ISvcFactory& x##Factory; \
                                               x##Factory.addRef();
///
#include "GaudiKernel/ICnvFactory.h"
#define DLL_DECL_CONVERTER(x) extern const ICnvFactory& x##Factory; \
                                               x##Factory.addRef();
///
#include "GaudiKernel/IAlgFactory.h"
#define DLL_DECL_ALGORITHM(x) extern const IAlgFactory& x##Factory; \
                                               x##Factory.addRef();
///
#include "GiGa/IGiGaPhysListFactory.h"
#define DLL_DECL_PLFACTORY(x) extern const IGiGaPhysListFactory& x##Factory; \
                                                        x##Factory.addRef();
///
#include "GiGa/IGiGaSensDetFactory.h"
#define DLL_DECL_SDFACTORY(x) extern const IGiGaSensDetFactory& x##Factory; \
                                                       x##Factory.addRef();
///
#include "GiGa/IGiGaMagFieldFactory.h"
#define DLL_DECL_MFFACTORY(x) extern const IGiGaMagFieldFactory& x##Factory; \
                                                        x##Factory.addRef();
///
#include "GiGa/IGiGaStackActionFactory.h"
#define DLL_DECL_STACKFACTORY(x) extern const IGiGaStackActionFactory& \
                                            x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaTrackActionFactory.h"
#define DLL_DECL_TRACKFACTORY(x) extern const IGiGaTrackActionFactory& \
                                            x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaStepActionFactory.h"
#define DLL_DECL_STEPFACTORY(x)  extern const IGiGaStepActionFactory& \
                                            x##Factory; x##Factory.addRef();
// 
#include "GiGa/IGiGaEventActionFactory.h"
#define DLL_DECL_EVENTFACTORY(x) extern const IGiGaEventActionFactory& \
                                            x##Factory; x##Factory.addRef();
///
void GiGa_load() 
{ 
  /// GiGa Service itself
  DLL_DECL_SERVICE      (    GiGaSvc                 ); 
  /// Conversion services and their converters 
  DLL_DECL_ALGORITHM    (    GiGaStream              );
  /// Physics List
  DLL_DECL_PLFACTORY    (    GiGaPhysListGeantino    );
  DLL_DECL_PLFACTORY    (    GiGaPhysListEm          );
  DLL_DECL_PLFACTORY    (    GiGaPhysListFull        );
  /// Sensitive Detector 
  /// DLL_DECL_SDFACTORY    (    GiGaSensDetPrint        );
  /// Magnetic Field  
  DLL_DECL_MFFACTORY    (    GiGaMagFieldGlobal      );
  DLL_DECL_MFFACTORY    (    GiGaMagFieldUniform     );
  /// stacking action 
  DLL_DECL_STACKFACTORY (    GiGaStackActionEmpty    );
  /// tracking action 
  DLL_DECL_TRACKFACTORY (    GiGaTrackActionEmpty    );
  DLL_DECL_TRACKFACTORY (    GiGaTrackActionSimple   );
  DLL_DECL_TRACKFACTORY (    GiGaTrackActionSequence );
  /// stepping  action 
  DLL_DECL_STEPFACTORY  (    GiGaStepActionEmpty     );
  DLL_DECL_STEPFACTORY  (    GiGaStepActionDraw      );
  DLL_DECL_STEPFACTORY  (    GiGaStepActionSequence  );
  /// event action 
  DLL_DECL_EVENTFACTORY (    GiGaEventActionEmpty    );
  DLL_DECL_EVENTFACTORY (    GiGaEventActionDraw     );
  DLL_DECL_EVENTFACTORY (    GiGaEventActionSequence );
  ///
}; 

/// ===========================================================================
/// ===========================================================================
extern "C" void GiGa_loadRef() { GiGa_load(); } ; 

/// ===========================================================================








