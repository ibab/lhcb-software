// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaHitsCnvSvc.cpp,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
/// Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
/// local
#include "GiGaHitsCnvSvc.h" 

///////////////////////////////////////////////////////////////////////////////////////////
extern const IAddrFactory& GiGaHitsAddressFactory ; 
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
static const  SvcFactory<GiGaHitsCnvSvc>                         s_GiGaHitsCnvSvcFactory ; 
const        ISvcFactory&                GiGaHitsCnvSvcFactory = s_GiGaHitsCnvSvcFactory ; 
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
GiGaHitsCnvSvc::GiGaHitsCnvSvc( const std::string&   ServiceName          , 
                                  ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaHits_StorageType )
  ///
{
  setAddressFactory(&GiGaHitsAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
}
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaHitsCnvSvc::initialize() { return GiGaCnvSvcBase::initialize(); };
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaHitsCnvSvc::finalize()   { return GiGaCnvSvcBase::finalize  (); };  
///////////////////////////////////////////////////////////////////////////////////////////




















