//  $Id: Generators_entries.cpp,v 1.4 2005-11-17 15:56:02 robbep Exp $
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
  DECLARE_ALGORITHM( Generation ) ;
  DECLARE_TOOL( VariableLuminosity ) ;
  DECLARE_TOOL( FixedNInteractions ) ;
  DECLARE_TOOL( MinimumBias ) ;
  DECLARE_TOOL( CollidingBeams ) ;
  DECLARE_TOOL( FixedTarget ) ;
  DECLARE_TOOL( EvtGenDecay ) ;
  DECLARE_TOOL( LHCbAcceptance ) ;
  DECLARE_TOOL( Inclusive ) ;
  DECLARE_TOOL( SignalPlain ) ;
  DECLARE_TOOL( SignalRepeatedHadronization ) ;
  DECLARE_TOOL( SignalForcedFragmentation ) ;
  DECLARE_TOOL( Special ) ;
}
  
