// $Id: MuonID_load.cpp,v 1.3 2003-05-30 09:16:32 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: 
//
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonID) {
  
  DECLARE_ALGORITHM( MuonIDFOI );
  DECLARE_ALGORITHM( MuonDSTPrepare );
  DECLARE_TOOL     ( MuonIDDLLTool );
};


// ============================================================================
// The End 
// ============================================================================
