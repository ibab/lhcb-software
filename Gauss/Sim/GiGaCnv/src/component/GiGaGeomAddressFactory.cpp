// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaGeomAddressFactory.cpp,v 1.1 2001-04-23 17:44:39 ibelyaev Exp $ 
#define   GIGACNV_GIGAGEOMADDRESS_CPP 1 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGaCnv
#include "GiGaCnv/GiGaGeomAddress.h" 


/// Address Factory definition ///////////////////////////////////////////////////////////////
static const  AddrFactory<GiGaGeomAddress>         s_GiGaGeomAddressFactory(GiGaGeom_StorageType);
const        IAddrFactory&GiGaGeomAddressFactory = s_GiGaGeomAddressFactory;
//////////////////////////////////////////////////////////////////////////////////////////////

