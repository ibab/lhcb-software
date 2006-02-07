//  $
//====================================================================
//  ITAlgorithms_load.cpp
//--------------------------------------------------------------------
//  
//  Package    : IT/STDAQ
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

void STDAQ_load(){

  DLL_DECL_ALGORITHM( STClustersToRawBankAlg );
  DLL_DECL_ALGORITHM( RawBankToSTClusterAlg );
  DLL_DECL_ALGORITHM( RawBankToSTLiteClusterAlg );

  DLL_DECL_TOOL( ITReadoutTool );
  DLL_DECL_TOOL( TTReadoutTool );

}

extern "C" void STDAQ_loadRef() {
  STDAQ_load();
}






