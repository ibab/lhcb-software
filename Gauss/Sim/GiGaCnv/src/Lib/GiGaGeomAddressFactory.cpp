// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/Lib/GiGaGeomAddressFactory.cpp,v 1.1.1.1 2001-04-23 08:34:14 ibelyaev Exp $ 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGaCnv
#include "GiGaCnv/GiGaGeomAddress.h" 

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




