///
/// from STL 
#include <string> 

///
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"

///
/// from GiGa
#include "GiGa/GiGaKineAddress.h" 


///
///
///  Implementation of GiGaKineAddress 
///
///
///  Author Vanya Belyaev 
///  Date   9 Aug 2000 
///  

///
/// XmlAddress Factory definition
///
static const  AddrFactory<GiGaKineAddress>                                s_GiGaKineAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&                       GiGaKineAddressFactory = s_GiGaKineAddressFactory;

///
/// 
///
GiGaKineAddress::GiGaKineAddress( const CLID&        Clid          ,
				  const std::string& FileName      ,
				  const int          SeqID         ,
				  const std::string& ContainerName ) 
  : GenericAddress( GiGaKine_StorageType, Clid )
{
  { int aux = 0 ; aux = SeqID ; } 
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaInputObject" );
};

///
/// 
///
GiGaKineAddress::GiGaKineAddress( const GenericLink& Link          ,
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
///
///
GiGaKineAddress::GiGaKineAddress( const GenericLinkBase& Link  ) 
  : GenericAddress(  Link ) 
{};
///
///
///
GiGaKineAddress::GiGaKineAddress( const GenericLinkBase* Link  ) 
  : GenericAddress( *Link ) 
{};











