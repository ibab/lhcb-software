// $Id: HltBaseAlg.cpp,v 1.12 2008-05-01 20:25:50 graven Exp $
// Include files 

// local
#include "HltBase/HltBaseAlg.h"
#include "HltBase/HltBase.icpp"

template class HltBase<GaudiHistoAlg>;
HltBaseAlg::HltBaseAlg( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : HltBase<GaudiHistoAlg> ( name , pSvcLocator ) {
}

HltBaseAlg::~HltBaseAlg() {} 


StatusCode HltBaseAlg::initialize() {
  debug() << "HltBaseAlg::initialize()" << endmsg;
  return HltBase<GaudiHistoAlg>::initialize();
}

StatusCode HltBaseAlg::finalize() {
  debug() << "HltBaseAlg::finalize()" << endmsg;
  return HltBase<GaudiHistoAlg>::finalize();
}
