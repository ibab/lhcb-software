#ifndef      __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
#define      __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__  1

///
#include "GaudiKernel/GenericAddress.h"

///
///   GiGaHitsAddress: definition of artificial address
///                          for Input Kine GiGa store 
///  Author Vanya Belyaev 
///  Date   9 Aug 2000

///
class  GiGaHitsAddress: public GenericAddress   
{
  ///
public:
  ///  
  GiGaHitsAddress(  const CLID&        Clid          , 
                    const std::string& FileName      , 
                    const int          /* SeqInt */  , 
                    const std::string& ContainerName )
    : GenericAddress( GiGaHits_StorageType, Clid )
  {
    setDbName        ( FileName          );
    setContainerName ( ContainerName     );
    setObjectName    ( "GiGaHitsObject"  );
  };
  ///
  GiGaHitsAddress(  const GenericLink& Link          ,
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
  GiGaHitsAddress(  const GenericLinkBase & Link     )
    : GenericAddress( Link )
  {};
  ///
  GiGaHitsAddress(  const GenericLinkBase * Link     )
    : GenericAddress( Link )
  {};
  /// Standard destructor
  virtual ~GiGaHitsAddress(){};
  ///
};
///

#endif   //  __GIGA_GIGACONVERSION_GIGAHitsAddRESS_H__
