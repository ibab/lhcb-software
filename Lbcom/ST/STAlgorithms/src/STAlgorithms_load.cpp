// $Id: STAlgorithms_load.cpp,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
//====================================================================
//  ITAlgorithms_load.cpp
//--------------------------------------------------------------------
//  
//  Package    : IT/ITAlgorithms
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================

#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
#include "GaudiKernel/IToolFactory.h"

#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_TOOL(x)       extern const IToolFactory& x##Factory; x##Factory.addRef();

void STAlgorithms_load(){

  DLL_DECL_ALGORITHM( MCSTDepositCreator );
  DLL_DECL_ALGORITHM( MCSTDigitCreator );
  DLL_DECL_ALGORITHM( STDigitCreator );
  DLL_DECL_ALGORITHM( STClusterCreator );

  DLL_DECL_TOOL( STAmplifierResponse );
  DLL_DECL_TOOL( STDepositedCharge );
  DLL_DECL_TOOL( STGeantDepositedCharge );
 

}

extern "C" void STAlgorithms_loadRef() {
  STAlgorithms_load();
}






