//====================================================================
//  L0Calo_load.cpp
//--------------------------------------------------------------------
//
//  Package    : L0Calo
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================

#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"

#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"



#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();


void L0Calo_load() 
{

  ///
  /// Now declare all available converters
  ///


  ///
  /// algorithms 
  ///
  
  DLL_DECL_ALGORITHM( L0CaloAlg     );
  DLL_DECL_ALGORITHM( L0CaloMonit   );
  
}

extern "C" void L0Calo_loadRef()     {  L0Calo_load(); }


