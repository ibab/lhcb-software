// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/Lib/GiGaHitsAddress.cpp,v 1.2 2001-04-23 17:44:39 ibelyaev Exp $ 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGaCnv
#include "GiGaCnv/GiGaHitsAddress.h" 
///

///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
GiGaHitsAddress::GiGaHitsAddress(  const GenericLinkBase & Link     )
  : GenericAddress( Link )
{};
///////////////////////////////////////////////////////////////////////////////
GiGaHitsAddress::GiGaHitsAddress(  const GenericLinkBase * Link     )
  : GenericAddress( Link )
{};
/// Standard destructor ///////////////////////////////////////////////////////
GiGaHitsAddress::~GiGaHitsAddress(){};
///////////////////////////////////////////////////////////////////////////////

