// $Id: MuonID_load.cpp,v 1.7 2005-12-07 06:57:19 pkoppenb Exp $
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
  DECLARE_TOOL     ( MuonIDNSharedHitsTool );
};


// ============================================================================
// The End 
// ============================================================================
