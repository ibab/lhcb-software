#ifndef      __GIGA_GIGACONVERSION_GIGAGEOMADDRESS_H__
#define      __GIGA_GIGACONVERSION_GIGAGEOMADDRESS_H__  1

///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaGeomAddress: definition of artificial address
///                          for Input Geom GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000



///
class  GiGaGeomAddress: public GenericAddress   
{
  ///
public:
  ///
  GiGaGeomAddress(  const CLID&        Clid          , 
                    const std::string& FileName      , 
                    const int          /* SeqInt */  , 
                    const std::string& ContainerName )
    : GenericAddress( GiGaGeom_StorageType, Clid )
  {
    setDbName        ( FileName          );
    setContainerName ( ContainerName     );
    setObjectName    ( "GiGaGeomObject"  );
  };
  ///
  GiGaGeomAddress(  const GenericLink& Link          ,
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
  GiGaGeomAddress(  const GenericLinkBase & Link     )
    : GenericAddress( Link )
  {};
  ///
  GiGaGeomAddress(  const GenericLinkBase * Link     )
    : GenericAddress( Link )
  {};  
  /// Standard destructor
  virtual ~GiGaGeomAddress(){};
  ///
};

#endif   //  __GIGA_GIGACONVERSION_GIGAGEOMADDRESS_H__
