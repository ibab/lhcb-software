// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:09  ibelyaev
/// the package restructurisation
///
// ============================================================================
#define GIGACNV_GIGAHITSADDRESS_CPP 1 
// ============================================================================
/// Include files
/// GaudiKernel
#include "GaudiKernel/AddrFactory.h"
/// local
#include "GiGaCnv/GiGaHitsAddress.h"
/// from GiGa
#include "GiGaCnv/GiGaHitsAddress.h" 

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaHitsAddress
//
// 10/07/2001 : Ivan Belyaev
//-----------------------------------------------------------------------------

// ============================================================================
/// factory 
// ============================================================================
static const  AddrFactory<GiGaHitsAddress>
s_GiGaHitsAddressFactory(GiGaHits_StorageType);
const        IAddrFactory& GiGaHitsAddressFactory = s_GiGaHitsAddressFactory;

// ============================================================================
/// constructor 
// ============================================================================
GiGaHitsAddress::GiGaHitsAddress(  const CLID&        Clid          , 
                                   const std::string& FileName      , 
                                   const int          /* SeqInt */  , 
                                   const std::string& ContainerName )
  : GenericAddress( GiGaHits_StorageType, Clid )
{
  setDbName        ( FileName          );
  setContainerName ( ContainerName     );
  setObjectName    ( "GiGaHitsObject"  );
};

// ============================================================================
/// constructor 
// ============================================================================
GiGaHitsAddress::GiGaHitsAddress(  const GenericLink& Link          ,
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
GiGaHitsAddress::GiGaHitsAddress(  const GenericLinkBase & Link     )
  : GenericAddress( Link )
{};

// ============================================================================
/// constructor 
// ============================================================================
GiGaHitsAddress::GiGaHitsAddress(  const GenericLinkBase * Link     )
  : GenericAddress( Link )
{};

// ============================================================================
/// destructor
// ============================================================================
GiGaHitsAddress::~GiGaHitsAddress(){};



// ============================================================================
