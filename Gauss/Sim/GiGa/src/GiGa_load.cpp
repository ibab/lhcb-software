//====================================================================
//  GaudiSvc_load.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi/System
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================


///
#include "GaudiKernel/ISvcFactory.h"
#define DLL_DECL_SERVICE(x)       extern const ISvcFactory&          x##Factory; x##Factory.addRef();
///
#include "GaudiKernel/ICnvFactory.h"
#define DLL_DECL_CONVERTER(x)     extern const ICnvFactory&          x##Factory; x##Factory.addRef();
///
#include "GaudiKernel/IAlgFactory.h"
#define DLL_DECL_ALGORITHM(x)     extern const IAlgFactory&          x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaPhysListFactory.h"
#define DLL_DECL_PLFACTORY(x)     extern const IGiGaPhysListFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaSensDetFactory.h"
#define DLL_DECL_SDFACTORY(x)     extern const IGiGaSensDetFactory&  x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaMagFieldFactory.h"
#define DLL_DECL_MFFACTORY(x)     extern const IGiGaMagFieldFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaStackActionFactory.h"
#define DLL_DECL_STACKFACTORY(x)  extern const IGiGaStackActionFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaTrackActionFactory.h"
#define DLL_DECL_TRACKFACTORY(x)  extern const IGiGaTrackActionFactory& x##Factory; x##Factory.addRef();
///
#include "GiGa/IGiGaStepActionFactory.h"
#define DLL_DECL_STEPFACTORY(x)   extern const IGiGaStepActionFactory& x##Factory; x##Factory.addRef();
///
void GiGa_load() 
{ 
  /// GiGa Service itself
  DLL_DECL_SERVICE      (    GiGaSvc                 ); 
  /// Conversion services and their converters 
  DLL_DECL_SERVICE      (    GiGaKineCnvSvc          );  /// service 
  DLL_DECL_CONVERTER    (    GiGaMCVertexCnv         );  /// converter
  ///
  DLL_DECL_SERVICE      (    GiGaGeomCnvSvc          );  /// service
  DLL_DECL_CONVERTER    (    GiGaIsotopeCnv          );
  DLL_DECL_CONVERTER    (    GiGaElementCnv          );
  DLL_DECL_CONVERTER    (    GiGaMixtureCnv          );
  DLL_DECL_CONVERTER    (    GiGaLVolumeCnv          );
  DLL_DECL_CONVERTER    (    GiGaDetectorElementCnv  );
  ///
  DLL_DECL_SERVICE      (    GiGaHitsCnvSvc          );  /// service 
  /// Algorithms 
  DLL_DECL_ALGORITHM    (    ParticleGunAlgorithm    );
  DLL_DECL_ALGORITHM    (    GiGaStream              );
  DLL_DECL_ALGORITHM    (    GiGaMiscGetEventAlg     );
  /// Physics List
  DLL_DECL_PLFACTORY    (    GiGaPhysListGeantino    );
  DLL_DECL_PLFACTORY    (    GiGaPhysListEm          );
  DLL_DECL_PLFACTORY    (    GiGaPhysListFull        );
  /// Sensitive Detector 
  DLL_DECL_SDFACTORY    (    GiGaSensDetPrint        );
  /// Magnetic Field  
  DLL_DECL_MFFACTORY    (    GiGaMagFieldGlobal      );
  DLL_DECL_MFFACTORY    (    GiGaMagFieldUniform     );
  /// stacking action 
  DLL_DECL_STACKFACTORY (    GiGaStackActionEmpty    );
  /// tracking action 
  DLL_DECL_TRACKFACTORY (    GiGaTrackActionEmpty    );
  /// stepping  action 
  DLL_DECL_STEPFACTORY  (    GiGaStepActionEmpty     );
  ///
}; 
/////////////////////////////////////////////////////////////
extern "C" void GiGa_loadRef() { GiGa_load(); } ; 
/////////////////////////////////////////////////////////////




