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

#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"

///
///
///

#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();


///
///
///

void GiGa_load() 
{ 
  ///
  /// GiGa Service itself
  DLL_DECL_SERVICE   (    GiGaSvc                 ); 

  ///
  /// Conversion services and their converters 
  DLL_DECL_SERVICE   (    GiGaKineCnvSvc          );  /// service 
  DLL_DECL_CONVERTER (    GiGaMCVertexCnv         );  /// converter

  DLL_DECL_SERVICE   (    GiGaGeomCnvSvc          );  /// service
  //  DLL_DECL_CONVERTER (    GiGaDetectorElementCnv  );  
  DLL_DECL_CONVERTER (    GiGaIsotopeCnv          );
  DLL_DECL_CONVERTER (    GiGaElementCnv          );
  DLL_DECL_CONVERTER (    GiGaMixtureCnv          );
  DLL_DECL_CONVERTER (    GiGaDetElemCnv          );

  ///
  /// Algorithms 
  DLL_DECL_ALGORITHM (    ParticleGunAlgorithm );
  DLL_DECL_ALGORITHM (    GiGaStream           );

}; 

///
///
///

extern "C" void GiGa_loadRef() { GiGa_load(); } ; 

///
///
///


