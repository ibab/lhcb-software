// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaHitsAddressFactory.cpp,v 1.1 2001-04-23 17:44:39 ibelyaev Exp $ 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGaCnv
#include "GiGaCnv/GiGaHitsAddress.h" 
///


//////////////////////////////////////////////////////////////////////////////////////////////////
static const  AddrFactory<GiGaHitsAddress>         s_GiGaHitsAddressFactory(GiGaHits_StorageType);
const        IAddrFactory&GiGaHitsAddressFactory = s_GiGaHitsAddressFactory;
//////////////////////////////////////////////////////////////////////////////////////////////////


