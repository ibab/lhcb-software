

/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"


/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 


// local
#include "GiGaHitsCnvSvc.h" 


///
///
/// Implementation of GiGaHitsCnvSvc 
///
///
/// Author: Vanya Belyaev
/// Date  : 7 Aug 2000
///

///
extern const IAddrFactory& GiGaHitsAddressFactory ; 
///
static const  SvcFactory<GiGaHitsCnvSvc>                         s_GiGaHitsCnvSvcFactory ; 
const        ISvcFactory&                GiGaHitsCnvSvcFactory = s_GiGaHitsCnvSvcFactory ; 
/// constructor
GiGaHitsCnvSvc::GiGaHitsCnvSvc( const std::string&   ServiceName          , 
  				ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvc(                                      ServiceName          , 
		                                     ServiceLocator       , 
						     GiGaHits_StorageType )
  ///
{
  setAddressFactory(&GiGaHitsAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
}
///
StatusCode GiGaHitsCnvSvc::initialize() { return GiGaCnvSvc::initialize(); };
///  
StatusCode GiGaHitsCnvSvc::finalize()   { return GiGaCnvSvc::finalize  (); };  
///




















