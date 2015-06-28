#ifndef DICT_HLTINTERFACESDICT_H
#define DICT_HLTINTERFACESDICT_H 1

#include "Kernel/IANNSvc.h"
#include "Kernel/IANSvc.h"
#include "Kernel/IANSvc.h"
#include "Kernel/ConfigTreeNodeAlias.h"
#include "Kernel/ConfigTreeNode.h"
#include "Kernel/PropertyConfig.h"
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/IConfigAccessSvc.h"
#include "Kernel/IConfigTreeEditor.h"
#include "Kernel/ReadRoutingBits.h"

namespace {

  struct _hltinterfaces_instantiations
  {
    std::vector<ConfigTreeNodeAlias> py_hlt_07;
    boost::optional<ConfigTreeNode> py_hlt_08;
    boost::optional<PropertyConfig> py_hlt_09;
    _hltinterfaces_instantiations(){}
  };
}


#endif // DICT_HLTINTERFACESDICT_H
