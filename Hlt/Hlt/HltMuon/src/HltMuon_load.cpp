// $Id: HltMuon_load.cpp,v 1.2 2006-09-27 13:55:23 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(HltMuon) {
  DECLARE_TOOL( MuonPosTool );
  DECLARE_ALGORITHM( HltMuonRec );
  DECLARE_ALGORITHM( HltMuonRefine );
}
