/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGa
#include "GiGa/GiGaGeomAddress.h" 

///
///  Implementation of GiGaGeomAddress 
///
///  Author Vanya Belyaev 
///  Date   9 Aug 2000 
///  

/// Address Factory definition //////////////////////////////////////////////////////////////////////////////////////
static const  AddrFactory<GiGaGeomAddress>                           s_GiGaGeomAddressFactory(GiGaGeom_StorageType);
const        IAddrFactory&                  GiGaGeomAddressFactory = s_GiGaGeomAddressFactory;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




