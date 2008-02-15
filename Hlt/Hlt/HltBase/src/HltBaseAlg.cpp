// $Id: HltBaseAlg.cpp,v 1.11 2008-02-15 15:33:11 graven Exp $
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
  return HltBase<GaudiHistoAlg>::initialize();
}

StatusCode HltBaseAlg::finalize() {
  return HltBase<GaudiHistoAlg>::initialize();
}
