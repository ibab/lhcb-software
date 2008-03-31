#ifndef DICT_HLTBASEDICT_H 
#define DICT_HLTBASEDICT_H 1

#include "HltBase/IHltConfSummaryTool.h"
#include "HltBase/IHltFunctionFactory.h"
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/IPrepareCaloSeed.h"
#include "HltBase/IPrepareMuonSeed.h"
#include "HltBase/IPrepareMuonTSeed.h"
#include "HltBase/ITrackConfirmTool.h"
#include "HltBase/ITrackL0CaloMatchTool.h"
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/IL0Value.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/ITrackMatch.h"
#include "HltBase/IFunctionFactory.h"
#include "HltBase/IBiFunctionFactory.h"
#include "HltBase/ERelations.h"
#include "HltBase/HltTypes.h"
#include "HltBase/MD5.h"
#include "HltBase/IConfigTreeEditor.h"

#include "GaudiAlg/ISequencerTimerTool.h"

namespace {
  
  struct _hltbase_instantiations 
  {
    Hlt::Configuration py_hlt_00;
    zen::IntRelation py_hlt_05; 
    Hlt::IntRelation py_hlt_06;
    _hltbase_instantiations(){}
  };  
}

#endif // DICT_HLTBASEDICT_H

