// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaKineAddressFactory.cpp,v 1.1 2001-04-23 17:44:40 ibelyaev Exp $ 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// GiGaCnv
#include "GiGaCnv/GiGaKineAddress.h" 


/// Address Factory definition ///////////////////////////////////////////////////////////////////
static const  AddrFactory<GiGaKineAddress>         s_GiGaKineAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&GiGaKineAddressFactory = s_GiGaKineAddressFactory;
////////////////////////////////////////////////////////////////////////////////////////////////// 

