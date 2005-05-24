// $Id: CaloDet_load.cpp,v 1.6 2005-05-24 08:25:21 cattanem Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// includes 
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
// 
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

// ============================================================================
/** @file  CaloDet_load.cpp
 *
 * Implementation of \<Package\>_load routine. This routine 
 * is needed for forcing the linker to load all the components of the library
 */
// ============================================================================

#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory;\
                               x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory;\
                               x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory;\
                                x##Factory.addRef();


#include "CaloDet/DeCalorimeter.h"
_ImplementDataObjectFactory( DeCalorimeter    ) ;

#include "CaloDet/DeSubCalorimeter.h"
_ImplementDataObjectFactory( DeSubCalorimeter ) ;

void CaloDet_load() 
{

  /// Object factories 
  DLL_DECL_OBJECTFACTORY( DeCalorimeter    ) ; 
  DLL_DECL_OBJECTFACTORY( DeSubCalorimeter ) ; 

  DLL_DECL_ALGORITHM(  CaloDetTestAlgorithm ) ;
  
};


extern "C" void CaloDet_loadRef()     {  CaloDet_load(); }

// ============================================================================
// The End 
// ============================================================================

