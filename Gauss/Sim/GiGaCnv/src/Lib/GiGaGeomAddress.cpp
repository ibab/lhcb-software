// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/Lib/GiGaGeomAddress.cpp,v 1.2 2001-04-23 17:44:39 ibelyaev Exp $ 
#define   GIGACNV_GIGAGEOMADDRESS_CPP 1 
/// from STL 
#include <string> 
/// from Gaudi
#include "GaudiKernel/AddrFactory.h"
/// from GiGaCnv
#include "GiGaCnv/GiGaGeomAddress.h" 

///////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////
GiGaGeomAddress::GiGaGeomAddress(  const GenericLinkBase & Link     )
  : GenericAddress( Link )
{};
/////////////////////////////////////////////////////////////////////////
GiGaGeomAddress::GiGaGeomAddress(  const GenericLinkBase * Link     )
  : GenericAddress( Link )
{};  
/// Standard destructor ////////////////////////////////////////////////
GiGaGeomAddress::~GiGaGeomAddress(){};
///////////////////////////////////////////////////////////////////////





