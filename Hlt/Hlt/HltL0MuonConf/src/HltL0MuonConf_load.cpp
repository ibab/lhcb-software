// $Id: HltL0MuonConf_load.cpp,v 1.2 2007-05-22 13:44:46 asatta Exp $
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

DECLARE_FACTORY_ENTRIES(HltL0MuonConf) {
   DECLARE_ALGORITHM( L0MuonConfWithT );
   DECLARE_ALGORITHM( L0MuonAndTConf );
   DECLARE_ALGORITHM( RZMuonMatch );
   DECLARE_ALGORITHM( MuonTConfWithT );
}
