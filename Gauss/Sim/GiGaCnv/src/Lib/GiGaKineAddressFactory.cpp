// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/Lib/GiGaKineAddressFactory.cpp,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// GiGaCnv
#include "GiGaCnv/GiGaKineAddress.h" 


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





