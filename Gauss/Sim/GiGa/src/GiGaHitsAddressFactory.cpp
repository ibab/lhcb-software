/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGa
#include "GiGa/GiGaHitsAddress.h" 
///
static const  AddrFactory<GiGaHitsAddress>                          s_GiGaHitsAddressFactory(GiGaHits_StorageType);
const        IAddrFactory&                 GiGaHitsAddressFactory = s_GiGaHitsAddressFactory;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////




