/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGa
#include "GiGa/GiGaTrajAddress.h" 
///  
static const  AddrFactory<GiGaTrajAddress>                                s_GiGaTrajAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&                       GiGaTrajAddressFactory = s_GiGaTrajAddressFactory;
///
GiGaTrajAddress::GiGaTrajAddress( const CLID&        Clid          ,
				  const std::string& FileName      ,
				  const int          /* SeqID */   ,
				  const std::string& ContainerName ) 
  : GenericAddress( GiGaTraj_StorageType, Clid )
{
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaTrajectory"  );
};
///
GiGaTrajAddress::GiGaTrajAddress( const GenericLink& Link          ,
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
GiGaTrajAddress::GiGaTrajAddress( const GenericLinkBase& Link  ) 
  : GenericAddress(  Link ) 
{};
///
GiGaTrajAddress::GiGaTrajAddress( const GenericLinkBase* Link  ) 
  : GenericAddress( *Link ) 
{};
///










