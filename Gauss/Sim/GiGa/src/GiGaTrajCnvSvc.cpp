

/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"


/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 


// local
#include "GiGaTrajCnvSvc.h" 


///
///
/// Implementation of GiGaTrajCnvSvc 
///
///
/// Author: Vanya Belyaev
/// Date  : 7 Aug 2000
///

///
extern const IAddrFactory& GiGaTrajAddressFactory ; 
///
static const  SvcFactory<GiGaTrajCnvSvc>                         s_GiGaTrajCnvSvcFactory ; 
const        ISvcFactory&                GiGaTrajCnvSvcFactory = s_GiGaTrajCnvSvcFactory ; 
/// constructor
GiGaTrajCnvSvc::GiGaTrajCnvSvc( const std::string&   ServiceName          , 
  				ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvc(                                      ServiceName          , 
		                                     ServiceLocator       , 
						     GiGaTraj_StorageType )
  ///
{
  setAddressFactory(&GiGaTrajAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
}
///
StatusCode GiGaTrajCnvSvc::initialize() { return GiGaCnvSvc::initialize() ; }; 
/// 
StatusCode GiGaTrajCnvSvc::finalize()   { return GiGaCnvSvc::finalize  () ; };  
///












