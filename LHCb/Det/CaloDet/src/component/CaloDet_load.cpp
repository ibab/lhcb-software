//====================================================================
//  Calo_load.cpp
//--------------------------------------------------------------------
//
//  Package    : Calo
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================

#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"

///
///
/// TMP: all object businedd  should go to LHCbEvent! 
///
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"



#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();


#include "CaloDet/DeCalorimeter.h"
_ImplementDataObjectFactory( DeCalorimeter    ) ;

#include "CaloDet/DeSubCalorimeter.h"
_ImplementDataObjectFactory( DeSubCalorimeter ) ;


void CaloDet_load() 
{
  
  /// Converter factories 
  DLL_DECL_CONVERTER( XmlDeCalorimeterCnv    );
  DLL_DECL_CONVERTER( XmlDeSubCalorimeterCnv );
  
  /// Object factories 
  DLL_DECL_OBJECTFACTORY( DeCalorimeter    ) ; 
  DLL_DECL_OBJECTFACTORY( DeSubCalorimeter ) ; 
  
}

extern "C" void CaloDet_loadRef()     {  CaloDet_load(); }


