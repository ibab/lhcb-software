// $Id: MuonMoniSim_load.cpp,v 1.2 2009-04-29 14:23:21 svecchi Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonMoniSim){
  DECLARE_ALGORITHM(MuonHitChecker)
  DECLARE_ALGORITHM(MuonMultipleScatteringChecker)
}

