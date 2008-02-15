// $Id: HltBaseTool.cpp,v 1.2 2008-02-15 15:33:11 graven Exp $
// Include files 


// local
#include "HltBase/HltBaseTool.h"
#include "HltBase/HltBase.icpp"

template class HltBase<GaudiHistoTool>;

HltBaseTool::HltBaseTool( const std::string& name,
                          const std::string& type,
                          const IInterface* parent)
  : HltBase<GaudiHistoTool> ( name , type, parent ) {
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
