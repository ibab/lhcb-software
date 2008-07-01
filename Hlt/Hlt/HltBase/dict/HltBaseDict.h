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
#include "HltBase/IConfigTreeEditor.h"
#include "HltBase/ConfigTreeNodeAlias.h"
#include "HltBase/ConfigTreeNode.h"
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
    std::vector<ConfigTreeNodeAlias> py_hlt_07;
    boost::optional<ConfigTreeNode> py_hlt_08;
    boost::optional<PropertyConfig> py_hlt_09;
    _hltbase_instantiations(){}
  };  
}

#endif // DICT_HLTBASEDICT_H

