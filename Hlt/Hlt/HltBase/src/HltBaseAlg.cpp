// $Id: HltBaseAlg.cpp,v 1.10 2008-01-22 09:35:00 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "DetDesc/Condition.h"

// local
#include "HltBase/HltBaseAlg.h"

#include "HltBase/HltBase.icpp"

template class HltBase<GaudiHistoAlg>;
HltBaseAlg::HltBaseAlg( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : HltBase<GaudiHistoAlg> ( name , pSvcLocator ) {
  create();
}

HltBaseAlg::~HltBaseAlg() {} 


StatusCode HltBaseAlg::initialize() {
  StatusCode sc = HltBase<GaudiHistoAlg>::initialize();
  return sc;
}

StatusCode HltBaseAlg::finalize() {
  StatusCode sc = HltBase<GaudiHistoAlg>::initialize();
  return sc;
}
