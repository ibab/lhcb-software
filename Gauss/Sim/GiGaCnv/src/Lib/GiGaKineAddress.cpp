// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/Lib/GiGaKineAddress.cpp,v 1.1 2001-04-23 17:21:39 ibelyaev Exp $ 
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



////////////////////////////////////////////////////////////////////////////////
GiGaKineAddress::GiGaKineAddress(  const CLID&        Clid          , 
				   const std::string& FileName      , 
				   const int          /* SeqInt */  , 
				   const std::string& ContainerName )
  : GenericAddress( GiGaKine_StorageType, Clid )
{
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaKineObject" );
};
////////////////////////////////////////////////////////////////////////////////
GiGaKineAddress::GiGaKineAddress(  const GenericLink& Link          ,
				   const std::string& FileName      , 
				   const std::string& ContainerName , 
				   const std::string& ObjName       )
  : GenericAddress( Link )
{
  setDbName       ( FileName      );
  setContainerName( ContainerName );
  setObjectName   ( ObjName       );
};
////////////////////////////////////////////////////////////////////////////////
GiGaKineAddress::GiGaKineAddress(  const GenericLinkBase & Link     )
  : GenericAddress(  Link ) 
{};
////////////////////////////////////////////////////////////////////////////////
GiGaKineAddress::GiGaKineAddress(  const GenericLinkBase * Link     )
  : GenericAddress(  Link ) 
{};
/// Standard destructor ////////////////////////////////////////////////////////
GiGaKineAddress::~GiGaKineAddress(){};
////////////////////////////////////////////////////////////////////////////////





