// $Id: HltBaseTool.cpp,v 1.1 2008-01-22 10:24:28 hernando Exp $
// Include files 


// local
#include "HltBase/HltBaseTool.h"
#include "HltBase/HltBase.icpp"

template class HltBase<GaudiHistoTool>;

HltBaseTool::HltBaseTool( const std::string& name,
                          const std::string& type,
                          const IInterface* parent)
  : HltBase<GaudiHistoTool> ( name , type, parent ) {
  create();
}

HltBaseTool::~HltBaseTool() {} 


StatusCode HltBaseTool::initialize() {
  StatusCode sc = HltBase<GaudiHistoTool>::initialize();
  return sc;
}


StatusCode HltBaseTool::finalize() {
  StatusCode sc = HltBase<GaudiHistoTool>::initialize();
  return sc;
}
