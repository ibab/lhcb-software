// $Id: CaloDet_load.cpp,v 1.4 2002-03-28 13:47:14 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/12/15 18:28:17  ibelyaev
//  update for new policy of versions and new test algorithm
//
// Revision 1.2  2001/06/29 10:26:18  ibelyaev
// update to use new features of DetDesc v7 package
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
/** @file  Calo_load.cpp
 *
 * Implementation of <Package>_load routine. This routine 
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

