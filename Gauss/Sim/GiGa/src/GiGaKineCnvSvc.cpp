

/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"


/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 


// local
#include "GiGaKineCnvSvc.h" 


///
///
/// Implementation of GiGaKineCnvSvc 
///
///
/// Author: Vanya Belyaev
/// Date  : 7 Aug 2000
///


extern const IAddrFactory& GiGaKineAddressFactory ; 

static const  SvcFactory<GiGaKineCnvSvc>                         s_GiGaKineCnvSvcFactory ; 
const        ISvcFactory&                GiGaKineCnvSvcFactory = s_GiGaKineCnvSvcFactory ; 


/// constructor
GiGaKineCnvSvc::GiGaKineCnvSvc( const std::string&   ServiceName          , 
  				ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvc(                                      ServiceName          , 
		                                     ServiceLocator       , 
						     GiGaKine_StorageType )
  ///
{
  setAddressFactory(&GiGaKineAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
}
///
StatusCode GiGaKineCnvSvc::initialize() { return GiGaCnvSvc::initialize(); };  
StatusCode GiGaKineCnvSvc::finalize()   { return GiGaCnvSvc::finalize(); };  





















