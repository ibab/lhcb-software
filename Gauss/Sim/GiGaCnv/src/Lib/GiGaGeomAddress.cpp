/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#define GIGACNV_GIGAGEOMCNVADDRESS_CPP 1 
/// ===========================================================================
// Include files
/// 
#include <string>
/// GaufiKernel
#include "GaudiKernel/AddrFactory.h"
// local
#include "GiGaCnv/GiGaGeomAddress.h"
//-----------------------------------------------------------------------------
// Implementation file for class : GiGaGeomAddress
//
// 10/07/2001 : Ivan Belyaev
//-----------------------------------------------------------------------------

//=============================================================================
/// Address Factory definition
//=============================================================================
static const  AddrFactory<GiGaGeomAddress>
s_GiGaGeomAddressFactory(GiGaGeom_StorageType);
const        IAddrFactory&GiGaGeomAddressFactory = s_GiGaGeomAddressFactory;

//=============================================================================
// creator
//=============================================================================
GiGaGeomAddress::GiGaGeomAddress(  const CLID&        Clid          , 
                                   const std::string& FileName      , 
                                   const int          /* SeqInt */  , 
                                   const std::string& ContainerName )
  : GenericAddress( GiGaGeom_StorageType, Clid )
{
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaGeomObject"  );
};

//=============================================================================
// creator
//=============================================================================
GiGaGeomAddress::GiGaGeomAddress(  const GenericLink& Link          ,
                                   const std::string& FileName      , 
                                   const std::string& ContainerName , 
                                   const std::string& ObjName       )
  : GenericAddress( Link )
{
  setDbName       ( FileName      );
  setContainerName( ContainerName );
  setObjectName   ( ObjName       );
};

//=============================================================================
// creator
//=============================================================================
GiGaGeomAddress::GiGaGeomAddress(  const GenericLinkBase & Link     )
  : GenericAddress( Link )
{};

//=============================================================================
// creator
//=============================================================================
GiGaGeomAddress::GiGaGeomAddress(  const GenericLinkBase * Link     )
  : GenericAddress( Link )
{};

/// ===========================================================================
/// Standard destructor
/// ===========================================================================
GiGaGeomAddress::~GiGaGeomAddress(){};

/// ===========================================================================


