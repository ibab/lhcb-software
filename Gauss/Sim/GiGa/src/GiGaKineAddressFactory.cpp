/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// GiGa
#include "GiGa/GiGaKineAddress.h" 


///
///
///  Implementation of GiGaKineAddress 
///
///
///  Author Vanya Belyaev 
///  Date   9 Aug 2000 
///  

/// Address Factory definition /////////////////////////////////////////////////////////////////////////////////////
static const  AddrFactory<GiGaKineAddress>                          s_GiGaKineAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&                 GiGaKineAddressFactory = s_GiGaKineAddressFactory;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 





