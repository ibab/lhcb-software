// $Id :$

//====================================================================
//  DaVinciTools_load.cpp
//--------------------------------------------------------------------
//
//  Package    : DaVinciTools
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

#define DLL_DECL_SERVICE(x)  extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_TOOL(x) extern const IToolFactory& x##Factory; x##Factory.addRef();

void DaVinciTools_load()   {

  /// Algorithms
  DLL_DECL_ALGORITHM( AxPrimVertexAlgorithm );
  DLL_DECL_ALGORITHM( PhysSelectAlgorithm );
  DLL_DECL_ALGORITHM( DaVinciInitialisation );
  DLL_DECL_ALGORITHM( DaVinciFinalisation );
  DLL_DECL_ALGORITHM( MCDecayFinderAlgorithm );

  /// Tools
  DLL_DECL_TOOL( AddFourMom );
  DLL_DECL_TOOL( AxPart2MCParticleAsct );
  DLL_DECL_TOOL( ImpactPar );


}

extern "C" void DaVinciTools_loadRef()    {
  DaVinciTools_load();
}




  
