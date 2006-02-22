//  $Id: Generators_entries.cpp,v 1.7 2006-02-22 22:18:09 robbep Exp $
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
  DECLARE_ALGORITHM( DumpMC );
  DECLARE_ALGORITHM( DumpMCDecay );
  // New structure:

  // Main algorithm
  DECLARE_ALGORITHM( Generation ) ;

  // Luminosity Tools
  DECLARE_TOOL( VariableLuminosity ) ;
  DECLARE_TOOL( FixedLuminosity ) ;
  DECLARE_TOOL( FixedNInteractions ) ;

  // Sample Generation tools
  DECLARE_TOOL( MinimumBias ) ;
  DECLARE_TOOL( Inclusive ) ;
  DECLARE_TOOL( SignalPlain ) ;
  DECLARE_TOOL( SignalRepeatedHadronization ) ;
  DECLARE_TOOL( SignalForcedFragmentation ) ;
  DECLARE_TOOL( Special ) ;

  // Beam Tools
  DECLARE_TOOL( CollidingBeams ) ;
  DECLARE_TOOL( FixedTarget ) ;

  // Decay Tools
  DECLARE_TOOL( EvtGenDecay ) ;

  // Cut Tools
  DECLARE_TOOL( LHCbAcceptance ) ;

  // Full event cut tools
  DECLARE_TOOL( LeptonInAcceptance ) ;


  DECLARE_TOOL( PythiaProduction ) ;
}
  
