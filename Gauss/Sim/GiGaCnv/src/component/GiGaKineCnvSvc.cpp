// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaKineCnvSvc.cpp,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
// local
#include "GiGaKineCnvSvc.h" 


///////////////////////////////////////////////////////////////////////////////////////////
extern const IAddrFactory& GiGaKineAddressFactory ; 
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
static const  SvcFactory<GiGaKineCnvSvc>                         s_GiGaKineCnvSvcFactory ; 
const        ISvcFactory&                GiGaKineCnvSvcFactory = s_GiGaKineCnvSvcFactory ; 
///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
GiGaKineCnvSvc::GiGaKineCnvSvc( const std::string&   ServiceName          , 
                                  ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaKine_StorageType )
  ///
{
  setAddressFactory(&GiGaKineAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
};
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaKineCnvSvc::initialize() { return GiGaCnvSvcBase::initialize(); };  
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaKineCnvSvc::finalize()   { return GiGaCnvSvcBase::finalize(); };  
///////////////////////////////////////////////////////////////////////////////////////////





















