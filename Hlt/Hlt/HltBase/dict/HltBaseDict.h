#ifndef DICT_HLTBASEDICT_H 
#define DICT_HLTBASEDICT_H 1

#include "HltBase/IHltConfSummaryTool.h"
#include "HltBase/IHltFunctionFactory.h"
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/ITrackConfirmTool.h"
#include "HltBase/ITrackL0CaloMatchTool.h"
#include "HltBase/IMatchTVeloTracks.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/ITrackView.h"
#include "HltBase/IFunctionFactory.h"
#include "HltBase/IBiFunctionFactory.h"
#include "HltBase/ERelations.h"
#include "HltBase/HltTypes.h"
#include "LHCbMath/MD5.h"
#include "HltBase/IConfigTreeEditor.h"
#include "HltBase/stringKey.h"
#include "HltBase/PropertyConfig.h"
#include "HltBase/IPropertyConfigSvc.h"
#include "HltBase/IConfigAccessSvc.h"

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

