// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/component/L0Muon_load.cpp,v 1.5 2006-10-20 14:48:55 cattanem Exp $
//====================================================================
//  L0Muon_load.cpp
//--------------------------------------------------------------------
//
//  Package    : L0Muon

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(L0Muon) 
{
  DECLARE_ALGORITHM(L0MuonAlg);
  DECLARE_ALGORITHM(L0MuonCandidatesFromRaw);
  DECLARE_ALGORITHM(L0MuonProcessorDatasFromRaw);
}


