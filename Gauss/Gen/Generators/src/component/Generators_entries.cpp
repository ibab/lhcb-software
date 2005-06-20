//  $Id: Generators_entries.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
//====================================================================
//  Generators_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Gen/Generators
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(Generators) {
  DECLARE_TOOL( EvtGenTool ) ;
  DECLARE_ALGORITHM( DumpMC );
  DECLARE_ALGORITHM( DumpMCDecay );
  DECLARE_ALGORITHM( EvtDecayAlg );
  DECLARE_ALGORITHM( SetDecayAlg );
  DECLARE_ALGORITHM( SmearVertexAlg);
  DECLARE_ALGORITHM( SignalDecayAlg ) ;
}
  
