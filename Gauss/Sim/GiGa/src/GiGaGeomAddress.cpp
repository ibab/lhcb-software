///
/// from STL 
#include <string> 

///
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"

///
/// from GiGa
#include "GiGa/GiGaGeomAddress.h" 


///
///
///  Implementation of GiGaGeomAddress 
///
///
///  Author Vanya Belyaev 
///  Date   9 Aug 2000 
///  

extern unsigned char GiGaGeom_StorageType; 

///
/// XmlAddress Factory definition
///
static const  AddrFactory<GiGaGeomAddress>                                s_GiGaGeomAddressFactory(GiGaGeom_StorageType);
const        IAddrFactory&                       GiGaGeomAddressFactory = s_GiGaGeomAddressFactory;

///
/// 
///
GiGaGeomAddress::GiGaGeomAddress( const CLID&        Clid          ,
				  const std::string& FileName      ,
				  const int          SeqID         ,
				  const std::string& ContainerName ) 
  : GenericAddress( GiGaGeom_StorageType, Clid )
{
  { int aux = 0 ; aux = SeqID ; } 
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaInputObject" );
};

///
/// 
///
GiGaGeomAddress::GiGaGeomAddress( const GenericLink& Link          ,
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
GiGaGeomAddress::GiGaGeomAddress( const GenericLinkBase& Link  ) 
  : GenericAddress(  Link ) 
{};
///
///
///
GiGaGeomAddress::GiGaGeomAddress( const GenericLinkBase* Link  ) 
  : GenericAddress( *Link ) 
{};











