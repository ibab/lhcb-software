// $Id: HltBaseTool.cpp,v 1.3 2008-05-01 20:25:50 graven Exp $
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
  debug() << " calling HltBase<GaudiHistTool>::initialize() " << endmsg;
  return HltBase<GaudiHistoTool>::initialize();
}


StatusCode HltBaseTool::finalize() {
  debug() << " calling HltBase<GaudiHistTool>::finalize() " << endmsg;
  return HltBase<GaudiHistoTool>::finalize();
}
