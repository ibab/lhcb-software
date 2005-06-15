// $Id: MuonID_load.cpp,v 1.4 2005-06-15 06:26:39 pkoppenb Exp $
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
