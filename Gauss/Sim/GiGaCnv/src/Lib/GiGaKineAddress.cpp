// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:09  ibelyaev
/// the package restructurisation
///
// ============================================================================
#define GIGACNV_GIGAKINEADDRESS_CPP 1 
// ============================================================================
// Include files
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
// local
#include "GiGaCnv/GiGaKineAddress.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaKineAddress
//
// 10/07/2001 : Ivan Belyaev
//-----------------------------------------------------------------------------

// ============================================================================
/// factory 
// ============================================================================
static const  AddrFactory<GiGaKineAddress>
s_GiGaKineAddressFactory(GiGaKine_StorageType);
const        IAddrFactory&GiGaKineAddressFactory = s_GiGaKineAddressFactory;


// ============================================================================
/// constructor
// ============================================================================
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

// ============================================================================
/// constructor
// ============================================================================
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


// ============================================================================
/// constructor
// ============================================================================
GiGaKineAddress::GiGaKineAddress(  const GenericLinkBase & Link     )
  : GenericAddress(  Link ) 
{};


// ============================================================================
/// constructor
// ============================================================================
GiGaKineAddress::GiGaKineAddress(  const GenericLinkBase * Link     )
  : GenericAddress(  Link )
{};

// ============================================================================
/// destructor 
// ============================================================================
GiGaKineAddress::~GiGaKineAddress(){};

// ============================================================================
