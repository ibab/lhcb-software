/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGa
#include "GiGa/GiGaHitsAddress.h" 
///
static const  AddrFactory<GiGaHitsAddress>                                s_GiGaHitsAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&                       GiGaHitsAddressFactory = s_GiGaHitsAddressFactory;
///
GiGaHitsAddress::GiGaHitsAddress( const CLID&        Clid          ,
				  const std::string& FileName      ,
				  const int          /* SeqID */   ,
				  const std::string& ContainerName ) 
  : GenericAddress( GiGaHits_StorageType, Clid )
{
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaInputObject" );
};
///
GiGaHitsAddress::GiGaHitsAddress( const GenericLink& Link          ,
				  const std::string& FileName      , 
				  const std::string& ContainerName , 
				  const std::string& ObjName       )
  : GenericAddress( Link )
{
  setDbName       ( FileName      );
  setContainerName( ContainerName );
  setObjectName   ( ObjName       );
};
///
GiGaHitsAddress::GiGaHitsAddress( const GenericLinkBase& Link  ) 
  : GenericAddress(  Link ) 
{};
///
GiGaHitsAddress::GiGaHitsAddress( const GenericLinkBase* Link  ) 
  : GenericAddress( *Link ) 
{};
///










